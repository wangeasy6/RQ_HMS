//  
//	dht11.c 
//  
#include "dht11.h"

extern int connfd;
extern int send_data(const char *data);

uint8 dht11_init()
{
	if (-1 == wiringPiSetup()) {  
		printf("Setup wiringPi failed!");  
		return 0;  
	}

#ifdef PRINTF_SIGN
	printf("Use WiringPi_GPIO%d to dht11!\n",dht11_Pin);
#endif

	return 1;
}

uint8 readSensorData(void)  
{  
    uint8 crc;   
    uint8 i;  
   
    pinMode(dht11_Pin,OUTPUT); // set mode to output  
    digitalWrite(dht11_Pin, 0); // output a high level   
    delay(25);  
    digitalWrite(dht11_Pin, 1); // output a low level   
    pinMode(dht11_Pin, INPUT); // set mode to input  
    pullUpDnControl(dht11_Pin,PUD_UP);  
  
    delayMicroseconds(27);  
    if(digitalRead(dht11_Pin)==0) //SENSOR ANS  
    {  
		while(!digitalRead(dht11_Pin)); //wait to high  
  
		for(i=0;i<32;i++)  
		{  
			while(digitalRead(dht11_Pin)); //data clock start  
			while(!digitalRead(dht11_Pin)); //data start  
			delayMicroseconds(HIGH_TIME);  
			dht11_databuf*=2;  
			if(digitalRead(dht11_Pin)==1) //1  
			{  
					dht11_databuf++;  
			}  
		}  
  
		for(i=0;i<8;i++)  
		{  
			while(digitalRead(dht11_Pin)); //data clock start  
			while(!digitalRead(dht11_Pin)); //data start  
			delayMicroseconds(HIGH_TIME);  
			crc*=2;    
			if(digitalRead(dht11_Pin)==1) //1  
			{  
                crc++;  
			}  
		}  
		return 1;  
   }  
   else  
   {  
        return 0;  
   }  
}

void *dht11_run(void *arg)
{
	static char buf[8] = {0};
	while(1){
		if(connfd != -1)
		{
			if( readSensorData() )  
			{  
				/*
			   printf("Congratulations ! Sensor data read ok!\n");  
			   printf("RH:%d.%d\n",(dht11_databuf>>24)&0xff,(dht11_databuf>>16)&0xff);   
			   printf("TMP:%d.%d\n",(dht11_databuf>>8)&0xff,dht11_databuf&0xff);  
			   */
				memset(buf,0,sizeof(buf));
				buf[0] = 'E';
				snprintf(&buf[1],sizeof(buf),"%d,%d",(dht11_databuf>>24)&0xff,(dht11_databuf>>8)&0xff);
				send_data(buf);
			    dht11_databuf=0;
			}
			else
			{
				printf("read dht11 error!\n");
			}
		}
			sleep(1);
	}
}