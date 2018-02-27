#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>

using namespace std;

int main()
{
	int sockClient, nMsgLen, nReady;
	char szRecv[1024], szSend[1024], szMsg[1024];
	struct sockaddr_in addrServer, addrClient, addrLocal;
	socklen_t addrLen;
	fd_set setHold, setTest;

	sockClient = socket(AF_INET, SOCK_DGRAM, 0);
	addrLen = sizeof(struct sockaddr_in);
	bzero(&addrServer, sizeof(addrServer));
	addrServer.sin_family = AF_INET;
	addrServer.sin_addr.s_addr = inet_addr("127.0.0.1");
	addrServer.sin_port = htons(8000);

	bzero(&addrLocal, sizeof(addrLocal));
	addrLocal.sin_family = AF_INET;
	addrLocal.sin_addr.s_addr = htonl(INADDR_ANY);
	addrLocal.sin_port = htons(9000);
	
	if(bind(sockClient, (struct sockaddr *)&addrLocal,
				sizeof(addrLocal)) == -1) {
		perror("error in binding");
		exit(2);
	}

	int f = 0;
	if(connect(sockClient, (struct sockaddr *)&addrServer,
				sizeof(addrServer)) == -1) {
		perror("error in connecting");
		exit(1);
	}

	f = 1;

	FD_ZERO(&setHold);
	FD_SET(STDIN_FILENO, &setHold);
	FD_SET(sockClient, &setHold);
	std::cout<<"you can type in sentences any time"<<std::endl;
	while(true) {
		setTest = setHold;
		nReady = select(sockClient+1, &setTest, NULL, NULL, NULL);
		if(FD_ISSET(0, &setTest)) {
			nMsgLen = read(0, szMsg, 1024);
			write(sockClient, szMsg, nMsgLen);
		}

		if(FD_ISSET(sockClient, &setTest)) {
			if(f == 1) {
				nMsgLen = recvfrom(sockClient, szRecv, 1024,
						0, NULL, NULL);
				perror("error in connecting recvfrom");
				addrServer.sin_family = AF_UNSPEC;
				connect(sockClient, (struct sockaddr*)&addrServer,
						sizeof(addrServer));
				f = 0;
			} else {
				f = 1;
				nMsgLen = recvfrom(sockClient, szRecv, 1024, 0,
						(struct sockaddr*)&addrServer, &addrLen);
				bzero(&addrServer, sizeof(addrServer));
				addrServer.sin_family = AF_INET;
				addrServer.sin_addr.s_addr = inet_addr("127.0.0.1");
				addrServer.sin_port = htons(8000);
				connect(sockClient, (struct sockaddr*)&addrServer,
						sizeof(addrServer));
			
			}
			szRecv[nMsgLen] = '\0';
			std::cout<<"read:"<<szRecv<<std::endl;
		}
	}
}
