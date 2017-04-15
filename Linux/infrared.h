#ifndef __INFR__
#define __INFR__

#include "sharing_data.h"
#include <wiringPi.h>  
#include <stdio.h>
#include <string.h>

#define infrared_Pin 4

int infrared_init();
void *infrared_run(void *arg);

#endif