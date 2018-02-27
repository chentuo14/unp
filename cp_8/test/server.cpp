#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

using namespace std;

int main()
{
	int sockListener, nMsgLen;
	char szBuf[1024];
	struct sockaddr_in addrListener;
	socklen_t addrLen;
	addrLen = sizeof(struct sockaddr_in);
	bzero(&addrListener, sizeof(addrListener));
	addrListener.sin_family = AF_INET;
	addrListener.sin_port = htons(8000);

	if((sockListener = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
		perror("error in getting a socket");
		exit(1);
	}

	if(bind(sockListener, (struct sockaddr*)&addrListener,
				sizeof(addrListener)) == -1) {
		perror("bind a listener for a socket");
		exit(2);
	}

	struct sockaddr_in addrClient;
	std::cout<<"callback server begin to listen"<<std::endl;
	while(true) {
		nMsgLen = recvfrom(sockListener, szBuf, 1024, 0,
				(struct sockaddr *)&addrClient, &addrLen);
		if(nMsgLen > 0) {
			szBuf[nMsgLen] = '\0';
			std::cout<<"send back:"<<szBuf<<std::endl;
			sendto(sockListener, szBuf, nMsgLen, 0,
					(struct sockaddr *)&addrClient, addrLen);
		}
	}
}
