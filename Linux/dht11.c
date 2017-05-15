//  
//	dht11.c 
//  
#include "dht11.h"

extern int connfd;
extern int send_data(const char *data,const uint16 length);

uint32 dht11_databuf; 
uint16 temp_threshold = 45;

uint8 set_temp_threshold(uint16 temp_num)
{
	temp_threshold = temp_num;
	return SECCESS;
}

uint8 dht11_init()
{
	if (-1 == wiringPiSetup()) {  
		printf("Setup wiringPi failed!");  
		return FAILED;  
	}

#ifdef PRINTF_SIGN
	printf("Use WiringPi_GPIO%d to dht11!\n",dht11_Pin);
#endif

	return SECCESS;
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
		return SECCESS;  
   }  
   else  
   {  
        return FAILED;  
   }  
}

static uint8 verify_value(uint8 num)
{
	static signed char value[10] = {-128,-128,-128,-128,-128,-128,-128,-128,-128,-128};
	static uint8 index = 0;
	int i,sum,average;
	if(value[0] == -128)
	{
		value[index] = num;
		index ++;
		return SECCESS;
	}
	
	for(i=0,sum=0;i<10 && value[i] != -128;i++)
	{
		sum += value[i];
	}
	average = sum/i;
	
	if(num > average+5 || num < average-5 )
	{
		//printf("filter num: %d , avr: %d\n",num,average);
		return FAILED;
	}
	else
	{
		value[index] = num;
		index ++;
		if(index = 10)
			index = 0;
	}
	
	return SECCESS;
}

void *dht11_run(void *arg)
{
	static char buf[8] = {0};
	buf[0] = 'E';
	while(1){
		if( readSensorData() == SECCESS)  
		{  
			/*
			  printf("Congratulations ! Sensor data read ok!\n");  
			  printf("RH:%d.%d\n",(dht11_databuf>>24)&0xff,(dht11_databuf>>16)&0xff);   
			  printf("TMP:%d.%d\n",(dht11_databuf>>8)&0xff,dht11_databuf&0xff);  
			*/
			//verify value
			if( verify_value( (dht11_databuf>>8)&0xff ) == FAILED)
				continue;
			
			if( ((dht11_databuf>>8)&0xff) >= temp_threshold )
			{
				bee_start();
			}
			else
			{
				bee_stop();
			}
			
			memset(buf + 1, 0, sizeof(buf)-1);
			snprintf(&buf[1],sizeof(buf),"%d,%d",(dht11_databuf>>24)&0xff,(dht11_databuf>>8)&0xff);
			send_data(buf, sizeof(buf) );
			
			dht11_databuf=0;
		}
		else
		{
			print_e("read dht11 error!\n");
		}
		sleep(1);
	}
	pthread_exit(NULL);
}
