#include <stdio.h>
#include <signal.h>

int cnt = 0;

void cbSigAlrm(int signo)
{
	printf(" seconds: %d\n", ++cnt);
	alarm(1);
}

void main()
{
	if(signal(SIGALRM, cbSigAlrm) == SIG_ERR) {
		perror("signal");
		return;
	}

	setbuf(stdout, NULL);
	alarm(1);
	while(1) {
		pause();
	}
}
