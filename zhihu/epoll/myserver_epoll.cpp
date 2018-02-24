#include <iostream>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <strings.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

using namespace std;

const int SERV_PORT = 8000;
const int MAX_EVENT = 5;
const int MAXLINE = 100;

void setnonblocking(int sock)
{
	int opts;
	opts = fcntl(sock, F_GETFL);
	if(opts < 0) {
		perror("fcntl(sock, GETFL)");
		exit(1);
	}
	opts = opts | O_NONBLOCK;
	if(fcntl(sock, F_SETFL, opts) < 0) {
		perror("fcntl(sock, SETFL, opts)");
		exit(1);
	}
}

int main(void)
{
	int listenfd, connfd, sockfd;
	int epfd, nfds;
	int n, count;
	char line[MAXLINE];
	struct epoll_event ev, events[MAX_EVENT];
	struct sockaddr_in clientaddr;
	struct sockaddr_in serveraddr;

	epfd = epoll_create(256);
	if(epfd == -1) {
		perror("epoll_create");
		exit(EXIT_FAILURE);
	}


	if((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket");
		exit(1);
	}
	setnonblocking(listenfd);

	ev.events = EPOLLIN | EPOLLET;
	ev.data.fd = listenfd;
	if(epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &ev) == -1) {
		perror("epoll_ctl");
		exit(EXIT_FAILURE);
	}


	bzero(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(SERV_PORT);
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	
	if(bind(listenfd, (struct sockaddr *)&serveraddr, 
				sizeof(serveraddr)) < 0) {
		perror("bind");
		exit(EXIT_FAILURE);
	}

	listen(listenfd, 20);
	printf("Accepting connecting ... \n");

	for( ; ; ) {
		nfds = epoll_wait(epfd, events, MAX_EVENT, 500);
		if(nfds == -1) {
			perror("epoll_wait");
			exit(EXIT_FAILURE);
		}

		for(n=0;n<nfds;n++) {
			if(events[n].data.fd == listenfd) {
				socklen_t cliaddr_len = sizeof(clientaddr);
				connfd = accept(listenfd, (struct sockaddr *)&clientaddr,
						&cliaddr_len);
				std::cout<<"accept a connect"<<std::endl;
				if(connfd == -1) {
					perror("accept");
					exit(EXIT_FAILURE);
				}
				setnonblocking(connfd);
				ev.data.fd = connfd;
				ev.events = EPOLLIN | EPOLLET;
				epoll_ctl(epfd, EPOLL_CTL_ADD, connfd, &ev);
			} else if(events[n].events & EPOLLIN) {
				//TODO:read
				if((sockfd = events[n].data.fd) < 0) {
					std::cout<<"sockfd < 0"<<std::endl;
					continue;
				}
				if((count = read(sockfd, line, MAXLINE)) < 0) {
					if(errno == ECONNRESET) {
						close(sockfd);
						events[n].data.fd = -1;
					} else {
						std::cout<<"readline error"<<std::endl;
					}
				} else if(n == 0) {
					close(sockfd);
					events[n].data.fd = -1;
				} 

				std::cout<<"count: "<<count<<std::endl;
				ev.data.fd = sockfd;
				ev.events = EPOLLOUT | EPOLLET;
				epoll_ctl(epfd, EPOLL_CTL_MOD, sockfd, &ev);
			} else if(events[n].events & EPOLLOUT) {
				//TODO:write
				std::cout<<"coming EPOLLOUT"<<std::endl;
				sockfd = events[n].data.fd;
				//if(strcmp(line, "Hello") == 0) {
					write(sockfd, line, MAXLINE);
				//}
				ev.data.fd = sockfd;
				ev.events = EPOLLIN | EPOLLET;
				epoll_ctl(epfd, EPOLL_CTL_MOD, sockfd, &ev);
				//TODO:else
			}
		}
	}
}
