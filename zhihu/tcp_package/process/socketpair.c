#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

#define SOCKET_BUFFER_SIZE ((32)*(1024))

#define BUFFER_SIZE  (256)

int main(int argc, char *argv[])
{
	int sockets[2];
	int result;
	int bufferSize;

	pthread_t thread;

	char buf[BUFFER_SIZE] = {0};
	int readlen;

	int father_fd;
	int child_fd;

	result = socketpair(AF_UNIX, SOCK_SEQPACKET, 0, sockets);
	if(result == -1) {
		printf("sockpair error!\n");
		return -1;
	}

	bufferSize = SOCKET_BUFFER_SIZE;
	setsockopt(sockets[0], SOL_SOCKET, SO_SNDBUF, &bufferSize, sizeof(bufferSize));
	setsockopt(sockets[0], SOL_SOCKET, SO_RCVBUF, &bufferSize, sizeof(bufferSize));
	setsockopt(sockets[1], SOL_SOCKET, SO_SNDBUF, &bufferSize, sizeof(bufferSize));
	setsockopt(sockets[1], SOL_SOCKET, SO_RCVBUF, &bufferSize, sizeof(bufferSize));

	father_fd = sockets[0];
	child_fd = sockets[1];

	result = fork();
	if(result == -1) {
		printf("fork error!\n");
		return -1;
	} else if(result == 0) {
		printf("(1) send to main thread : %s\n", "Hello,world!");
		write(child_fd, "Hello, world!", sizeof("Hello,world!"));

		readlen = read(child_fd, buf, BUFFER_SIZE);
		buf[readlen] = '\0';
		printf("(4) recv from main thread : %s\n", buf);
	} else if(result > 0) {
		sleep(1);

		readlen = read(father_fd, buf, BUFFER_SIZE);
		buf[readlen] = '\0';
		printf("(2) recv from child thread : %s\n", buf);

		printf("(3) send to child thread : %s\n", "TECH-PRO");
		write(father_fd, "TECH-PRO", sizeof("TECH-PRO"));

		sleep(1);
	}

	return 0;
}
