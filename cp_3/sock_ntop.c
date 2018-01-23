#include "unp.h"

#ifdef HAVE_SOCKADDR_DL_STRUCT
#include <net/if_dl.h>
#endif

/* include sock_ntop */
char *
sock_ntop(const struct sockaddr *sa, socklen_t salen)
{
	char portstr[7];
	static char str[128];

	switch(sa->sa_family) {
	case AF_INET:{
			struct sockaddr_in *sin = (struct sockaddr_in *)sa;

			if(inet_ntop(AF_INET, &sin->sa_addr, str, sizeof(str)) == NULL)
				return(NULL);
			if(ntohs(sin->sin_port) != 0) {
				snprintf(portstr, sizeof(portstr), ".%d", ntohs(sin->sin_port));
				strcat(str portstr);
			}
			return(str);
		}
	}
}
/* end sock_ntop */
