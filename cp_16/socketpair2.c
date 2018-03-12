#include "socketpair2.h"

int write_channel_fd(int fd, message_t *data)
{
	struct msghdr msg_wr;
	struct iovec iov[1];
	struct cmsghdr *ptr = NULL;

	union {			//使用这个结构体与真正要发的数据放在一起时为了使辅助数据的地址对齐
		struct cmsghdr cm;
		char ctl[CMSG_SPACE(sizeof(int))];
	}ctl_un;

	switch(data->info.type) {
	case FD_TRANS:
		msg_wr.msg_control = ctl_un.ctl;
		msg_wr.msg_controllen = sizeof(ctl_un.ctl);

		ptr = CMSG_FIRSTHDR(&msg_wr);
		ptr->cmsg_len = CMSG_LEN(sizeof(int));
		ptr->cmsg_level = SOL_SOCKET;
		ptr->cmsg_type = SCM_RIGHTS;
		*((int *)CMSG_DATA(ptr)) = data->data.fd;

		iov[0].iov_base = (void *)(&(data->info));
		iov[0].iov_len = sizeof(info_t);
		break;
	case MSG_TRANS:
		msg_wr.msg_control = NULL;
		msg_wr.msg_controllen = 0;

		iov[0].iov_base = data;
		iov[0].iov_len = sizeof(message_t);
		break;
	}
	
	msg_wr.msg_name = NULL;
	msg_wr.msg_namelen = 0;

	msg_wr.msg_iov = iov;
	msg_wr.msg_iovlen = 1;

	return(sendmsg(fd, &msg_wr, 0));
}

int recv_channel(int fd, message_t *data)
{
	struct msghdr msg_rc;
	struct iovec iov[1];
	ssize_t n;

	union {
		struct cmsghdr cm;
		char ctl[CMSG_SPACE(sizeof(int))];
	}ctl_un;

	struct cmsghdr *ptr = NULL;
	msg_rc.msg_control = ctl_un.ctl;
	msg_rc.msg_controllen = sizeof(ctl_un.ctl);

	msg_rc.msg_name = NULL;
	msg_rc.msg_namelen = 0;

	iov[0].iov_base = (void *)data;
	iov[0].iov_len = sizeof(message_t);

	msg_rc.msg_iov = iov;
	msg_rc.msg_iovlen = 1;

	if((n = recvmsg(fd, &msg_rc, 0)) < 0) {
		perror("recvmsg error");
		return n;
	} else if(n == 0) {
		fprintf(stderr, "peer close the socket\n");
		exit(1);
	}

	if((ptr = CMSG_FIRSTHDR(&msg_rc)) != NULL
			&& ptr->cmsg_len == CMSG_LEN(sizeof(int))) {
		if(ptr->cmsg_level != SOL_SOCKET) {
			fprintf(stderr, "Ctl level should be SOL_SOCKET\n");
			exit(EXIT_FAILURE);
		}

		if(ptr->cmsg_type != SCM_RIGHTS) {
			fprintf(stderr, "Ctl type should be SCM_RIGHTS\n");
			exit(EXIT_FAILURE);
		}

		data->data.fd = *(int *)CMSG_DATA(ptr);
	} else {
		data->data.fd = -1;
	}

	return n;
}

void add_fd_to_epoll_in(int epollfd, int fd)
{

}
