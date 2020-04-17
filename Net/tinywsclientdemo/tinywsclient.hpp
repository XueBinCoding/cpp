#ifndef TINYWSCLIENT_H
#define TINYWSCLIENT_H
#ifdef _WIN32
#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)
#define _CRT_SECURE_NO_WARNINGS // _CRT_SECURE_NO_WARNINGS for sscanf errors in MSVC2013 Express
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <fcntl.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#pragma comment( lib, "ws2_32" )
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <io.h>
#ifndef _SSIZE_T_DEFINED
//typedef int ssize_t;
#define _SSIZE_T_DEFINED
#endif
#ifndef _SOCKET_T_DEFINED
typedef SOCKET socket_t;
#define _SOCKET_T_DEFINED
#endif
#ifndef snprintf
#define snprintf _snprintf_s
#endif
#if _MSC_VER >=1600
// vs2010 or later
#include <stdint.h>
#else
typedef __int8 int8_t;
typedef unsigned __int8 uint8_t;
typedef __int32 int32_t;
typedef unsigned __int32 uint32_t;
typedef __int64 int64_t;
typedef unsigned __int64 uint64_t;
#endif
#define socketerrno WSAGetLastError()
#define SOCKET_EAGAIN_EINPROGRESS WSAEINPROGRESS
#define SOCKET_EWOULDBLOCK WSAEWOULDBLOCK
#else
#include <fcntl.h>
#include <netdb.h>
#include <netinet/tcp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdint.h>
#ifndef _SOCKET_T_DEFINED
typedef int socket_t;
#define _SOCKET_T_DEFINED
#endif
#ifndef INVALID_SOCKET
#define INVALID_SOCKET (-1)
#endif
#ifndef SOCKET_ERROR
#define SOCKET_ERROR   (-1)
#endif
#define closesocket(s) ::close(s)
#include <errno.h>
#define socketerrno errno
#define SOCKET_EAGAIN_EINPROGRESS EAGAIN
#define SOCKET_EWOULDBLOCK EWOULDBLOCK
#endif

#include <vector>
#include <string>

//#include "websocketbase.hpp"
#include "common.hpp"
#include "tinySSL.h"

#include "mbedtls/net.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/error.h"
#include "mbedtls/certs.h"

namespace tinywsclient {

	//struct secureSession {
	//	mbedtls_net_context      mbedNetCtx;
	//	mbedtls_ssl_context      mbedSslCtx;
	//	mbedtls_ssl_config       mbedSslConf;
	//	mbedtls_ctr_drbg_context mbedDrbgCtx;
	//	mbedtls_entropy_context  mbedEtpyCtx;
	//	mbedtls_x509_crt         mbedX509Crt;
	//};

	class websocketSecure {
	public:
		typedef enum readyStateValues { CLOSING, CLOSED, CONNECTING, OPEN } readyStateValues;
		struct wsheader_type {
			unsigned header_size;
			bool fin;
			bool mask;
			enum opcode_type {
				CONTINUATION = 0x0,
				TEXT_FRAME = 0x1,
				BINARY_FRAME = 0x2,
				CLOSE = 8,
				PING = 9,
				PONG = 0xa,
			} opcode;
			int N0;
			uint64_t N;
			uint8_t masking_key[4];
		};

		std::vector<uint8_t> rxbuf;
		std::vector<uint8_t> txbuf;
		std::vector<uint8_t> receivedData;

		socket_t sockfd;
		tiny_ssl_ctx *sessionfd;
		readyStateValues readyState;
		bool isRxBad;
		bool useSSL;
		bool useMask;  //base64 encode

		websocketSecure(bool useMask, bool sslSupport) :
			readyState(OPEN), useMask(true)
			, isRxBad(false) ,useSSL(true){
			useSSL = sslSupport;
			if (useSSL) {
				sessionfd = (tiny_ssl_ctx *)calloc(1, sizeof(struct tiny_ssl_ctx));
			}
		}

		~websocketSecure() {
		}
		readyStateValues getReadyState()const {
			return readyState;
		}

