#ifndef __INFR__
#define __INFR__

#include "sharing_data.h"
#include "bee.h"
#include <wiringPi.h>
#include <stdio.h>
#include <string.h>

#define infrared_Pin 4

int infrared_init();
void reset_status();
void *infrared_run(void *arg);

#endif