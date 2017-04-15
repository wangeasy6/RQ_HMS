#ifndef __SHARING_DATA__
#define __SHARING_DATA__
#include <pthread.h>

#define FAILED -1
#define SECCESS 1

#define USED 1
#define UN_USE 0

#define PRINTF_SIGN
//#define SEND_INFO

#define MAX_THREAD 10
#define DEV_NUM 4

typedef struct __SHARED
{
	volatile unsigned char status[MAX_THREAD];
	volatile int connfd[MAX_THREAD];
	pthread_mutex_t mutex;
}SHARED;

#define fun_in() printf("fun:%s Line: %d,In\n",__FUNCTION__,__LINE__)
#define fun_out() printf("fun:%s Line: %d,Out\n",__FUNCTION__,__LINE__)
#endif