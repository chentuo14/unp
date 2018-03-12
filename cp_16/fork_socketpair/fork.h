#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>

#ifndef PROCESS_MAX
#define PROCESS_MAX 3
#endif

enum message_type{FD_TRANS = 0, MSG_TRANS};

typedef struct {
	pid_t pid;
	int index;
	int chanel[2];
}process_t;

typedef struct {
	enum message_type type;
	int sourceIndex;
}info_t;

typedef struct {
	int fd;
	char str[64];
}content_t;

typedef struct {
	info_t info;
	content_t data;
}message_t;

