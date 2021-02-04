#include "epollInc.h"

#include "socket.h"

#define AMXEVENTS 64

struct MSG
{
    std::string strBuff;
    SOCKET s;
}

//use epoll realize client

int main()
{
    std::string ip = "192.168.176.131";
    int port = 1122;
    TcpSocket tcp(AF_INET, SOCK_STREAM);
    tcp.ConnectSocket(ip, port, SOCK_CLIENT);
    int pid =fork();
    if(pid <0)
    {
        std::cout<<"create thread fail"<<std::endl;
    }
    else if(pid ==0)
    {
        
    }
}


