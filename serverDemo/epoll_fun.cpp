#include <sys/epoll.h>
#include "epoll_fun.h"

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

void add_fd_to_epoll_in(int epollfd, int fd)
{
	struct epoll_event event;
	event.data.fd = fd;
	event.events = EPOLLIN|EPOLLET;
	epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event);
}

void mod_fd_to_epoll_in(int epollfd, int fd)
{
	struct epoll_event event;
	event.data.fd = fd;
	event.events = EPOLLIN|EPOLLET;
	epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &event);
}

void mod_fd_to_epoll_out(int epollfd, int fd)
{
	struct epoll_event event;
	event.data.fd = fd;
	event.events = EPOLLOUT|EPOLLET;
	epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &event);
}

void del_fd_to_epoll(int epollfd, int fd)
{
	struct epoll_event event;
	event.data.fd = fd;
	event.events = EPOLLIN|EPOLLET;
	epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, &event);
}

void disconnect_info(int sock)
{
	struct sockaddr_in clientaddr;
	socklen_t clilen = sizeof(struct sockaddr_in);
	char *str_clientaddr;
	bzero(&clientaddr, sizeof(struct sockaddr_in));
	if(!getpeername(sock, (struct sockaddr *)&clientaddr, &clilen)) {
		str_clientaddr = inet_ntoa(clientaddr.sin_addr);
		std::cout<<"disconnect from "<<str_clientaddr<<std::endl;
	}
}
