#include <string.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

using namespace std;

int main()
{
	socklen_t addrLen = sizeof(struct sockaddr_in);
	struct sockaddr_in addrServer;
	char szMsg[1024];
	int sockClient;

	addrServer.sin_family = AF_INET;
	addrServer.sin_addr.s_addr = inet_addr("127.0.0.1");
	addrServer.sin_port = htons(9000);

	sockClient = socket(AF_INET, SOCK_DGRAM, 0);
	while(true) {
		static int id = 0;
		snprintf(szMsg, sizeof(szMsg), "this is %d", id++);
		sendto(sockClient, szMsg, strlen(szMsg), 0,
				(struct sockaddr *)&addrServer, sizeof(addrServer));
		sleep(1);
	}
}
