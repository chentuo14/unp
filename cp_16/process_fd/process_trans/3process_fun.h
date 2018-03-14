#ifndef _3PROCESS_FUN_H_
#define _3PROCESS_FUN_H_

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

void send_fd(int sock, int sendfd);
int recv_fd(int sock);

#endif
