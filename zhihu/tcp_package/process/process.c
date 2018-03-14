#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>

int main(int argc, char *argv[])
{
	int pid[3];
	int sockets[2];
	int bufferSize;
	int ret;
	int father_fd, child_fd;
	char buf[1024] = {0};
	char *child1_buf = "child 1\n";
	char *child2_buf = "child 2\n";

	ret = socketpair(AF_UNIX, SOCK_SEQPACKET, 0, sockets);
	if(ret < 0) {
		printf("socketpair error!\n");
		return -1;
	}

	bufferSize = 1024;
	setsockopt(sockets[0], SOL_SOCKET, SO_SNDBUF, &bufferSize, sizeof(bufferSize));
	setsockopt(sockets[0], SOL_SOCKET, SO_RCVBUF, &bufferSize, sizeof(bufferSize));
	setsockopt(sockets[1], SOL_SOCKET, SO_SNDBUF, &bufferSize, sizeof(bufferSize));
	setsockopt(sockets[1], SOL_SOCKET, SO_RCVBUF, &bufferSize, sizeof(bufferSize));

	father_fd = sockets[0];
	child_fd = sockets[1];

	pid[0] = fork();
	if(pid[0] < 0) {
		printf("fork error!\n");
		return -1;
	} else if(pid[0] == 0) {
		printf("child 1\n");
//		sleep(2);
		write(child_fd, child1_buf, strlen(child1_buf));
	} else if(pid[0] > 0) {
		pid[1] = fork();
		if(pid[1] < 0) {
			printf("fork2 error!\n");
			return -1;
		} else if(pid[1] == 0) {
			printf("child 2\n");
//			sleep(5);
			read(father_fd, buf, sizeof(buf));
			if(strlen(buf) > 7 && !strcmp(buf, "child 1\n"))
				printf("child 2 get child 1 Msg");
			write(child_fd, child2_buf, strlen(child2_buf));
		} else if(pid[1] > 0) {
			printf("parent process\n");
			while(1) {
				read(father_fd, buf, sizeof(buf));
				if(strlen(buf) > 7 && !strcmp(buf, "child 1\n")) {
					printf("parent Get child 1 Msg\n");
					write(child_fd, child1_buf, strlen(child1_buf));
				}
				if(strlen(buf) > 7 && !strcmp(buf, "child 2\n"))
					printf("parent get child 2 Msg\n");
				printf("Parent Get Msg: %s\n", buf);
			}
		}
	}
}
