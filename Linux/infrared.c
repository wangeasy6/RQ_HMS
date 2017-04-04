#include "infrared.h"

extern int connfd;
extern int send_data(const char *data);

int infrared_init()
{
	pinMode (infrared_Pin, INPUT);
	printf("Use WiringPi_GPIO%d to infrared!\n",infrared_Pin);
	return 1;
}

void *infrared_run(void *arg)
{
	static char buf[2] = {0};
	while(1)
	{
		if(connfd != -1)
		{
			if( digitalRead(infrared_Pin) )
			{
				buf[0] = '1';
				
			}
			else
			{
				buf[0] = '0';
			}
				send_data(buf);
				printf("%s\n",buf);
				sleep(1);
		}
	}
}