#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/socket.h>
#include <stdlib.h>

int main() {
	int s[2];
	int w,r;
	pid_t pid;
	char arr[6]={0};
	char brr[9]={0};

	if(socketpair(AF_UNIX, SOCK_STREAM, 0, s) == -1) {
		printf("create unnamed socket pair failed:%s\n", strerror(errno));
		exit(-1);
	}
	pid = fork();
	if(pid > 0) {
		pid_t pid1;
		pid1 = fork();
		if(pid1 > 0) {
			w = write(s[1], "abcde", 5);
			sleep(1);
			w = write(s[1], "12345", 5);
			wait(NULL);
			exit(0);
		} else {
			read(s[0], arr, 5);
			printf("child2:%s\n", arr);
			exit(0);
		}
	} else if(pid == 0) {
		read(s[0], arr, 5);
		printf("child1:%s\n",arr);
		exit(0);
	}
	return 0;
}
