#include "bee.h"

static volatile unsigned char bee_control = STOP;
pthread_mutex_t *bee_mutex;

int bee_init()
{
	if (-1 == wiringPiSetup()) {  
		printf("Setup wiringPi failed!");  
		return 0;  
	}
	pinMode (bee_Pin, OUTPUT);
#ifdef PRINTF_SIGN
	printf("Use WiringPi_GPIO%d to bee!\n",bee_Pin);
#endif

	bee_mutex = (pthread_mutex_t *) malloc( sizeof(pthread_mutex_t) );
	pthread_mutex_init(bee_mutex,NULL);
	return 1;
}
void *bee_run(void *arg)
{
	while(1)
	{
		if(bee_control == STOP)
			continue;
		digitalWrite(bee_Pin, 0);
		//delayMicroseconds(DELAY);
		delay(DELAY);
		digitalWrite(bee_Pin, 1);
		//delayMicroseconds(DELAY);
		delay(DELAY);
	}
	
	pthread_mutex_destroy(bee_mutex);
	pthread_exit(NULL);
}

void bee_start()
{
	if( bee_control == START )
		return;
	pthread_mutex_lock(bee_mutex);
	bee_control = START;
	pthread_mutex_unlock(bee_mutex);
}

void bee_stop()
{
	if( bee_control == STOP )
		return;
	pthread_mutex_lock(bee_mutex);
	bee_control = STOP;
	pthread_mutex_unlock(bee_mutex);
	digitalWrite(bee_Pin, 1);
}