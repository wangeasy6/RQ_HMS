#include "main.h"
SHARED g_data;
static int data_init()
{
	printf("addr:%p,",&g_data);
	int i;
	for(i = 0;i < MAX_THREAD;i++)
		g_data.status[i] = UN_USE;
	return SECCESS;
}

int main(){
	
	//uint8 send_pic[250*1024]={0};

	if( !( data_init() && dht11_init() && infrared_init() && service_init() ) )//camera_init(); && bee_init()
	{
		printf("init failed\n");
		return(0);
	}


/******************************    init    *********************************/

	pthread_t external_device[4];		//pthread_t infrared,dht11;camera,

	if( pthread_create(&external_device[infrared],NULL,infrared_run,(void*)1) != 0 ){
		printf("Creat infrared failed\n");
		exit(0);
	}

#ifdef PRINTF_SIGN
	printf("Creat infrared seccess\n");
#endif
	
	if( pthread_create(&external_device[dht11],NULL,dht11_run,(void*)1) != 0 ){
		printf("Creat dht11_run failed\n");
		exit(0);
	}
#ifdef PRINTF_SIGN
	printf("Creat dht11_run seccess\n");
#endif
/*************************    pthread_create    ***************************/

	service_run();								//service_run

	//Ïß³ÌÍË³ö
	void *thrd_ret;
	if(!pthread_join(external_device[infrared],&thrd_ret) )
#ifdef PRINTF_SIGN
		printf("thread infrared exited\n");
#endif
	else
		printf("thread infrared exit failed\n");
	if(!pthread_join(external_device[dht11],&thrd_ret) )
#ifdef PRINTF_SIGN
		printf("thread dht11 exited\n");
#endif
	else
		printf("thread dht11 exit failed\n");

	return 0;
}