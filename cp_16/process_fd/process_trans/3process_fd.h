#ifndef _3PROCESS_FD_H_
#define _3PROCESS_FD_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

using namespace std;

#include <sys/socket.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/uio.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <strings.h>
#include <errno.h>
#include "3process_fun.h"

typedef struct {
	int index;
	int chanel[2];
}process_t;

#define MAXPROCESS 3
#define MAXLINE 100
#define LISTENQ 100

#define SERV_PORT  9876

#endif
