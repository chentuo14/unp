#include "common.h"

int main()
{
	char buff[MSG_SIZE];
	int msg_id = GetMessageQueue();
	while(1) {
		//receive data
		ReceiveDataFromMessageQueue(msg_id, SERVER_TYPE, buff);
		printf("from server:%s\n", buff);

		//send data
		printf("client please enter# ");
		fflush(stdout);
		ssize_t s = read(0, buff, sizeof(buff)-1);
		if(s <= 0) {
			perror("read error");
			return 1;
		} else {
			buff[s-1] = 0;
			SendDataToMessageQueue(msg_id, CLIENT_TYPE, buff);
			printf("data has sended,wait receive......\n");
		}
	}
	return 0;
}
