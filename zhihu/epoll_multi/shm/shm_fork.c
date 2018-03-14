#include <stdio.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <string.h>
#include <unistd.h>

#define SIZE 1024

int main()
{
	int shmid;
	pid_t pid;
	char *shmaddr;
	int flag;
	struct shmid_ds buf;

	shmid = shmget(IPC_PRIVATE, SIZE, IPC_CREAT|0600);
	if(shmid < 0) {
		perror("get shm ipc_id error");
		return -1;
	}

	if((pid = fork()) < 0) {
		perror("get fork error");
		return -1;
	} else if(pid == 0) {
		shmaddr = (char *)shmat(shmid, NULL, 0);
		if((int)shmaddr == -1) {
			perror("shmat error");
			return -1;
		}
		strcpy(shmaddr, "Hi,I am child process!\n");
		shmdt(shmaddr);
		return 0;
	} else {
		sleep(3);
		flag = shmctl(shmid, IPC_STAT, &buf);
		if(flag == -1) {
			perror("shmctl error");
			return -1;
		}

		printf("shm_segsz=%d bytes\n", buf.shm_segsz);
		printf("parent pid=%d,shm_cpid=%d\n", getpid(), buf.shm_cpid);
		printf("child pid=%d,shm_lpid=%d\n", pid, buf.shm_lpid);
		shmaddr = (char *)shmat(shmid, NULL, 0);
		if((int)shmaddr == -1) {
			perror("parent:shmat error");
			return -1;
		}
		printf("shmaddr in %s \n", shmaddr);
		shmdt(shmaddr);
		shmctl(shmid, IPC_RMID, NULL);
	}

	return 0;
}
