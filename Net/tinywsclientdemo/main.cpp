#include "tinywsclient.hpp"
#include "websocketbase.hpp"
#include "assert.h"
#include <iostream>

using namespace tinywsclient;
int main() {
#ifdef _WIN32
	INT rc;
	WSADATA wsaData;

	rc = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (rc) {
		printf("WSAStartup Failed.\n");
		return 1;
	}
#endif
	//tinywsclient::WebSocket::pointer ws;
	websocketSecure *ws = new websocketSecure(true,false);
	assert(ws);
	std::string ip = "123.207.136.134";
	std::string host = "123.207.136.134:9010";
	std::string url = "/ajaxchattest";
	unsigned int port = 9010;
	ws->createWebsocketSecure(ip,host,port,url);
	while (ws->getReadyState() != tinywsclient::websocketSecure::CLOSED) {
		ws->poll(50);
		std::string  recv;
		ws->getReceive(recv);
		if (recv != "")
		{
			std::cout << "receive message" << std::endl;
			std::cout << recv << std::endl;
		}
		//ws->dispatch(aikit::messageReceived);
	}
}