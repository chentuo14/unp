#include "3process_fun.h"

void send_fd(int sock, int sendfd)
{
	struct msghdr msg;
	struct iovec iov[1];
	char buf[32];
	int cmsgsize = CMSG_LEN(sizeof(int));
	struct cmsghdr *cmptr;

	cmptr = (cmsghdr *)malloc(cmsgsize);
	if(cmptr == NULL) {
		cout<<"[send_fd] init cmptr error"<<endl;
		exit(1);
	}

	cmptr->cmsg_level = SOL_SOCKET;
	cmptr->cmsg_type = SCM_RIGHTS;  /* send file description */
	cmptr->cmsg_len = cmsgsize;
	*((int *)CMSG_DATA(cmptr)) = sendfd;

	iov[0].iov_base = buf;
	iov[0].iov_len = sizeof(buf);

	msg.msg_name = NULL;
	msg.msg_namelen = 0;
	msg.msg_iov = iov;
	msg.msg_iovlen = 1;
	msg.msg_control = cmptr;
	msg.msg_controllen = cmsgsize;

	int ret = sendmsg(sock, &msg, 0);
	free(cmptr);
	if(ret == -1) {
		cout<<"[send_fd] sendmsg error"<<endl;
		exit(1);
	}
}

int recv_fd(int sock)
{
	struct msghdr msg;
	struct iovec iov[1];
	char buf[32];
	int cmsgsize = CMSG_LEN(sizeof(int));
	struct cmsghdr *cmptr;

	iov[0].iov_base = buf;
	iov[0].iov_len = sizeof(buf);

	//  cmptr = CMSG_FIRSTHDR(&msg);
	cmptr = (cmsghdr *)malloc(cmsgsize);
	if(cmptr == NULL) {
		cout<<"[send_fd] init cmptr error"<<endl;
		exit(1);
	}

	msg.msg_name = NULL;
	msg.msg_namelen = 0;
	msg.msg_iov = iov;
	msg.msg_iovlen = 1;
	msg.msg_control = cmptr;
	msg.msg_controllen = cmsgsize;

	int ret = recvmsg(sock, &msg, 0);
	free(cmptr);
	if(ret == -1) {
		cout<<"[recv_fd] recvmsg error"<<endl;
		exit(1);
	}

	int fd = *((int *)CMSG_DATA(cmptr));
	return fd;
}

void child_process(int index, int sockets[2])
{
	cout<<"child"<<index<<" process #"<<getpid()<<endl;	
}