		void poll(int timeout) { // timeout in milliseconds
			if (readyState == CLOSED) {
				if (timeout > 0) {
					timeval tv = { timeout / 1000, (timeout % 1000) * 1000 };
					select(0, NULL, NULL, NULL, &tv);
				}
				return;
			}
			if (timeout != 0) {
				fd_set rfds;
				fd_set wfds;
				timeval tv = { timeout / 1000, (timeout % 1000) * 1000 };
				FD_ZERO(&rfds);
				FD_ZERO(&wfds);
				FD_SET(sockfd, &rfds);
				if (txbuf.size()) { FD_SET(sockfd, &wfds); }
				select(sockfd + 1, &rfds, &wfds, 0, timeout > 0 ? &tv : 0);
			}

			while (txbuf.size()) {
				if (useSSL) {
					int ret = tiny_ssl_write(sessionfd, (const char *)&txbuf[0], txbuf.size());
					if (ret < 0) {
						break;
					}
					else if (ret <= 0) {
						readyState = CLOSED;
						break;
					}
					else {
						txbuf.erase(txbuf.begin(), txbuf.begin() + ret);
					}
				}
				else {
					int ret = ::send(sockfd, (char*)&txbuf[0], txbuf.size(), 0);
					if (ret < 0 && (socketerrno == SOCKET_EWOULDBLOCK || socketerrno == SOCKET_EAGAIN_EINPROGRESS)) {
						break;
					}
					else if (ret <= 0) {
						closesocket(sockfd);
						readyState = CLOSED;
						break;
					}
					else {
						txbuf.erase(txbuf.begin(), txbuf.begin() + ret);
					}
				}

			}
			while (true) {
				// FD_ISSET(0, &rfds) will be true
				int N = rxbuf.size();
				int ret;
				rxbuf.resize(N + 1500);
				if (useSSL) {
					ret = tiny_ssl_read(sessionfd, (unsigned char*)&rxbuf[0] + N, 1500);
					if (ret == MBEDTLS_ERR_SSL_WANT_READ) {
						rxbuf.resize(N);
						break;
					}
					else if (ret < 0) {
						rxbuf.resize(N);
						readyState = CLOSED;
						tiny_ssl_free(sessionfd);
						break;
					}
					else {
						rxbuf.resize(N + ret);
					}
				}
				else{
					ret = recv(sockfd, (char*)&rxbuf[0] + N, 1500, 0);
					if (false) {}
					else if (ret < 0 && (socketerrno == SOCKET_EWOULDBLOCK || socketerrno == SOCKET_EAGAIN_EINPROGRESS)) {
						rxbuf.resize(N);
						break;
					}
					else if (ret <= 0) {
						rxbuf.resize(N);
						closesocket(sockfd);
						readyState = CLOSED;
						break;
					}
					else {
						rxbuf.resize(N + ret);
					}
				}

			}

			if (!txbuf.size() && readyState == CLOSING) {
				readyState = CLOSED;
			}
		}

