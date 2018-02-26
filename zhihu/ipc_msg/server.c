#include "common.h"

int main()
{
	char buff[MSG_SIZE];
	int msg_id = CreateMessageQueue();

	while(1)
	{
		//send data
		printf("server please enter# ");
		fflush(stdout);
		ssize_t s = read(0, buff, sizeof(buff)-1);
		if(s > 0) {
			buff[s-1] = 0;
			SendDataToMessageQueue(msg_id, SERVER_TYPE, buff);
			printf("data has sended,wait receive......\n");
		} else {
			perror("read error");
			return 1;
		}

		//receive data
		ReceiveDataFromMessageQueue(msg_id, CLIENT_TYPE, buff);
		printf("from client: %s\n", buff);
	}
	DeleteMessageQueue(msg_id);

	return 0;
}
