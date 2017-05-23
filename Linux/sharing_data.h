#ifndef __SHARING_DATA__
#define __SHARING_DATA__
#include <pthread.h>

#define FAILED -1
#define SECCESS 1

#define USED 1
#define UN_USE 0
/****    		1|0   <--->   USE|UNUSE *****/
#define PRINTF_SIGN 1
#define SEND_INFO 0
#define DEBUG 0

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

#if PRINTF_SIGN
#define print_i(a) printf("i. %s",a)
#define print_w(a) printf("w. %s",a)
#else
#define print_i(a) //
#define print_w(a) //
#endif

#if SEND_INFO
#define print_s(a) printf("s. %s",a)
#else
#define print_s(a) //
#endif

#define print_e(a) printf("e. %s",a)

#endif