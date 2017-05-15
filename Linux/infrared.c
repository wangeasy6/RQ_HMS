#include "infrared.h"

int status = -1;
pthread_mutex_t infrared_mutex;
extern int send_data(const char *data,const unsigned int length);

int infrared_init()
{
	pinMode (infrared_Pin, INPUT);
#ifdef PRINTF_SIGN
	printf("Use WiringPi_GPIO%d to infrared!\n",infrared_Pin);
#endif

	pthread_mutex_init(&infrared_mutex,NULL);
	return 1;
}

void reset_status()
{
	pthread_mutex_lock(&infrared_mutex);
	status = -1;
	pthread_mutex_unlock(&infrared_mutex);
	return;
}

void *infrared_run(void *arg)
{
	char buf[3] = {0};
	int read_data;
	
	buf[0] = 'I';
	buf[2] = '\0';
	
	while(1)
	{
		read_data = digitalRead(infrared_Pin);
		if( read_data == status )
		{
			sleep(1);
			continue;
		}
		if( read_data )
		{
			
			bee_stop();
			buf[1] = '0';
		}
		else				//somebody broken in
		{
			bee_start();
			buf[1] = '1';
		}
		send_data(buf, strlen(buf));
		pthread_mutex_lock(&infrared_mutex);
		status = read_data;
		pthread_mutex_unlock(&infrared_mutex);
		sleep(1);
	}
	pthread_exit(NULL);
}