#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>

int main(int argc, char *argv[])
{
	int ret;
	int sock[2];
	struct msghdr msg;
	struct iovec iov[1];
	char send_buf[100] = "Hello";
	
	struct msghdr msgr;
	struct iovec iovr[1];
	char recv_buf[100];

	ret = socketpair(AF_LOCAL, SOCK_STREAM, 0, sock);
	if(ret == -1) {
		printf("socketpair err\n");
		return 1;
	}

	/* sock[1] send data to local */
	bzero(&msg, sizeof(msg));
	msg.msg_name = NULL;
	msg.msg_namelen = 0;
	iov[0].iov_base = send_buf;
	iov[0].iov_len = sizeof(send_buf);
	msg.msg_iov = iov;
	msg.msg_iovlen = 1;

	printf("Starting send data: \n");
	printf("Send data: %s\n", send_buf);
	ret = sendmsg(sock[1], &msg, 0);
	if(ret == -1) {
		printf("sendmsg err\n");
		return -1;
	}
	printf("Send successful\n");

	/* sock[0] recv data to local */
	bzero(&msgr, sizeof(msgr));
	msgr.msg_name = NULL;
	msgr.msg_namelen = 0;
	iovr[0].iov_base = recv_buf;
	iovr[0].iov_len  = sizeof(recv_buf);
	msgr.msg_iov = iovr;
	msgr.msg_iovlen = 1;
	ret = recvmsg(sock[0], &msgr, 0);
	if(ret == -1) {
		printf("recvmsg err\n");
		return -1;
	}
	printf("Recv successful\n");
	printf("Recv data: %s\n", recv_buf);

	close(sock[0]);
	close(sock[1]);
	
	return 0;
}
