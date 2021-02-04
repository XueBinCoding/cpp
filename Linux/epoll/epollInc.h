#ifndef EPOLLINC_H
#define EPOLLINC_H


#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include<fcntl.h>
#include <sys/types.h>  
#include <sys/stat.h>
#include <sys/wait.h>

#include <iostream>
#include <string.h>
#include <exception>

#include <sys/socket.h>
#include <netinet/in.h>     // sockaddr_in, "man 7 ip" ,htons
#include <poll.h>             //poll,pollfd
#include <arpa/inet.h>   //inet_addr,inet_aton
#include <unistd.h>        //read,write
#include <netdb.h>         //gethostbyname

#include <sys/epoll.h>

using namespace std;

#endif //
