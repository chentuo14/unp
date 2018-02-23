#include <unistd.h>
#include <iostream>
#include <sys/epoll.h>

using namespace std;

int main(void)
{
	int epfd, nfds;
	char buf[256];
	struct epoll_event ev, events[5];
	epfd = epoll_create(1);
	ev.data.fd = STDIN_FILENO;
	ev.events = EPOLLIN;
	epoll_ctl(epfd, EPOLL_CTL_ADD, STDIN_FILENO, &ev);
	for( ; ; ) {
		nfds = epoll_wait(epfd, events, 5, -1);
		for(int i=0;i<nfds;i++) {
			if(events[i].data.fd == STDIN_FILENO) {
				read(STDIN_FILENO, buf, sizeof(buf));
				cout<<"hello world!"<<endl;
			}
		}
	}
}
