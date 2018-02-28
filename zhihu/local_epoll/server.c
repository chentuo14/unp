#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <signal.h>
#include <sys/epoll.h>
#include <errno.h>

#define UNIX_DOMAIN "/tmp/UNIX.domain"

void handler(int num) 
{
	printf("clean program start\n");
	remove(UNIX_DOMAIN);
	printf("clean end.\n");
}

int main(void)
{
	int lsn_fd, apt_fd;
	struct sockaddr_un srv_addr;
	struct sockaddr_un clt_addr;
	socklen_t clt_len;
	int ret;
	int i;
	char recv_buf[1024];
	char send_buf[1024];

	signal(SIGTERM, handler);
	//create epoll
	int epfd, eventfd;
	struct epoll_event ev,events[1024];

	epfd = epoll_create(1024);
	//create socket to bind local IP and PORT
	lsn_fd = socket(AF_UNIX, SOCK_STREAM, 0);
	ev.data.fd = lsn_fd;
	ev.events = EPOLLIN|EPOLLET;

	epoll_ctl(epfd, EPOLL_CTL_ADD, lsn_fd, &ev);
	if(lsn_fd < 0) {
		perror("can't create communication socket!");
		return 1;
	}

	//create local IP and PORT
	srv_addr.sun_family = AF_UNIX;
	strncpy(srv_addr.sun_path, UNIX_DOMAIN, sizeof(srv_addr.sun_path) - 1);

	//bind sockfd and sockaddr
	ret = bind(lsn_fd, (struct sockaddr *)&srv_addr, sizeof(srv_addr));
	if(ret == -1) {
		perror("can't bind local sockaddr!");
		close(lsn_fd);
		unlink(UNIX_DOMAIN);
		return 1;
	}

	ret = listen(lsn_fd, 5);
	if(ret == -1) {
		perror("can't listen client connect request");
		close(lsn_fd);
		unlink(UNIX_DOMAIN);
		return 1;
	}

	clt_len = sizeof(clt_addr);
	while(1) {
		int nfds = epoll_wait(epfd, events, 1024, 100);
		int i = 0;
		for(i=0;i<nfds;i++) {
			if(events[i].data.fd == lsn_fd) {
				apt_fd = accept(lsn_fd, (struct sockaddr *)&clt_addr, &clt_len);
				if(apt_fd < 0) {
					perror("can't listen client connect request");
					close(lsn_fd);
					unlink(UNIX_DOMAIN);
					return 1;
				}
				char lines[256];
				sprintf(lines, "server data to client\n");
				write(apt_fd, lines, 256);

				ev.data.fd = apt_fd;
				ev.events = EPOLLIN|EPOLLET;
				epoll_ctl(epfd, EPOLL_CTL_ADD, apt_fd, &ev);
			} else if(events[i].events & EPOLLIN) {
				printf("EPOLLIN\n");
				if((eventfd = events[i].data.fd) < 0)
					continue;

				int n = 0, ret = 0;
				char line[256];
				if((ret = read(eventfd, line, 256)) < 0) {
					if(errno == ECONNRESET) {
						close(eventfd);
						events[i].data.fd = -1;
					} else
						printf("readline error\n");
				} else if(ret == 0) {
					close(eventfd);
					events[i].data.fd = -1;
				} else if(ret > 0) {
					line[ret] = '\0';
					printf("%s", line);
					while((ret = read(eventfd, line, 256)) < 0) {
						line[ret] = '\0';
						printf("%s", line);
					}
					printf("\n");
				}
			} else if(events[i].events & EPOLLOUT) {
				int eventfd = events[i].data.fd;
				char line[256];
				write(eventfd, line, 256);

				ev.data.fd = eventfd;
				ev.events = EPOLLIN | EPOLLET;

				epoll_ctl(epfd, EPOLL_CTL_ADD, eventfd, &ev);
			}
		}
	}
	close(apt_fd);
	close(lsn_fd);
	unlink(UNIX_DOMAIN);
	return 0;
}
