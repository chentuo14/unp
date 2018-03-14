#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <signal.h>

#define MAXLINE 80
#define SERV_PORT 9876

void heart_timeout(int signo)
{
	printf("heart timeout\n");
	return;
}

int main()
{
    char buf[MAXLINE];
    int listenfd = 0;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in servaddr = {0};
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
    listen(listenfd, 20);

	signal(SIGALRM, heart_timeout);
    printf("Accepting connections ...\n");
    while(1) {
        struct sockaddr_in cliaddr = {0};
        socklen_t cliaddr_len = sizeof(cliaddr);
        int connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &cliaddr_len);

        char str[INET_ADDRSTRLEN];
        printf("connect from %s at PORT %d\n",
               inet_ntop(AF_INET, &cliaddr.sin_addr, str, sizeof(str)),
               ntohs(cliaddr.sin_port));
        while(1) {
			alarm(5);
            int count = read(connfd, buf, MAXLINE);
            if(count == 0)
                break;

            if(!strcmp(buf, "Hello")) {
                printf("client send %s\n", buf);
				alarm(0);
                write(connfd, "World", 6);
            }
        }
        close(connfd);
        printf("closed from %s at PORT %d\n",
                inet_ntop(AF_INET, &cliaddr.sin_addr, str, sizeof(str)),
                ntohs(cliaddr.sin_port));
    }
    return 0;
}
