#include "main.h"
SHARED g_data;
static int data_init()
{
	int i;
	for(i = 0;i < MAX_THREAD;i++)
		g_data.status[i] = UN_USE;
	return SECCESS;
}

int main(){
	
	//uint8 send_pic[250*1024]={0};
	unsigned int width = WIDTH;
	unsigned int height = HIGHT;
	int size;

	if( !( data_init() && dht11_init() && infrared_init() && \
		service_init() && bee_init() ) )//&& camera_init(DEV_PATH, &width, &height, &size)
	{
		print_e("init failed\n");
		return(0);
	}


/******************************    init    *********************************/

	pthread_t external_device[4];		//pthread_t infrared,dht11;camera,
	
	if( pthread_create(&external_device[bee],NULL,bee_run,(void*)1) != 0 ){
		print_e("Creat bee failed\n");
		exit(0);
	}

	print_i("Creat bee seccess\n");

	if( pthread_create(&external_device[infrared],NULL,infrared_run,(void*)1) != 0 ){
		print_e("Creat infrared failed\n");
		exit(0);
	}

	print_i("Creat infrared seccess\n");
	
	if( pthread_create(&external_device[dht11],NULL,dht11_run,(void*)1) != 0 ){
		print_e("Creat dht11_run failed\n");
		exit(0);
	}
	print_i("Creat dht11_run seccess\n");
#if 1
	if( pthread_create(&external_device[camera],NULL,camera_run,(void*)1) != 0 ){
		print_e("Creat camera_proess failed\n");
		exit(0);
	}
	print_i("Creat camera_proess seccess\n");
#endif
/*************************    pthread_create    ***************************/

	service_run();								//service_run

	//wait thread exit
	void *thrd_ret;
	if(!pthread_join(external_device[bee],&thrd_ret) )
		print_w("thread bee exited\n");
	else
		print_e("thread bee exit failed\n");
	if(!pthread_join(external_device[infrared],&thrd_ret) )
		print_w("thread infrared exited\n");
	else
		print_e("thread infrared exit failed\n");
	if(!pthread_join(external_device[dht11],&thrd_ret) )
		print_w("thread dht11 exited\n");
	else
		print_e("thread dht11 exit failed\n");
	if(!pthread_join(external_device[camera],&thrd_ret) )
		print_w("thread camera exited\n");
	else
		print_e("thread camera exit failed\n");

	return 0;
}