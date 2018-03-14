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

#define MAX_EVENTS 1000
#define MAXLINE    5
#define SERV_PORT  8000
#define LISTENQ    20
#define OPEN_MAX   100

void Process(int listenfd);

int main(void)
{
	int i, maxi, listenfd;
	ssize_t n;
	char line[MAXLINE];
	

	struct sockaddr_in serveraddr;
	listenfd = socket(AF_INET, SOCK_STREAM, 0);

	bzero(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(SERV_PORT);
	serveraddr.sin_addr.s_addr = INADDR_ANY;

	bind(listenfd, (sockaddr *)&serveraddr, sizeof(serveraddr));
	listen(listenfd, LISTENQ);

	pid_t pid = fork();
	if(pid < 0) {
		perror("fork");
		exit(1);
	} else if(pid == 0) { 	//child
		Process(listenfd);
		return 0;
	} else {				//parent

	}
}

void Process(int listenfd)
{
	struct epoll_event ev, events[MAX_EVENTS];
	int nfds, sockfd;
	socklen_t clilen;
	struct sockaddr_in clientaddr;
	ssize_t n;
	char line[MAXLINE];
	char buf[20];
	int epfd = epoll_create(MAX_EVENTS);

	ev.data.fd = listenfd;
	ev.events = EPOLLIN|EPOLLET;
	epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &ev);

	for( ; ; ) { 
		nfds = epoll_wait(epfd, events, 20, 5000);

		for(int i=0;i<nfds;i++) {
			if(events[i].data.fd == listenfd) {
				sockfd = accept(listenfd, (struct sockaddr *)&clientaddr, &clilen);
				if(sockfd < 0) {
					continue;
				}
				ev.data.fd = sockfd;
				ev.events = EPOLLIN|EPOLLET;
				epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &ev);
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
				ev.events = EPOLLOUT|EPOLLET;
				epoll_ctl(epfd, EPOLL_CTL_MOD, sockfd, &ev);
			} else if(events[i].events & EPOLLOUT) {
				sockfd = events[i].data.fd;
				if(!strcmp(line, "Hello")) {
					strcpy(buf, "World");
					write(sockfd, buf, 6);
				}
				ev.data.fd = sockfd;
				ev.events = EPOLLIN|EPOLLET;
				epoll_ctl(epfd, EPOLL_CTL_MOD, sockfd, &ev);
			}
		}
	}
}
