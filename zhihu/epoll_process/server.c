#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>

#define WORKER_MAX 		1024
#define EVENT_LIST_MAX	128
#define EVENT_MAX		12
#define WORK_REAL		8
#define SERVER_PORT		8080

static int workers[WORKER_MAX];
static int icEpollFd = -1;
static int cur_pid;

typedef int (*PFCALLBACL)(struct epoll_event *);
typedef struct EPOLL_DATA_S {
	int iEpoll_Fd;
	int iEvent_Fd;
	PFCALLBACL pfCallBack;
} Epoll_Data_S;

//互斥量
static pthread_mutex_t *mutex;

//创建共享mutex
static void initMutex(void)
{
	pthread_mutexattr_t attr;
	int ret;

	//设置互斥量为进程间共享
	mutex = (pthread_mutex_t *)mmap(NULL, sizeof(pthread_mutex_t),
			PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANON, -1, 0);
	if(MAP_FAILED == mutex) {
		perror("mutex mmap failed");
		return;
	}

	//设置attr的属性
	pthread_mutexattr_init(&attr);
	ret = pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
	if(ret != 0) {
		fprintf(stderr, "mutex set shared failed");
		return;
	}
	pthread_mutex_init(mutex, &attr);

	return;
}

static int startup(unsigned short port)
{
	struct sockaddr_in servAddr;
	unsigned value = 1;
	int listenFd;

	memset(&servAddr, 0, sizeof(servAddr));

	//协议域(ip地址和端口)
	servAddr.sin_family = AF_INET;
	//绑定默认网卡
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	//端口
	servAddr.sin_port = htons(port);

	//创建套接字
	if((listenFd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		printf("create socket error: %s(errno: %d)\n", strerror(errno), errno);
		return 0;
	}

	setsockopt(listenFd, SOL_SOCKET, SO_REUSEADDR, &value, sizeof(value));

	//绑定套接字
	if(bind(listenFd, (struct sockaddr *)&servAddr, sizeof(servAddr))) {
		printf("bind socket error: %s(errno: %d)\n", strerror(errno), errno);
		return 0;
	}

	//开始监听，设置最大连接请求
	if(listen(listenFd, 10) == -1) {
		printf("listen socket error: %s(errno: %d)\n", strerror(errno), errno);
		return 0;
	}

	return listenFd;
}

static int create_workers(unsigned int worker_num)
{
	unsigned int i;
	unsigned int real_num = worker_num;
	int pid;

	if(real_num > WORKER_MAX) {
		real_num = WORKER_MAX;
	}

	for(i=0;i<real_num;i++) {
		pid = fork();
		if(pid == 0) {
			return 0;
		} else if(pid > 0) {
			workers[i] = pid;
			continue;
		} else {
			printf("fork error\r\n");
			return 0;
		}
	}

	return 1;
}

static int create_epoll(unsigned int event_num)
{
	int epoll_fd;

	epoll_fd = epoll_create(event_num);
	if(epoll_fd == -1) {
		printf("epoll create failed\r\n");
		return -1;
	}

	return epoll_fd;
}

static void add_event_epoll(int iEpoll_Fd, int iEpvent_Fd, PFCALLBACL pfCallBack)
{
	int op = EPOLL_CTL_ADD;
	struct epoll_event ee;
	Epoll_Data_S *data;

	data = malloc(sizeof(Epoll_Data_S));
	if(data == NULL) {
		return;
	}

	data->iEpoll_Fd = iEpoll_Fd;
	data->iEvent_Fd = iEvent_Fd;
	data->pfCallBack = pfCallBack;

	ee.events = EPOLLIN | EPOLLOUT | EPOLLHUP;
	ee.data.ptr = (void void*)data;

	if(epoll_ctl(iEpoll_Fd, op, iEvent_Fd, &ee) == -1) {
		printf("epoll_ctl(%d, %d) failed", op, iEvent_Fd);
		return;
	}

	return;
}

static void del_event_epoll(int iEpoll_Fd, int iEvent_Fd)
{
	int op = EPOLL_CTL_DEL;

	if(epoll_ctl(iEpoll_Fd, op, iEvent_Fd, NULL) == -1) {
		printf("epoll_ctl(%d, %d) failed", op, iEvent_Fd);
	}

	return;
}

static int make_socket_non_blocking(int fd)
{
	int flags, s;

	flags = fcntl(fd, F_GETFL, 0);
	if(flags == -1) {
		perror("fcntl");
		return -1;
	}

	flags |= O_NONBLOCK;
	s = fcntl(fd, F_SETFL, flags);
	if(s == -1) {
		perror("fcntl");
		return -1;
	}

	return 0;
}

static int proc_receive(struct epoll_event *pstEvent)
{
	char buff[4096];
	int len;
	Epoll_Data_S *data = (Epoll_Data_S *)(pstEvent->data.ptr);
	int iEpoll_Fd = data->iEpoll_Fd;
	int iEvent_Fd = data->iEvent_Fd;

	if(pstEvent->events & EPOLLIN) {
		while(1) {
			len = (int)recv(iEvent_Fd, buff, sizeof(buff), 0);
			if(len <= 0) {
				if(errno == EINTR) {
					continue;
				}

				del_event_epoll(iEpoll_Fd, iEvent_Fd);
				close(iEvent_Fd);
				free(data);
			} else if(len > 0) {
				buff[len] = '\0';
				printf("pid %d receive data: %s\r\n", cur_pid, buff);
			}

			break;
		}
	} else if(pstEvent->events & EPOLLHUP) {
		printf("receive EPOLLHUP or EPOLLOUT\r\n");
		del_event_epoll(iEpoll_Fd, iEvent_Fd);
		close(iEvent_Fd);
		free(data);
	} else {
		//printf("receive others pstEvent->events=%d\r\n", pstEvent->events);
	}

	return 0;
}

//处理Accept事件
static int proc_accept(struct epoll_event *pstEvent)
{
	int newFd;
	Epoll_Data_S *data = (Epoll_Data_S *)(pstEvent->data.ptr);
	int iEpoll_Fd = data->iEpoll_Fd;
	int iEvent_Fd = data->iEvent_Fd;

	if(pthread_mutex_trylock(mutex) == 0) {
		while((newFd = accept(iEvent_Fd, (struct sockaddr *)NULL, NULL)) != -1) {
			make_socket_non_blocking(newFd);
			add_event_epoll(icEpollFd, newFd, proc_receive);
		}

		pthread_mutex_unlock(mutex);
	}

	return 0;
}

static void handleterm(int sig)
{
	int i;

	for(i=0;i<WORK_REAL;i++) {
		kill(workers[i], SIGTERM);
	}

	return;
}

static void proc_epoll(int iEpollFd, int timeout)
{
	int iEventNum;
	int i;
	struct epoll_event events[EVENT_LIST_MAX];

	iEventNum = epoll_wait(iEpollFd, events, EVENT_LIST_MAX, timeout);
	for(i=0;i<iEventNum;i++) {
		Epoll_Data_S *data = (Epoll_Data_S *)(events[i].data.ptr);
		data->pfCallBack(&(events[i]));
	}

	return;
}

int main()
{
	int iServerFd = -1;
	int bParent;
	int iEpollFd = -1;

	initMutex();

	iServerFd = startup(SERVER_PORT);
	if(iServerFd == -1) {
		return 0;
	}

	make_socket_non_blocking(iServerFd);

	iEpollFd = create_epoll(EVENT_MAX);
	if(iEpollFd == -1) {
		close(iServerFd);
		return 0;
	}

	add_event_epoll(iEpollFd, iServerFd, proc_accept);

	bParent = create_workers(WORK_REAL);

	if(bParent) {
		while(1) {
			signal(SIGTERM, handleterm);
			pause();
		}
	} else {
		icEpollFd = create_epoll(EVENT_MAX);
		if(icEpollFd == -1) {
			close(iServerFd);
			return 0;
		}
		
		cur_pid = getpid();

		while(1) {
			proc_epoll(iEpollFd, 50);

			proc_epoll(icEpollFd, 50);
		}
	}

	return 0;
}
