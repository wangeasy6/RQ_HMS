#ifndef __WEB__
#define __WEB__

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <semaphore.h>

#define PRINTF_SIGN

typedef struct
{
	char send_info[8];
	int connfd;

}DATA;

#endif