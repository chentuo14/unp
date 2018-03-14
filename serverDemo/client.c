#include <stdio.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>

#define MAXLINE 80
#define SERV_PORT 5000
#define MESSAGE "Hello"

typedef struct{
	int len;
	char data[1024];
}NetPacket;

int main(int argc, char *argv[])
{
	char buf[MAXLINE];

	int sockfd = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in servaddr = {0};
	servaddr.sin_family = AF_INET;
	inet_pton(AF_INET, "192.168.1.63", &servaddr.sin_addr);
	servaddr.sin_port = htons(SERV_PORT);

	if(connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) != 0)
	{
		printf("connected failed:%d,%s",errno, strerror(errno));
		return 1;
	}
	while(1)
	{
		write(sockfd, MESSAGE, sizeof(MESSAGE));
		int count = read(sockfd, buf, MAXLINE);

		printf("Response from server: %s\n", buf);
		sleep(6);
	}

	close(sockfd);
	return 0;
}
