#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define TTL       64
#define BUFF_SIZE 30

void error_handling(char *message);

int main(int argc, char *argv[])
{
	//send socket
	int sender_socket;
	//broadcast address
	struct sockaddr_in broadcast_addr;
	FILE *fp;
	char buff[BUFF_SIZE];

	int opt_so_broadcast = 1;
	if(argc != 3) {
		printf("Usage: %s <GroupIP> <Port> \n", argv[0]);
		exit(1);
	}

	sender_socket = socket(PF_INET, SOCK_DGRAM, 0);
	memset(&broadcast_addr, 0, sizeof(broadcast_addr));
	broadcast_addr.sin_family = AF_INET;
	broadcast_addr.sin_addr.s_addr = inet_addr(argv[1]);
	broadcast_addr.sin_port = htons(atoi(argv[2]));

	//open broadcast option
	setsockopt(sender_socket, SOL_SOCKET, SO_BROADCAST, (void *)&opt_so_broadcast, sizeof(opt_so_broadcast));
	if((fp = fopen("word_file.txt", "r")) == NULL) {
		error_handling("fopen() error");
	}

	while(fgets(buff, BUFF_SIZE, fp)) {
		printf("word_file:%s", buff);
		sendto(sender_socket, buff, strlen(buff), 0,
				(struct sockaddr *)&broadcast_addr, sizeof(broadcast_addr));
		sleep(2);
	}

	fclose(fp);
	close(sender_socket);
	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
