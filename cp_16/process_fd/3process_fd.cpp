#include "3process_fd.h"

void child_process(int index, process_t *processes)
{
	cout<<"child "<<index<<" pid: "<<getpid()<<endl;
	struct epoll_event ev, events[20];
	int epfd, nfds;
	int i;
	char buf[MAXLINE];
	
	epfd = epoll_create(10);
	ev.data.fd = (processes+index)->chanel[1];
	ev.events = EPOLLIN | EPOLLET;
	epoll_ctl(epfd, EPOLL_CTL_ADD, (processes+index)->chanel[1], &ev);

	for( ; ;) {
		nfds = epoll_wait(epfd, events, 20, 500);
		for(i=0;i<nfds;i++) {
			if(events[i].data.fd == (processes+index)->chanel[1]) {
				read((processes+index)->chanel[1], buf, sizeof(buf));
				cout<<"child "<<index<<" recv: "<<buf<<endl;
			} else if(events[i].events & EPOLLIN) {

			} else if(events[i].events & EPOLLOUT) {

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