		void getReceive(std::string& message) {
			if (isRxBad) {
				return;
			}
			while (true) {
				wsheader_type ws;
				if (rxbuf.size() < 2) { return; /* Need at least 2 */ }
				const uint8_t * data = (uint8_t *)&rxbuf[0]; // peek, but don't consume
				ws.fin = (data[0] & 0x80) == 0x80;
				ws.opcode = (wsheader_type::opcode_type) (data[0] & 0x0f);
				ws.mask = (data[1] & 0x80) == 0x80;
				ws.N0 = (data[1] & 0x7f);
				ws.header_size = 2 + (ws.N0 == 126 ? 2 : 0) + (ws.N0 == 127 ? 8 : 0) + (ws.mask ? 4 : 0);
				if (rxbuf.size() < ws.header_size) { return; /* Need: ws.header_size - rxbuf.size() */ }
				int i = 0;
				if (ws.N0 < 126) {
					ws.N = ws.N0;
					i = 2;
				}
				else if (ws.N0 == 126) {
					ws.N = 0;
					ws.N |= ((uint64_t)data[2]) << 8;
					ws.N |= ((uint64_t)data[3]) << 0;
					i = 4;
				}
				else if (ws.N0 == 127) {
					ws.N = 0;
					ws.N |= ((uint64_t)data[2]) << 56;
					ws.N |= ((uint64_t)data[3]) << 48;
					ws.N |= ((uint64_t)data[4]) << 40;
					ws.N |= ((uint64_t)data[5]) << 32;
					ws.N |= ((uint64_t)data[6]) << 24;
					ws.N |= ((uint64_t)data[7]) << 16;
					ws.N |= ((uint64_t)data[8]) << 8;
					ws.N |= ((uint64_t)data[9]) << 0;
					i = 10;
					if (ws.N & 0x8000000000000000ull) {
						isRxBad = true;
						fprintf(stderr, "ERROR: Frame has invalid frame length. Closing.\n");
						close();
						return;
					}
				}
				if (ws.mask) {
					ws.masking_key[0] = ((uint8_t)data[i + 0]) << 0;
					ws.masking_key[1] = ((uint8_t)data[i + 1]) << 0;
					ws.masking_key[2] = ((uint8_t)data[i + 2]) << 0;
					ws.masking_key[3] = ((uint8_t)data[i + 3]) << 0;
				}
				else {
					ws.masking_key[0] = 0;
					ws.masking_key[1] = 0;
					ws.masking_key[2] = 0;
					ws.masking_key[3] = 0;
				}

				// Note: The checks above should hopefully ensure this addition
				//       cannot overflow:
				if (rxbuf.size() < ws.header_size + ws.N) { return; /* Need: ws.header_size+ws.N - rxbuf.size() */ }

				// We got a whole message, now do something with it:
				if (false) {}
				else if (
					ws.opcode == wsheader_type::TEXT_FRAME
					|| ws.opcode == wsheader_type::BINARY_FRAME
					|| ws.opcode == wsheader_type::CONTINUATION
					) {
					if (ws.mask) { for (size_t i = 0; i != ws.N; ++i) { rxbuf[i + ws.header_size] ^= ws.masking_key[i & 0x3]; } }
					receivedData.insert(receivedData.end(), rxbuf.begin() + ws.header_size, rxbuf.begin() + ws.header_size + (size_t)ws.N);// just feed
					if (ws.fin) {
						//callable((const std::vector<uint8_t>) receivedData);
						std::string stringMessage(receivedData.begin(), receivedData.end());
						receivedData.erase(receivedData.begin(), receivedData.end());
						std::vector<uint8_t>().swap(receivedData);// free memory
						message = stringMessage; //return stringMessage;
					}
				}
				else if (ws.opcode == wsheader_type::PING) {
					if (ws.mask) { for (size_t i = 0; i != ws.N; ++i) { rxbuf[i + ws.header_size] ^= ws.masking_key[i & 0x3]; } }
					std::string data(rxbuf.begin() + ws.header_size, rxbuf.begin() + ws.header_size + (size_t)ws.N);
					sendData(wsheader_type::PONG, data.size(), data.begin(), data.end());
				}
				else if (ws.opcode == wsheader_type::PONG) {}
				else if (ws.opcode == wsheader_type::CLOSE) { close(); }
				else { fprintf(stderr, "ERROR: Got unexpected WebSocket message.\n"); close(); }

				rxbuf.erase(rxbuf.begin(), rxbuf.begin() + ws.header_size + (size_t)ws.N);
			}
		}
		void sendPing() {
			std::string empty;
			sendData(wsheader_type::PING, empty.size(), empty.begin(), empty.end());
		}

		void sendString(const std::string& message) {
			sendData(wsheader_type::TEXT_FRAME, message.size(), message.begin(), message.end());
		}
		void sendBinary(const std::string& message) {
			sendData(wsheader_type::BINARY_FRAME, message.size(), message.begin(), message.end());
		}

