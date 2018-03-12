#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

using namespace std;

#include <sys/socket.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/uio.h>

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
	cmptr->cmsg_type = SCM_RIGHTS;	/* send file description */
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
	
//	cmptr = CMSG_FIRSTHDR(&msg);
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

void worker_process_cycle(int sockets[2])
{
	cout<<"worker process #"<<getpid()<<endl;
	int fd = sockets[1];
	int file = recv_fd(fd);
	if(file < 0) {
		cout<<"[worker] invalid fd!"<<endl;
		exit(1);
	}
	char msg[] = "child process";
	cout<<"[worker] write file #"<<file<<" ret = "
		<<write(file, msg, sizeof(msg))<<endl;
	
	close(file);
	exit(0);
}

void master_process_cycle(int sockets[2])
{
	cout<<"master process #"<<getpid()<<endl;
	int fd = sockets[0];

	system("rm -f ./newfile");
	int file = open("./newfile", O_CREAT|O_TRUNC|O_RDWR);
	cout<<"[master] dispath fd to worker process, file=#"<<file<<endl;
	send_fd(fd, file);

	int status;
	waitpid(-1, &status, 0);
	exit(0);
}

int main(int argc, char *argv[])
{
	cout<<"current pid: "<<getpid()<<endl;
	int sockets[2];
	if(socketpair(AF_UNIX, SOCK_STREAM, 0, sockets) == -1) {
		cout<<"failed to create domain socket by socketpair"<<endl;
		exit(1);
	}
	cout<<"create domain socket by sockpair success"<<endl;

	cout<<"create process to communicate over domain socket"<<endl;
	pid_t pid = fork();
	if(pid == 0) {
		worker_process_cycle(sockets);
	} else {
		master_process_cycle(sockets);
	}

	for( ; ; ) {
		pause();
	}
}
