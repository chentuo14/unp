#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>

#define SOCKET_BUFFER_SIZE (32768U)

void *thread_function(void *arg)
{
	int len = 0;
	int fd = *((int *)(arg));
	char buf[500];
	int cnt = 0;

	while(1) {
		len = sprintf(buf, "Hi, main process, cnt = %d", cnt++);
		write(fd, buf, len);

		len = read(fd, buf, 500);
		buf[len] = '\0';
		printf("%s\n", buf);

		sleep(5);
	}

	return NULL;
}

int main()
{
	int ret;
	int sockets[2];
	int bufferSize = SOCKET_BUFFER_SIZE;
	pthread_t thread;

	ret = socketpair(AF_UNIX, SOCK_SEQPACKET, 0, sockets);
	if(ret == -1) {
		printf("socketpair create error!\n");
		return -1;
	}

	setsockopt(sockets[0], SOL_SOCKET, SO_SNDBUF, &bufferSize, sizeof(bufferSize));
	setsockopt(sockets[0], SOL_SOCKET, SO_RCVBUF, &bufferSize, sizeof(bufferSize));
	setsockopt(sockets[1], SOL_SOCKET, SO_SNDBUF, &bufferSize, sizeof(bufferSize));
	setsockopt(sockets[1], SOL_SOCKET, SO_RCVBUF, &bufferSize, sizeof(bufferSize));

	pthread_create(&thread, NULL, thread_function, (void *)(&sockets[1]));

	int len = 0;
	int fd = sockets[0];
	char buf[500];
	int cnt = 0;

	while(1) {
		len = read(fd, buf, 500);
		buf[len] = '\0';
		printf("%s\n", buf);

		len = sprintf(buf, "Hi, thread process, cnt = %d", cnt++);
		write(fd, buf, len);
	}
	return 0;
}
