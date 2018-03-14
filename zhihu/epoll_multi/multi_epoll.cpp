#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>

using namespace std;

#define MAX_EVENTS 1000

void Process(int listenfd);

struct shmstruct {
	int count;
};

int main(int argc, char **argv)
{
	short port = 8000;
	if(argc == 2) {
		port = atoi(argv[1]);
	}

	shm_unlink("test");
	int fd1 = shm_open("test", O_RDWR|O_CREAT|O_EXCL, 666);
	struct shmstruct *ptr;
	ftruncate(fd1, sizeof(struct shmstruct));
	ptr = (struct shmstruct*)mmap(NULL, sizeof(struct shmstruct),
			PROT_READ|PROT_WRITE, MAP_SHARED, fd1, 0);
	close(fd1);

	int listenfd = socket(AF_INET, SOCK_STREAM, 0);
	fcntl(listenfd, F_SETFL, O_NONBLOCK);
	struct sockaddr_in sin;
	bzero(&sin, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(port);
	sin.sin_addr.s_addr = INADDR_ANY;

	bind(listenfd, (struct sockaddr *)&sin, sizeof(sin));
	listen(listenfd, 5);

	for(int i=0;i<5;i++) {
		pid_t pid = fork();
		if(pid == 0) {
			Process(listenfd);
			return 0;
		}
	}

	while(true) {
		sleep(10);
	}

	return 0;
}

void Process(int listenfd)
{
	int fd1 = shm_open("test", O_RDWR, 666);
	struct shmstruct* ptr;
	ptr = (struct shmstruct *)mmap(NULL, sizeof(struct shmstruct),
			PROT_READ|PROT_WRITE, MAP_SHARED, fd1, 0);
	close(fd1);
	struct epoll_event ev, events[MAX_EVENTS];

	int epfd = epoll_create(MAX_EVENTS);
	ev.data.fd = listenfd;
	ev.events = EPOLLIN|EPOLLET;

	if(epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &ev) < 0) {
		printf("worker poll_ctl error = %s.", strerror(errno));
		exit(1);
	}

	while(true) {
		int nfds = epoll_wait(epfd, events, MAX_EVENTS, -1);
		for(int i=0;i<nfds;i++) {
			if(events[i].data.fd == listenfd) {
				socklen_t clilen;
				struct sockaddr_in clientaddr;

				int sockfd = accept(listenfd, (struct sockaddr *)&clientaddr, &clilen);
				if(sockfd < 0) {
					continue;
				}

				ev.data.fd = sockfd;
				ev.events = EPOLLIN|EPOLLET;
				epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &ev);
			} else if(events[i].events & EPOLLIN) {
				int sockfd = events[i].data.fd;
				if(sockfd < 0) {
					continue;
				}

				char buf[1024] = {0};

				bzero(buf, sizeof(buf));
				int len = read(sockfd, buf, 1023);
				if(len < 0) {
					if(errno == ECONNRESET) {
						close(sockfd);
						events[i].data.fd = -1;
					} else {
						printf("worker read data error = %s.", strerror(errno));
					}
				} else if(len == 0) {
					events[i].data.fd = -1;
				} else {
					if(send(sockfd, "asdf", 4, 0) < 0) {
						printf("send msg error: %s(errno: %d)\n",
								strerror(errno), errno);
						exit(0);
					}
					printf("count:%d\n", ++ptr->count);
				}
			}
		}
	}
}
