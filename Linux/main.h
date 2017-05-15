#ifndef __MAIN__
#define __MAIN__

#include "sharing_data.h"
#include "dht11.h"
#include "infrared.h"
#include "service.h"
#include "bee.h"
#include "camera.h"

#include <pthread.h>
#include <stdlib.h>
#include <string.h>

enum __DEV
{
	infrared = 0,
	dht11,
	bee,
	camera,
};

#endif