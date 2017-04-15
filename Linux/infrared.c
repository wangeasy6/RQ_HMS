#include "infrared.h"

extern int send_data(const char *data);

int infrared_init()
{
	pinMode (infrared_Pin, INPUT);
#ifdef PRINTF_SIGN
	printf("Use WiringPi_GPIO%d to infrared!\n",infrared_Pin);
#endif
	return 1;
}

void *infrared_run(void *arg)
{
	static char buf[3] = {0};
	buf[0] = 'I';
	buf[2] = '\0';
	while(1)
	{
		if( digitalRead(infrared_Pin) )
		{
			buf[1] = '0';
		}
		else
		{
			buf[1] = '1';
		}
			send_data(buf);
			sleep(1);
	}
	pthread_exit(NULL);
}