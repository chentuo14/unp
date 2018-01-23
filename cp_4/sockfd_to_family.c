#include "unp.h"

int
sockfd_to_family(int sockfd)
{
	union {
		struct sockaddr sa;
		char data[MAXSOCKADDR];
	}un;
	socklen_t len;
	len = MAXSOCKADDR;
	if(getsockname(sockfd, (SA *)un.data, &len) < 0)
		return(-1);
	return(un.sa.sa_family);
}
