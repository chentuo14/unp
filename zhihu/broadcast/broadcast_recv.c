#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define TTL 64
#define BUFF_SIZE 30

void error_handling(char *message);

int main(int argc, char *argv[])
{
	int receiver_socket;
	struct sockaddr_in receiver_addr;
	int str_len;
	char buff[BUFF_SIZE];

	if(argc != 2) {
		printf("Usage: %s <GroupIP> <Port> ", argv[0]);
		exit(1);
	}

	receiver_socket = socket(PF_INET, SOCK_DGRAM, 0);
	memset(&receiver_addr, 0, sizeof(receiver_addr));
	receiver_addr.sin_family = AF_INET;
	receiver_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	receiver_addr.sin_port = htons(atoi(argv[1]));

	if(bind(receiver_socket, (struct sockaddr *)&receiver_addr,
				sizeof(receiver_addr)) == -1) {
		error_handling("bind() error");
	}

	while(1) {
		str_len = recvfrom(receiver_socket, buff, BUFF_SIZE-1, 0, NULL, 0);
		if(str_len < 0) {
			break;
		}
		buff[BUFF_SIZE] = 0;
		fputs(buff, stdout);
	}
	close(receiver_socket);
	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
