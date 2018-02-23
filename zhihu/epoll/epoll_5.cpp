#include <unistd.h>
#include <iostream>
#include <sys/epoll.h>

using namespace std;

int main(void)
{
	int epfd, nfds;
	struct epoll_event ev, events[5];
	epfd = epoll_create(1);
	ev.data.fd = STDOUT_FILENO;
	ev.events = EPOLLOUT | EPOLLET;
	epoll_ctl(epfd, EPOLL_CTL_ADD, STDOUT_FILENO, &ev);
	for( ; ; ) {
		nfds = epoll_wait(epfd, events, 5, -1);
		for(int i=0;i<nfds;i++) {
			if(events[i].data.fd == STDOUT_FILENO)
				std::cout<<"hello world!";
		}
	}
}
