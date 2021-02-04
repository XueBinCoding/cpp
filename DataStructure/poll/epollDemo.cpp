#include <iostream>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
 
using namespace std;
 
#define MAXLINE 5
#define OPEN_MAX 100
#define LISTENQ 20
#define SERV_PORT 5000
#define INFTIM 1000

 int main(){
    int i, maxi, listenfd, connfd, sockfd,epfd,nfds, portnumber;
    char line[MAXLINE];
    socket_t client;

    portnumber =5000;
    struct epoll_event ev,events[20];
    epfd = epoll_create(256);
    listenfd = socket(AF_INET,SOCK_STREAM,0);

    memset(&serveraddr,0,sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port=htons(portnumber);

    bind(listenfd,(sockaddr *)&serveraddr,sizeof(serveraddr));
    ev.data.fd=listenfd;
    ev.events=EPOLLIN|EPOLLET;
    epoll_ctl(epfd,EPOLL_CTL_ADD,listenfd,&ev);

    maxi = 0;
    int bOut =0;
    for(;;){
        if(bOut ==1) break;
        nfds=epoll_wait(epfd,events,20,-1);

        for(i=0;i<nfds;++i){
            if(events[i].data.fd=listenfd){
                connfd =accept(listenfd,(sockaddr *)&clientaddr,&client);
                if(connfd <0){
                    perror("connfd<0");
                    return(1);
                }
                 char *str = inet_ntoa(clientaddr.sin_addr);
                 cout << "accapt a connection from " << str << endl;
                 //设置用于读操作的文件描述符

                 setnonblocking(connfd);
                 ev.data.fd=connfd;
                 //设置用于注测的读操作事件
                 ev.events=EPOLLIN | EPOLLET;
                 //ev.events=EPOLLIN;
                 //注册ev
                epoll_ctl(epfd,EPOLL_CTL_ADD,connfd,&ev);
            }
            else if(events[i].events & EPOLLIN){
                
            }
        }
        
    }
    

 }

 