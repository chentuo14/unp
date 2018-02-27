#include "unp.h"

#define DNG 2000
#define DGLEN 1400

void
dg_cli(FILE *fp, int sockfd, const SA *pserveaddr, socklen_t servlen)
{
	int i;
	char sendline[MAXLINE];
	for(i=0;i<NDG;i++) {
		Sendto(sockfd, sendline, DGLEN, 0, pservaddr, servlen);
	}
}
