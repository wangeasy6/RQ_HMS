#include "main.h"

int main(){
	//uint8 send_pic[250*1024]={0};
	DATA *data = (DATA *) malloc( sizeof(DATA) );
	memset( data , 0, sizeof(DATA) );

	if( !( dht11_init() && infrared_init() && service_init() ) )//camera_init();
	{
		printf("init failed\n");
		return(0);
	}

	pthread_t infrared,dht11;//camera,
	int res;

	res = pthread_create(&infrared,NULL,infrared_run,(void*)1);
	if(res != 0){
		printf("Creat infrared failed\n");
		exit(res);
	}
	printf("Creat infrared seccess\n");
	
	res = pthread_create(&dht11,NULL,dht11_run,(void*)1);
	if(res != 0){
		printf("Creat dht11_run failed\n");
		exit(res);
	}
	printf("Creat dht11_run seccess\n");

	service_run();

	//Ïß³ÌÍË³ö
	void *thrd_ret;
	res = pthread_join(infrared,&thrd_ret);
	if(!res)
		printf("thread infrared exited\n");
	else
		printf("thread infrared exit failed\n");
	res = pthread_join(dht11,&thrd_ret);
	if(!res)
		printf("thread dht11 exited\n");
	else
		printf("thread dht11 exit failed\n");

	return 0;
}