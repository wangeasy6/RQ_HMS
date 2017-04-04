//  
//	dht11.h  
//  
#include "print_control.h"
#include <wiringPi.h>  
#include <stdio.h>  
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
  
typedef unsigned char uint8;
typedef unsigned int  uint16;
typedef unsigned long uint32;

#define HIGH_TIME 32  
#define dht11_Pin 1  //use gpio1 to read data 

uint32 dht11_databuf; 

uint8 dht11_init(void);
uint8 readSensorData(void);
void *dht11_run();