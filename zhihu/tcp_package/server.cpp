#include <iostream>
#include <cstring>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>

using namespace std;

#include "protocol.h"

#define PORT 666
#define LISTEN_SIZE 1023
#define EPOLL_SIZE  1023

class TCPServer
{
public:
	TCPServer();
	~TCPServer();

	void acceptClient();

	void closeClient(int i);

	bool dealwithpacket(int conn_fd, unsigned char *recv_data, 
			uint16_t wOpcode, int datasize);
	bool server_recv(int conn_fd);

	void run();

private:
	int sock_fd;
	int conn_fd;
	int epollfd;
	socklen_t cli_len;
	struct epoll_event event;
	struct epoll_event *events;
	struct sockaddr_in cli_addr;
	struct sockaddr_in serv_addr;
};

TCPServer::TCPServer()
{
	sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(sock_fd < 0) {
		my_err("socket", __LINE__);
	}

	int optval = 1;
	if(setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, (void *)&optval, sizeof(int)) < 0) {
		my_err("setsock", __LINE__);
	}

	cli_len = sizeof(struct sockaddr_in);
	events = (struct epoll_event *)malloc(sizeof(struct epoll_event)*EPOLL_SIZE);

	epollfd = epoll_create(EPOLL_SIZE);
	if(epollfd == -1) {
		my_err("epollfd", __LINE__);
	}
	event.events = EPOLLIN;
	event.data.fd = sock_fd;
	if(epoll_ctl(epollfd, EPOLL_CTL_ADD, sock_fd, &event) < 0) {
		my_err("epoll_ctl", __LINE__);
	}
}

TCPServer::~TCPServer()
{
	close(sock_fd);
	std::cout<<"服务器成功退出"<<std::endl;
}

void TCPServer::acceptClient()
{
	conn_fd = accept(sock_fd, (struct sockaddr *)&cli_addr, &cli_len);
	if(conn_fd < 0) {
		my_err("accept", __LINE__);
	}
	event.events = EPOLLIN|EPOLLRDHUP;
	event.data.fd = conn_fd;
	if(epoll_ctl(epollfd, EPOLL_CTL_ADD, conn_fd, &event) < 0) {
		my_err("epoll", __LINE__);
	}
	std::cout<<"a connet is connected, ip is "<<inet_ntoa(cli_addr.sin_addr)<<endl;
}

void TCPServer::closeClient(int i)
{
	std::cout<<"a connect is quit,ip is "<<inet_ntoa(cli_addr.sin_addr)<<std::endl;
	epoll_ctl(epollfd, EPOLL_CTL_DEL, events[i].data.fd, &event);
	close(events[i].data.fd);
}

bool TCPServer::dealwithpacket(int conn_fd, unsigned char *recv_data,
		uint16_t wOpcode, int datasize)
{
	int fd;
	if(wOpcode == 1) {
		File_message *file_message = (File_message *)recv_data;
		strcat(file_message->filename, ".down");

		if((fd = open(file_message->filename, O_RDWR|O_APPEND|O_CREAT, 0777)) < 0) {
			std::cout<<"创建文件失败"<<std::endl;
			return false;
		}
	} else if(wOpcode == 2) {
		File_data *file_data=(File_data *)recv_data;
		strcat(file_data->filename, ".down");
		if((fd = open(file_data->filename, O_RDWR|O_APPEND)) < 0) {
			std::cout<<"打开文件失败"<<std::endl;
			return false;
		}
		if(write(fd, file_data->buffer, datasize-sizeof(file_data->filename)) < 0) {
			std::cout<<"写入文件失败"<<std::endl;
			return false;
		}
		close(fd);
	}
	return true;
}

bool TCPServer::server_recv(int conn_fd)
{
	int nrecvsize = 0;
	int sum_recvsize = 0;
	int packetsize;
	int datasize;
	unsigned char recv_buffer[10000];

	memset(recv_buffer, 0, sizeof(recv_buffer));

	while(sum_recvsize != sizeof(NetPacketHeader)) {
		nrecvsize = recv(conn_fd, recv_buffer+sum_recvsize, 
				sizeof(NetPacketHeader)-sum_recvsize, 0);
		if(nrecvsize == 0) {
			std::cout<<"从客户端接收数据失败"<<std::endl;
			return false;
		}
		sum_recvsize+=nrecvsize;
	}

	NetPacketHeader *phead = (NetPacketHeader *)recv_buffer;
	packetsize = phead->wDataSize;
	datasize = packetsize - sizeof(NetPacketHeader);

	while(sum_recvsize != packetsize) {
		nrecvsize = recv(conn_fd, recv_buffer+sum_recvsize, packetsize-sum_recvsize, 0);
		if(nrecvsize == 0) {
			std::cout<<"从客户端接收数据失败"<<std::endl;
			return false;
		}
		sum_recvsize+=nrecvsize;
	}

	dealwithpacket(conn_fd, (unsigned char *)(phead+1), phead->wOpcode, datasize);
}

void TCPServer::run()
{
	while(1) {
		int sum = 0, i;
		sum = epoll_wait(epollfd, events, EPOLL_SIZE, -1);
		for(i=0;i<sum;i++) {
			if(events[i].data.fd == sock_fd) {
				acceptClient();
			} else if(events[i].events & EPOLLIN) {
				server_recv(events[i].data.fd);
			}
			if(events[i].events & EPOLLRDHUP) {
				closeClient(i);
			}
		}
	}
}

int main()
{
	TCPServer server;
	server.run();

	return 0;
}
