#ifndef _PROTOCOL_H
#define _PROTOCOL_H

#define NET_PACKET_DATA_SIZE 5000

struct NetPacketHeader {
	unsigned short wDataSize;
	unsigned short wOpcode;
};

struct NetPacket {
	NetPacketHeader Header;
	unsigned char Data[NET_PACKET_DATA_SIZE];
};

enum eNetOpcode {
	NET_TEST1 = 1,
	NET_TEST2 = 2
};

struct File_message {
	char filename[100];
	long filesize;
};

struct File_data {
	char filename[100];
	unsigned char buffer[1024];
};

void my_err(const char *err_string, int line)
{
	std::cerr<<"line:"<<line<<std::endl;
	perror(err_string);
	exit(1);
}
	

#endif 
