#include "intf.h"

#include "socket.h"
#include <algorithm>
#define MAXEVENTS 64

struct MSG
{
    string strBuff;
    SOCKET s;
};

// 采用epoll异步机制实现服务器
int main()
{
    // 创建socket
    string ip = "192.168.176.131";
    int port = 1122;
    TcpSocket tcp(AF_INET, SOCK_STREAM);
    tcp.Bind(ip, port);
    tcp.Listen(MAXEVENTS);

    // 设置socket为O_NONBLOCK
    int flags = fcntl(tcp.m_Sock, F_GETFL, 0);
    if(!(flags & O_NONBLOCK))
    {
        flags |= O_NONBLOCK;
    }
    int exflags = fcntl(tcp.m_Sock, F_SETFL, 0);
    if(exflags == -1)
    {
        cout << "fcntl F_SETFL o_NONBLOCK faild" << endl;
        return -1;
    }
    // 创建epoll,该参数在新版本的linux中已经没有作用了
    int epollftd = epoll_create(MAXEVENTS);
    if(epollftd == -1)
    {
        cout << "epoll create faild" << endl;
        return -1;
    }
    // 将服务区socket描述符添加到epoll
    epoll_event event;
    event.events = EPOLLIN | EPOLLET;
    event.data.fd = tcp.m_Sock;

    int ret = epoll_ctl(epollftd, EPOLL_CTL_ADD, tcp.m_Sock, &event);
    if(ret == -1)
    {
        cout << "epoll_ctrl EPOLL_CTL_ADD faild" << endl;
        return -1;
    }

    epoll_event *pEvents = (epoll_event *)calloc(MAXEVENTS, sizeof(epoll_event));
    if(!pEvents)
    {
        cout << "calloc epoll events faild" << endl;
        return -1;
    }
    // 事件处理
    while(1)
    {
        int nEventNum = epoll_wait(epollftd, pEvents, MAXEVENTS, 500);
        for(int i = 0; i < nEventNum; i ++)
        {
            // 新连接到来
            if(pEvents[i].data.fd == tcp.m_Sock)
            {
                cout << "准备接收客户端的连接" << endl;
                SOCKET client = tcp.Accept();
                cout << "有客户端连接" << endl;
                // 添加到队列中
                epoll_event ev;
                ev.events = EPOLLIN | EPOLLET;
                ev.data.fd = client;
                epoll_ctl(epollftd, EPOLL_CTL_ADD, client, &ev);
            }
            else if(pEvents[i].events & EPOLLIN)
            {
                string strBuff = tcp.Recv(pEvents[i].data.fd);
                cout << "接收到客户端数据：" << strBuff << endl;
                if(strBuff == "EOF")
                {
                    cout << "客户端：" << pEvents[i].data.fd << "请求断开连接" << endl;
                } 
                
                string strSend = "";
                transform(strBuff.begin(), strBuff.end(), back_inserter(strSend), ::toupper);
               
                MSG msg;
                msg.strBuff = strSend;
                msg.s = pEvents[i].data.fd;

                epoll_event ev;
                ev.data.fd = pEvents[i].data.fd;
                ev.events = EPOLLOUT | EPOLLET;
                ev.data.ptr = (void *)&msg;
                epoll_ctl(epollftd, EPOLL_CTL_MOD, pEvents[i].data.fd, &ev);
            }
            else if(pEvents[i].events & EPOLLOUT)
            {
                MSG *pMsg = (MSG*)pEvents[i].data.ptr;
                cout << "向客户端输出信息:" << pMsg->strBuff << endl;
                int ret = tcp.Send(pMsg->s, pMsg->strBuff);
                event.data.fd = pMsg->s;
                event.events = EPOLLIN | EPOLLET;
                epoll_ctl(epollftd, EPOLL_CTL_MOD, pMsg->s, &event);
            }
            else
            {
                cout << "其它的处理" << endl;
            }
        }
    }
    
    return 0;
} 