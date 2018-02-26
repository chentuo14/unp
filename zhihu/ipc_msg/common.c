#include "common.h"

static int CommonMessageQueue(int flags)
{
	key_t _key = ftok(FILEPATH, ID);
	if(_key == -1) {
		perror("ftok error");
		return 1;
	}
	int _msg_id = msgget(_key, flags);
	if(_msg_id < 0) {
		perror("msgget error");
		return 2;
	}
	return _msg_id;
}

int CreateMessageQueue()
{
	return CommonMessageQueue(IPC_CREAT|IPC_EXCL|0666);
}

int GetMessageQueue()
{
	return CommonMessageQueue(IPC_CREAT);
}

int DeleteMessageQueue(int msg_id)
{
	if(msgctl(msg_id, IPC_RMID, NULL) < 0)
		return -1;
	return 0;
}

int SendDataToMessageQueue(int msg_id, int send_type, char *msg)
{
	msginfo buff;
	buff.mtype = send_type;
	strcpy(buff.mtext, msg);
	int msg_snd = msgsnd(msg_id, (void *)&buff, sizeof(buff), 0);
	if(msg_snd < 0) {
		perror("msgsnd error");
		return -3;
	}
	return 0;
}

int ReceiveDataFromMessageQueue(int msg_id, int receive_type, char *out)
{
	msginfo buff;
	int msg_rcv = msgrcv(msg_id, (void *)&buff, sizeof(buff), receive_type, 0);
	if(msg_rcv < 0) {
		perror("msg_rcv error");
		return -4;
	}
	strcpy(out, buff.mtext);
	return 0;
}
