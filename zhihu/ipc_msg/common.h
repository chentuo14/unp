#ifndef __COMMON_H_
#define __COMMON_H_

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <string.h>
#include <time.h>

#define MSG_SIZE 1024
#define FILEPATH "."
#define ID       0
#define SERVER_TYPE 1
#define CLIENT_TYPE 2

typedef struct msg_info {
	long mtype;
	char mtext[MSG_SIZE];
}msginfo;

int CreateMessageQueue();
int GetMessageQueue();
int DeleteMessageQueue(int msgid);
int SendDataToMessageQueue(int msg_id, int send_type, char *msg);
int ReceiveDataFromMessageQueue(int msg_id, int receive_type, char *out);

#endif
