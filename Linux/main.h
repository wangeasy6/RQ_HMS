#ifndef __MAIN__
#define __MAIN__

#include "sharing_data.h"
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include "dht11.h"
#include "infrared.h"
#include "service.h"
//#include "camera.h"

enum __DEV
{
	infrared = 0,
	dht11,
	bee,
	camera,
};

#endif