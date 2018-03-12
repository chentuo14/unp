#include "fork.h"

void child_run(int index, process_t *processes)
{
	char buf[256];
//	printf("Now Child%d is actived\n", index);
//	read(processes[index].chanel[1], buf, sizeof(buf));

//	printf("As Child%d, read buf:%s\n", index, buf);

}

int main()
{
	process_t *processes;
	pid_t pid;
	int i,j;
	int n;

	processes = (process_t *)malloc(sizeof(process_t) * PROCESS_MAX);
	for(i=0;i<PROCESS_MAX;i++)
		processes[i].index = -1;

	for(i=0;i<PROCESS_MAX;i++) {
		if(socketpair(AF_UNIX, SOCK_STREAM, 0, processes[i].chanel) < 0) {
			perror("socketpair error");
			exit(-1);
		}
		
		pid = fork();
		switch(pid) {
		case -1:
			perror("fork error");
			exit(-1);
		case 0:	//child
			child_run(i, processes);
			exit(1);
		default:
			break;
		}

		processes[i].pid = pid;
		processes[i].index = i;

		for(j=0;j<i;j++) {
			printf("As father, I just want to send child %d's fd to child %d\n", i, j);
			write_chanel_fd(processes[i].chanel[0], 
			printf("As father, I have just send child %d's fd to child %d, %d bytes\n", i, j, n);
		}
	}
}

