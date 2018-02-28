#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define UNIX_DOMAIN "/tmp/UNIX.domain"

int main(void)
{
	int connect_fd;
	struct sockaddr_un srv_addr;
	char snd_buf[256];
	char rcv_buf[256];
	int ret;
	int i;
	connect_fd = socket(AF_UNIX, SOCK_STREAM, 0);
	if(connect_fd < 0) {
		perror("client create socket failed");
		return 1;
	}
	srv_addr.sun_family = AF_UNIX;
	strcpy(srv_addr.sun_path, UNIX_DOMAIN);
	ret = connect(connect_fd, (struct sockaddr *)&srv_addr, sizeof(srv_addr));
	if(ret == -1) {
		perror("connect to server failed!");
		close(connect_fd);
		unlink(UNIX_DOMAIN);
		return 1;
	}

	memset(rcv_buf, 0, 256);
	int rcv_num = read(connect_fd, rcv_buf, sizeof(rcv_buf));
	printf("receive message from server (%d) :%s\n", rcv_num, rcv_buf);
	memset(snd_buf, 0, 256);
	strcpy(snd_buf, "message from client");
	printf("sizeof(snd_buf): %d\n", sizeof(snd_buf));

	printf("send data to server ... ... \n");
	for(i=0;i<4;i++) {
		write(connect_fd, snd_buf, sizeof(snd_buf));
	}
	printf("send end!\n");
	close(connect_fd);
	return 0;
}
