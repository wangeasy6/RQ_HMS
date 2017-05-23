#ifndef __BEE__
#define __BEE__

#include "sharing_data.h"
#include <wiringPi.h>  
#include <stdio.h>  
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
 
#define DELAY 1
#define bee_Pin 5

#define START 1
#define STOP 0

int bee_init();
void *bee_run(void *arg);
void bee_start();
void bee_stop();

#endif