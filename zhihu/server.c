#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <strings.h>
#include <string.h>

#define oops(msg, n) do{ perror(msg); exit(n);} while(0)

int main(int argc, char *argv[])
{
	int sock_id, sock_fd;
	struct sockaddr_in saddr;
	FILE *sock_fpi, *sock_fpo;
	char buf[BUFSIZ];
	const char *str_ret = "world";

	if((sock_id = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		oops("socket", 1);	

	bzero(&saddr, sizeof(saddr));
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(43210);
	saddr.sin_addr.s_addr = htonl(INADDR_ANY);
	if(bind(sock_id, (struct sockaddr *)&saddr, sizeof(saddr)) == -1)
		oops("bind", 2);

	if(listen(sock_id, 1) == -1)
		oops("listen", 3);

	while(1)
	{
		sock_fd = accept(sock_id, NULL, NULL);	
		if(sock_fd == -1)
			oops("accept", 3);
		printf("Wow!Got a call!\n");

		sock_fpi = fdopen(sock_fd, "r");
		if(sock_fpi == NULL)
			oops("fdopen in", 4);
		sock_fpo = fdopen(sock_fd, "w");
		if(sock_fpo == NULL)
			oops("fdopen out", 5);

		if(fgets(buf, BUFSIZ-5, sock_fpi) == NULL)
			oops("reading", 5);
		printf("%s", buf);

//		if(fputs("World", sock_fpo) == EOF)
//			oops("writing", 6);
		if(write(sock_fd, str_ret, strlen(str_ret)) < 0)
			oops("writing", 6);
		
		fclose(sock_fpi);
		fclose(sock_fpo);
		close(sock_fd);
	}

	return 0;
}
