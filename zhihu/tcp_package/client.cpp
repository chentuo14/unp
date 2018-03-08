#include <iostream>
#include <string.h>
#include <math.h>
#include <sys/signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>


#define PORT 6666

using namespace std;

#include "protocol.h"

class TCPClient
{
public:
	TCPClient(int argc, char *argv[]);
	~TCPClient();

	bool send_to_serv(unsigned char *data_buffer, int datasize, uint16_t wOpcode);
	bool send_file();
	void run();

private:
	int conn_fd;
	struct sockaddr_in serv_addr;
};

TCPClient::TCPClient(int argc, char *argv[])
{
	if(argc != 3) {
		std::cout<<"Usage: [-a] [serv_address]"<<std::endl;
		exit(1);
	}

	memset(&serv_addr, 0, sizeof(struct sockaddr_in));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);

	for(int i=0;i<argc;i++) {
		if(strcmp("-a", argv[i]) == 0) {
			if(inet_aton(argv[i+1], &serv_addr.sin_addr) == 0) {
				std::cout<<"invaild server ip address"<<std::endl;
				exit(1);
			}
			break;
		}
	}

	if(serv_addr.sin_addr.s_addr == 0) {
		std::cout<<"Usage: [-a] [serv_address]"<<std::endl;
		exit(1);
	}

	conn_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(conn_fd < 0) {
		my_err("connect", __LINE__);
	}

	if(connect(conn_fd, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr)) < 0) {
		my_err("connect", __LINE__);
	}
}

TCPClient::~TCPClient()
{
	close(conn_fd);
}

bool TCPClient::send_to_serv(unsigned char *data_buffer, int datasize, uint16_t wOpcode)
{
	NetPacket send_packet;
	send_packet.Header.wDataSize = datasize + sizeof(NetPacketHeader);
	send_packet.Header.wOpcode = wOpcode;

	memcpy(send_packet.Data, data_buffer, datasize);

	if(send(conn_fd, &send_packet, send_packet.Header.wDataSize, 0))
		return true;
	else
		return false;
}

bool TCPClient::send_file()
{
	unsigned char send_buffer[1024];
	string filename;
	int fd;
	struct stat file_buffer;
	File_message file_message;
	File_data file_data;
	int nsize = 0;
	int sum_size = 0;

	memset(send_buffer, 0, sizeof(send_buffer));
	std::cout<<"请输入要发送的文件路径及文件名"<<std::endl;
	getline(cin, filename);

	if(fd = open(filename.c_str(), O_RDONLY) < 0) {
		my_err("open file error", __LINE__);
	}
	if(stat(filename.c_str(), &file_buffer) < 0) {
		my_err("stat file error", __LINE__);
	}

	strcpy(file_message.filename, filename.c_str());
	file_message.filesize = file_buffer.st_size;

	if(send_to_serv((unsigned char *)&file_message, sizeof(file_data), NET_TEST1) < 0) {
		std::cout<<"向服务器发送文件信息失败"<<std::endl;
	}

	close(fd);
	if((fd = open(filename.c_str(), O_RDONLY)) < 0) {
		my_err("打开文件失败", __LINE__);
	}

	while(nsize = read(fd, send_buffer, sizeof(send_buffer))) {
		memset(&file_data, 0, sizeof(file_data));
		strcpy(file_data.filename, filename.c_str());
		memcpy(file_data.buffer, send_buffer, nsize);

		send_to_serv((unsigned char *)&file_data, nsize+sizeof(file_data.filename), NET_TEST2);
		sum_size += nsize;
	}

	if(sum_size == file_buffer.st_size) {
		std::cout<<"发送文件成功"<<std::endl;
		close(fd);
		return true;
	} else {
		std::cout<<"发送文件出错"<<std::endl;
		close(fd);
		return false;
	}
}

void TCPClient::run()
{
	send_file();
}

int main(int argc, char *argv[])
{
	TCPClient client(argc, argv);
	client.run();
	sleep(10);
}

