#include "bee.h"

static volatile unsigned char bee_control = STOP;

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
}

void bee_start()
{
	if( bee_control == START )
		return;
	bee_control = START;
}

void bee_stop()
{
	if( bee_control == STOP )
		return;
	bee_control = STOP;
	digitalWrite(bee_Pin, 1);
}