		void sendBinary(const std::vector<uint8_t>& message) {
			sendData(wsheader_type::BINARY_FRAME, message.size(), message.begin(), message.end());
		}
		template<class Iterator>
		void sendData(wsheader_type::opcode_type type, uint64_t message_size, Iterator message_begin, Iterator message_end) {
			// TODO:
			// Masking key should (must) be derived from a high quality random
			// number generator, to mitigate attacks on non-WebSocket friendly
			// middleware:
			const uint8_t masking_key[4] = { 0x12, 0x34, 0x56, 0x78 };
			// TODO: consider acquiring a lock on txbuf...
			if (readyState == CLOSING || readyState == CLOSED) { return; }
			std::vector<uint8_t> header;
			header.assign(2 + (message_size >= 126 ? 2 : 0) + (message_size >= 65536 ? 6 : 0) + (useMask ? 4 : 0), 0);
			header[0] = 0x80 | type;
			if (false) {}
			else if (message_size < 126) {
				header[1] = (message_size & 0xff) | (useMask ? 0x80 : 0);
				if (useMask) {
					header[2] = masking_key[0];
					header[3] = masking_key[1];
					header[4] = masking_key[2];
					header[5] = masking_key[3];
				}
			}
			else if (message_size < 65536) {
				header[1] = 126 | (useMask ? 0x80 : 0);
				header[2] = (message_size >> 8) & 0xff;
				header[3] = (message_size >> 0) & 0xff;
				if (useMask) {
					header[4] = masking_key[0];
					header[5] = masking_key[1];
					header[6] = masking_key[2];
					header[7] = masking_key[3];
				}
			}
			else { // TODO: run coverage testing here
				header[1] = 127 | (useMask ? 0x80 : 0);
				header[2] = (message_size >> 56) & 0xff;
				header[3] = (message_size >> 48) & 0xff;
				header[4] = (message_size >> 40) & 0xff;
				header[5] = (message_size >> 32) & 0xff;
				header[6] = (message_size >> 24) & 0xff;
				header[7] = (message_size >> 16) & 0xff;
				header[8] = (message_size >> 8) & 0xff;
				header[9] = (message_size >> 0) & 0xff;
				if (useMask) {
					header[10] = masking_key[0];
					header[11] = masking_key[1];
					header[12] = masking_key[2];
					header[13] = masking_key[3];
				}
			}
			// N.B. - txbuf will keep growing until it can be transmitted over the socket:
			txbuf.insert(txbuf.end(), header.begin(), header.end());
			txbuf.insert(txbuf.end(), message_begin, message_end);
			if (useMask) {
				size_t message_offset = txbuf.size() - message_size;
				for (size_t i = 0; i != message_size; ++i) {
					txbuf[message_offset + i] ^= masking_key[i & 0x3];
				}
			}
		}

		void close() {
			if (readyState == CLOSING || readyState == CLOSED) { return; }
			readyState = CLOSING;
			uint8_t closeFrame[6] = { 0x88, 0x80, 0x00, 0x00, 0x00, 0x00 }; // last 4 bytes are a masking key
			std::vector<uint8_t> header(closeFrame, closeFrame + 6);
			txbuf.insert(txbuf.end(), header.begin(), header.end());
		}

		websocketSecure* createWebsocketSecure(const std::string& ip, const std::string& host,
												unsigned int port, const std::string& url) {
			sockfd = hostname_connect(ip, port);
			if (useSSL) {
				// mbedtls client init
				tiny_ssl_init(sessionfd, sockfd, const_cast<char*>(ip.c_str()));
				tiny_ssl_handshake(sessionfd);
			}
			
			std::string queryStr = protocalUpgrade(url, host);
			const char* cp = queryStr.c_str();
			int remaining = queryStr.length();
			int n_written;
			while (remaining) {
				if (useSSL) {
					n_written = tiny_ssl_write(sessionfd, cp, remaining);
				}
				else{
					n_written = send(sockfd, cp, remaining, 0);
				}
				
				if (n_written <= 0) {
					return NULL;
				}
				remaining -= n_written;
				cp += n_written;
			}
			unsigned char buf[1024];
			int result;
			while (1) {
				if (useSSL) {
					result = tiny_ssl_read(sessionfd, buf, sizeof(buf));
				}
				else{
					result = recv(sockfd, (char *)buf, sizeof(buf), 0);
				}
				if (result < 0) {
					return NULL;
				}
				break;
			}
			int flag = 1;
			setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, sizeof(flag)); // Disable Nagle's algorithm
#ifdef _WIN32
			u_long on = 1;
			ioctlsocket(sockfd, FIONBIO, &on);
#else
			fcntl(sockfd, F_SETFL, O_NONBLOCK);
#endif
			return this;
		}
	};
}

#endif // !TINYWSCLIENT_H



