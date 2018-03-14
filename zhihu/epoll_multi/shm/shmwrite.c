#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>

typedef struct {
	char name[8];
	int age;
}people;

int main()
{
	int shmid;
	key_t key;
	char temp[8];
	char pathname[30];
	people *p_map;
	int i;

	strcpy(pathname, "/tmp");
	key = ftok(pathname, 0x03);
	if(key == -1) {
		perror("fork");
		return -1;
	}

	printf("key = %d\n", key);
	shmid = shmget(key, 4096, IPC_CREAT|IPC_EXCL|0600);
	if(shmid == -1) {
		perror("shmget error\n");
		return -1;
	}
	printf("shmid=%d\n", shmid);

	p_map = (people *)shmat(shmid, NULL, 0);
	if((int)p_map == -1) {
		perror("shmat");
		return -1;
	}
	memset(temp, 0x00, sizeof(temp));
	strcpy(temp, "test");
	temp[4] = '0';

	for(i=0;i<3;i++) {
		temp[4]+=1;
		strncpy((p_map+i)->name, temp, 5);
		(p_map+i)->age = i;
	}

	if(shmdt(p_map) == -1) {
		perror("shmdt");
		return -1;
	}

	return 0;
}
