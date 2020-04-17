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
#include <string>
#include <sys/types.h>
#include <io.h>
#ifndef _SSIZE_T_DEFINED
typedef int ssize_t;
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
namespace { // private module-only namespace

	static const unsigned char wsbase64_enc_map[64] =
	{
		'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
		'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
		'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd',
		'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
		'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x',
		'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7',
		'8', '9', '+', '/'
	};

	static int wsbase64_encode(char *dst, unsigned int *dlen, const char *src, unsigned int slen)
	{
		unsigned int i, n;
		int C1, C2, C3;
		char *p;
		unsigned int dlennnd = *dlen;

		if (slen == 0)
		{
			*dlen = 0;
			return(0);
		}

		n = (slen << 3) / 6;

		switch ((slen << 3) - (n * 6))
		{
		case  2: n += 3; break;
		case  4: n += 2; break;
		default: break;
		}

		if (*dlen < n + 1)
		{
			*dlen = n + 1;
			return -1;
		}

		n = (slen / 3) * 3;

		for (i = 0, p = dst; i < n; i += 3)
		{
			C1 = *src++;
			C2 = *src++;
			C3 = *src++;

			*p++ = wsbase64_enc_map[(C1 >> 2) & 0x3F];
			*p++ = wsbase64_enc_map[(((C1 & 3) << 4) + (C2 >> 4)) & 0x3F];
			*p++ = wsbase64_enc_map[(((C2 & 15) << 2) + (C3 >> 6)) & 0x3F];
			*p++ = wsbase64_enc_map[C3 & 0x3F];
		}

		if (i < slen)
		{
			C1 = *src++;
			C2 = ((i + 1) < slen) ? *src++ : 0;

			*p++ = wsbase64_enc_map[(C1 >> 2) & 0x3F];
			*p++ = wsbase64_enc_map[(((C1 & 3) << 4) + (C2 >> 4)) & 0x3F];

			if ((i + 1) < slen)
				*p++ = wsbase64_enc_map[((C2 & 15) << 2) & 0x3F];
			else *p++ = '=';

			*p++ = '=';
		}

		*dlen = p - dst;
		*p = 0;

		return 0;
	}
	static int wsCreateKey(char* dst, unsigned int* dlen)
	{
		int ret = -1;
		int key1 = rand() % 65536;
		int key2 = rand() % 65536;
		int key3 = rand() % 65536;
		int key4 = rand() % 65536;

		char key[17] = { '\0' };
		sprintf(key, "%04x%04x%04x%04x", key1, key2, key3, key4);
		ret = wsbase64_encode(dst, dlen, key, 17);

		return ret;
	}
	socket_t hostname_connect(const std::string& hostname, int port) {
		struct addrinfo hints;
		struct addrinfo *result;
		struct addrinfo *p;
		int ret;
		socket_t sockfd = INVALID_SOCKET;
		char sport[16];
		memset(&hints, 0, sizeof(hints));
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;
		snprintf(sport, 16, "%d", port);
		if ((ret = getaddrinfo(hostname.c_str(), sport, &hints, &result)) != 0)
		{
			fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(ret));
			return 1;
		}
		for (p = result; p != NULL; p = p->ai_next)
		{
			sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
			if (sockfd == INVALID_SOCKET) { continue; }
			if (connect(sockfd, p->ai_addr, p->ai_addrlen) != SOCKET_ERROR) {
				break;
			}
			closesocket(sockfd);
			sockfd = INVALID_SOCKET;
		}
		freeaddrinfo(result);
		return sockfd;
	}

	std::string protocalUpgrade(const std::string& url, const std::string& host) {
		std::string reqMethod;
		reqMethod = "GET " + url + " HTTP/1.1" + "\r\n";
		//sprintf(reqMethod, "GET %s HTTP/1.1\r\n", url);

		std::string reqHost;
		reqHost = "Host: " + host + "\r\n";

		char reqWSKey[512];
		char Key[25] = { '\0' };//
		unsigned int KeyLen = 25;
		const char* pWebSocket_Key = "MIbiW+L6OU6IgOE0Tbhbbw==";
		int ckRet = wsCreateKey(Key, &KeyLen);
		if (ckRet == 0)
		{
			pWebSocket_Key = Key;
		}
		sprintf(reqWSKey, "Sec-WebSocket-Key: %s\r\n", pWebSocket_Key);

		std::string queryStr;
		queryStr += reqMethod;
		queryStr += reqHost;
		queryStr += "Connection: Upgrade\r\n";
		queryStr += "Upgrade: websocket\r\n";
		queryStr += reqWSKey;
		queryStr += "Sec-WebSocket-Version: 13\r\n";
		//	queryStr += "Origin: file://\r\n";
		queryStr += "\r\n";

		return queryStr;
	}

} // end of module-only namespace

