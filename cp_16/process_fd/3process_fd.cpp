#include "3process_fd.h"

void setnonblocking(int sock)
{
	int opts;
	opts = fcntl(sock, F_GETFL);
	if(opts < 0) {
		cout<<"fcntl error"<<endl;
		exit(1);
	}

	opts = opts | O_NONBLOCK;
	if(fcntl(sock, F_SETFL, opts) < 0) {
		cout<<"set nonblock error"<<endl;
		exit(1);
	}
}

void child_process(int index, process_t *processes)
{
	cout<<"child "<<index<<" pid: "<<getpid()<<endl;
	struct epoll_event ev, events[20];
	int epfd, nfds, sockfd, listenfd, connfd;
	int i, n;
	char buf[MAXLINE], line[MAXLINE];
	struct sockaddr_in servaddr;
	struct sockaddr_in clientaddr;
	socklen_t clilen = sizeof(struct sockaddr_in);

	/* socket bind and listen */
	listenfd = socket(AF_INET, SOCK_STREAM, 0);	
	setnonblocking(listenfd);
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERV_PORT);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
	listen(listenfd, 20);

	/* create epoll	*/
	epfd = epoll_create(256);
	ev.data.fd = (processes+index)->chanel[1];
	ev.events = EPOLLIN | EPOLLET;
	epoll_ctl(epfd, EPOLL_CTL_ADD, (processes+index)->chanel[1], &ev);

	ev.data.fd = listenfd;
	ev.events = EPOLLIN | EPOLLET;
	epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &ev);

	for( ; ;) {
		nfds = epoll_wait(epfd, events, 100, 500);
		for(i=0;i<nfds;i++) {
			if(events[i].data.fd == listenfd) {
				connfd = accept(listenfd, (sockaddr *)&clientaddr, &clilen);
				if(connfd < 0) {
					cout<<"[child"<<index<<"] accept error"<<endl;
					exit(1);
				}
				setnonblocking(connfd);
				char *str = inet_ntoa(clientaddr.sin_addr);
				cout<<"[child"<<index<<"] connect from "<<str<<endl;
				ev.data.fd = connfd;
				ev.events = EPOLLIN | EPOLLET;
				epoll_ctl(epfd, EPOLL_CTL_ADD, connfd, &ev);
			} else if(events[i].data.fd == (processes+index)->chanel[1]) {
				read((processes+index)->chanel[1], buf, sizeof(buf));
				cout<<"child "<<index<<" recv: "<<buf<<endl;
			} else if(events[i].events & EPOLLIN) {
				if((sockfd = events[i].data.fd) < 0) {
					ev.data.fd = sockfd;
					ev.events = EPOLLIN | EPOLLET;
					epoll_ctl(epfd, EPOLL_CTL_DEL, sockfd, &ev);
					continue;
				}
				if((n = read(sockfd, line, MAXLINE)) < 0) {
					if(errno == ECONNRESET) {
						close(sockfd);
						events[i].data.fd = -1;
						ev.data.fd = sockfd;
						ev.events = EPOLLIN | EPOLLET;
						epoll_ctl(epfd, EPOLL_CTL_DEL, sockfd, &ev);
					} else {
						cout<<"readline error"<<endl;
					}
				} else if(n == 0) {
					close(sockfd);
					events[i].data.fd = -1;
					ev.data.fd = sockfd;
					ev.events = EPOLLIN | EPOLLET;
					epoll_ctl(epfd, EPOLL_CTL_DEL, sockfd, &ev);
					cout<<"[child"<<index<<"] close socket?"<<endl;
					continue;
				}
				cout<<"[child"<<index<<"] here is a EPOLLIN event."<<endl;
				ev.data.fd = sockfd;
				ev.events = EPOLLOUT | EPOLLET;
				epoll_ctl(epfd, EPOLL_CTL_MOD, sockfd, &ev);
			} else if(events[i].events & EPOLLOUT) {
				if((sockfd = events[i].data.fd) < 0) {
					ev.data.fd = sockfd;
					ev.events = EPOLLIN | EPOLLET;
					epoll_ctl(epfd, EPOLL_CTL_DEL, sockfd, &ev);
					continue;
				}
				if(!strcmp(line, "Hello")) {
					strcpy(buf, "World");
					write(sockfd, buf, strlen(buf));
				}
				cout<<"[child"<<index<<"] here is a EPOLLOUT event."<<endl;
				ev.data.fd = sockfd;
				ev.events = EPOLLIN | EPOLLET;
				epoll_ctl(epfd, EPOLL_CTL_MOD, sockfd, &ev);
			}
		}
	}

	exit(0);
}

int main(int argc, char *argv[])
{
	cout<<"current pid: "<<getpid()<<endl;
	process_t *pprocess = (process_t *)malloc(sizeof(process_t)*MAXPROCESS);
	int i;
	int pid[MAXPROCESS];

	for(i=0;i<MAXPROCESS;i++) {
		(pprocess+i)->index = i;
		if(socketpair(AF_UNIX, SOCK_STREAM, 0, (pprocess+i)->chanel) == -1) {
			cout<<"failed to create domain socket by socketpair"<<endl;
			exit(1);
		}
	}

	for(i=0;i<MAXPROCESS;i++) {
		pid[i] = fork();
		switch(pid[i]) {
			case -1:
				cout<<"fork error"<<endl;
				exit(-1);
			case 0:
				child_process(i, pprocess);
				exit(1);
			defualt:
				cout<<"default?"<<endl;
				break;
		}
	}

	for(i=0;i<MAXPROCESS;i++) {
		write((pprocess+i)->chanel[0], "Hello child", strlen("Hello child"));
	}

	for(;;) {
		pause();
	}
}
