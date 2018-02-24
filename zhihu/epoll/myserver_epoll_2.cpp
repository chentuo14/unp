#include <iostream>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>

using namespace std;

#define MAXLINE   5
#define OPEN_MAX  100
#define LISTENQ   20
#define SERV_PORT 8000
#define INFTIM    1000

int main(void)
{
	int i, maxi, listenfd, connfd, sockfd, epfd, nfds;
	ssize_t n;
	char line[MAXLINE];
	char buf[20];
	socklen_t clilen;

	struct epoll_event ev, events[20];
	epfd = epoll_create(256);
	struct sockaddr_in clientaddr;
	struct sockaddr_in serveraddr;
	listenfd = socket(AF_INET, SOCK_STREAM, 0);

	ev.data.fd = listenfd;
	ev.events = EPOLLIN | EPOLLET;
	epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &ev);
	
	bzero(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(SERV_PORT);
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);

	bind(listenfd, (sockaddr *)&serveraddr, sizeof(serveraddr));
	listen(listenfd, LISTENQ);

	for( ; ; ) {
		nfds = epoll_wait(epfd, events, 20, 500);

		for(i=0;i<nfds;i++) {
			if(events[i].data.fd == listenfd) {
				clilen = sizeof(clientaddr);
				connfd = accept(listenfd, (sockaddr *)&clientaddr, &clilen);
				ev.data.fd = connfd;
				ev.events = EPOLLIN | EPOLLET;
				epoll_ctl(epfd, EPOLL_CTL_ADD, connfd, &ev);
			} else if(events[i].events & EPOLLIN) {
				if((sockfd = events[i].data.fd) < 0) {
					continue;
				}
				if((n = read(sockfd, line, MAXLINE)) < 0) {
					if(errno == ECONNRESET) {
						close(sockfd);
						events[i].data.fd = -1;
					} else {
						std::cout<<"readline error"<<std::endl;
					}
				} else if(n == 0) {
					close(sockfd);
					events[i].data.fd = -1;
				}
				ev.data.fd = sockfd;
				ev.events = EPOLLOUT | EPOLLET;
				epoll_ctl(epfd, EPOLL_CTL_MOD, sockfd, &ev);
			} else if(events[i].events & EPOLLOUT) {
				sockfd = events[i].data.fd;
				if(!strcmp(line, "Hello")) {
					strcpy(buf, "World");
					write(sockfd, buf, 6);
				}
				ev.data.fd = sockfd;
				ev.events = EPOLLIN | EPOLLET;
				epoll_ctl(epfd, EPOLL_CTL_MOD, sockfd, &ev);
			}
		}
	}
}
