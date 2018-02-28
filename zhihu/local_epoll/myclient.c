#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define UNIX_DOMAIN "/tmp/UNIX.domain"

int main(void)
{
	int connect_fd, ret;
	struct sockaddr_un srv_addr;
	char rcv_buf[1024];
	char snd_buf[1024];

	if((connect_fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
		perror("client create socket failed");
		return 1;
	}

	bzero(&srv_addr, sizeof(srv_addr));
	srv_addr.sun_family = AF_UNIX;
	strcpy(srv_addr.sun_path, UNIX_DOMAIN);

	ret = connect(connect_fd, (struct sockaddr *)&srv_addr, sizeof(srv_addr));
	if(ret < 0) {
		perror("connect to server failed!");
		close(connect_fd);
		unlink(UNIX_DOMAIN);
		return 1;
	}

	memset(snd_buf, 0, 1024);
	strcpy(snd_buf, "Hello");
	ret = write(connect_fd, snd_buf, sizeof(snd_buf));
	if(ret < 0) {
		perror("write to server failed!");
		close(connect_fd);
		return 1;
	}

	ret = read(connect_fd, rcv_buf, sizeof(rcv_buf));
	if(ret < 0) {
		perror("read from server failed!");
		close(connect_fd);
		return 1;
	}

	printf("%s %s\n", snd_buf, rcv_buf);
	
	close(connect_fd);
	return 0;
}
