#ifndef _EPOLL_FUN_H_
#define _EPOLL_FUN_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <strings.h>

void setnonblocking(int sock);
void add_fd_to_epoll_in(int epollfd, int fd);
void mod_fd_to_epoll_in(int epollfd, int fd);
void mod_fd_to_epoll_out(int epollfd, int fd);
void del_fd_to_epoll(int epollfd, int fd);

void disconnect_info(int sock);

#endif
