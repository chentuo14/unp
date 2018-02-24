
/* Method 1 
	if(event[i].events & EPOLLIN) {
		std::cout<<"EPOLLIN"<<std::endl;
		sockfd = events[i].data.fd;
		if((n = read(sockfd, line, MAXLINE)) > 0) {
			line[n] = '\0';
			std::cout<<"read "<<line<<std::endl;
			if(n == MAXLINE) {
				ev.data.fd = sockfd;
				ev.events = EPOLLIN|EPOLLET;
				epoll_ctl(epfd, EPOLL_CTL_MOD, sockfd, &ev);
			} else {
				ev.data.fd = sockfd;
				ev.events = EPOLLIN|EPOLLET;
				epoll_ctl(epfd, EPOLL_CTL_MOD, sockfd, &ev);
			}
		} else if(n == 0) {
			close(sockfd);
		}
	} else if(events[i].events & EPOLLOUT) {
		sockfd = events[i].data.fd;
		write(sockfd, line, n);
		ev.data.fd = sockfd;
		ev.events = EPOLLIN|EPOLLET;
		epoll_ctl(epfd, EPOLL_CTL_MOD, sockfd, &ev);
	}
*/

/* Method 2 
	if(events[i].events & EPOLLIN) {
		n = 0;
	    while((nread = read(fd, buf+n, BUFSIZ-1)) > 0) {
		    n += nread;
	    }

	    if(nread == -1 && errno != EAGAIN) {
	    	perror("read error");
	   	}

	   	ev.data.fd = fd;
	   	ev.events = events[i].events | EPOLLOUT;
	   	epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &ev);
   	}
	if(events[i].events & EPOLLOUT) {
		int nwrite, data_size = strlen(buf);
		n = data_size;
		while(n>0) {
			nwrite = write(fd, buf+data_size-n, n);
			if(nwrite < n) {
				if(nwrite == -1 && errno != EAGAIN) {
					perror("write error");
				}
				break;
			}
			n -= nwrite;
		}
		ev.data.fd = fd;
		ev.events = EPOLLIN|EPOLLET;
		epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &ev);
	}
*/
	
