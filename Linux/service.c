#include "service.h"
#include "infrared.h"

#define Max_Send_Length 8*1024
unsigned short int PORT = 9999;
static int socket_fd;
extern SHARED g_data;

int service_init()
{
	int s_ret;
	int opt = 1;
	struct sockaddr_in servaddr;
	socklen_t addrlen;

	socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_fd == -1) {
		perror("server->socket:");
		return SECCESS;
	}

#ifdef PRINTF_SIGN
printf("Socket create success\n");
#endif
	int yes = 1;
	setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, (void *)&yes, sizeof(yes));
	int nSendBufLen = 32*1024; //设置为32K
	setsockopt( socket_fd, SOL_SOCKET, SO_SNDBUF, ( const char* )&nSendBufLen, sizeof( int ) );

	memset(&servaddr, 0, sizeof(struct sockaddr_in));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(PORT);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	s_ret = bind(socket_fd, (struct sockaddr *)&servaddr, sizeof(struct sockaddr_in));
	while (s_ret == -1) {
		perror("server->bind:");
		servaddr.sin_port = htons(--PORT);
		s_ret = bind(socket_fd, (struct sockaddr *)&servaddr, sizeof(struct sockaddr_in));
		//close(socket_fd);
		//return 0;
	}
	
	
#ifdef PRINTF_SIGN 
printf("bind port:%d \nIP:%s\n",PORT,inet_ntoa(servaddr.sin_addr));
#endif

	s_ret = listen(socket_fd, MAX_THREAD);
	if (s_ret == -1) {
		perror("server->listen:");
		close(socket_fd);
		return 0;
	}
#ifdef PRINTF_SIGN
printf("server listend\n");
#endif
	
	pthread_mutex_init(&g_data.mutex,NULL);
	return 1;
}

int send_data(const char *data,const unsigned int length)
{
	static int LEN,len,pos,i,test;
	static ssize_t ret;
	static char send_buf[Max_Send_Length] = {0};
	LEN = length;

	for(i = 0; i < MAX_THREAD ; i++)
	{
		if(g_data.status[i] != UN_USE)
		{
			if ( LEN < 1024 )
			{
				pthread_mutex_lock(&g_data.mutex);
				ret = write(g_data.connfd[i], data, strlen(data));
				pthread_mutex_unlock(&g_data.mutex);
				
				if (-1 == ret )
				{
					perror("server->write");
					return FAILED;
				}
			}
			else
			{
			#if DEBUG
				if(test == 1)
				continue;
				test = 1;
			#endif
				char header[10] = {0};
				#if SEND_INFO
					printf("send pic,size:%d\n",LEN);
				#endif
				len = LEN;
				snprintf(header, sizeof(header), "H%d\r\n", LEN);
				pthread_mutex_lock(&g_data.mutex);
				ret = write(g_data.connfd[i], header, sizeof(header));
				for(pos = 0,ret = 0;len > 0;len = len - ret)
				{
				#ifdef DEBUG
					memset(send_buf, 0, sizeof(send_buf));
					if( Max_Send_Length <= len )
					{
						memcpy( send_buf, data+pos, Max_Send_Length );
						ret = write( g_data.connfd[i], send_buf, Max_Send_Length );
					}
					else
					{
						memcpy(send_buf, data+pos,len);
						ret = write( g_data.connfd[i], send_buf, len );
					}
				#else
					pthread_mutex_lock(&g_data.mutex);
					ret = write( g_data.connfd[i], data+pos, len);
					pthread_mutex_unlock(&g_data.mutex);
				#endif
					if (-1 == ret )
					{
						perror("server->write");
						return FAILED;
					}
				#if SEND_INFO
					printf("%x %x %x .len:%d  ret:%d  pos:%d \r\n",send_buf[0],send_buf[1],send_buf[2], len, ret, pos);
				#endif
					pos += ret;
				}
				
				pthread_mutex_unlock(&g_data.mutex);
			#if SEND_INFO
				//printf("len:%d  ret:%d  pos:%d \n", len, ret, pos);
				printf("%x %x %x .len:%d  ret:%d  pos:%d \r\n",send_buf[0],send_buf[1],send_buf[2], len, ret, pos);
			#endif
				return pos;
			}
			
		}
		else
			continue;
	}
		return SECCESS;
}

static int get_node()
{
	static int ret;
	for(ret = 0;ret < MAX_THREAD;ret++)
	{
		if(g_data.status[ret] == UN_USE)
		{
			return ret;
		}
		else
			continue;
	}

	if(ret == MAX_THREAD)
		return FAILED;
	
	return ret;
}

static int free_node(int index)
{
	pthread_mutex_lock(&g_data.mutex);
	g_data.status[index] = UN_USE;
	pthread_mutex_unlock(&g_data.mutex);
	printf("connfd index:%d Out\n",index,__LINE__);
	return SECCESS;
}

void *connect_process(void *arg)
{
    char buf[1024];
    int index = (int*)arg - g_data.connfd;
    int ret;
    reset_status();
    printf("connfd index:%d In\n",index);
    while(1)
    {
        memset(buf,0,sizeof(buf));

        int ret = recv(g_data.connfd[index], buf, sizeof(buf), 0);
        {
            if (-1 == ret)
            {
                printf("data error!\n");
                continue;
            }
            if (0 == ret)
            {
                printf("user shutdown!\n");
                break;
            }
        }
    }

    free_node( index );
    pthread_exit(NULL);

}

int service_run()
{
	socklen_t addrlen;
	struct sockaddr_in cliaddr;
	char buf[1024];
	pthread_t newthread;
	static int index;
	print_i("service is running\r\n");
	while (1) {
		index = get_node();
		if ( index == FAILED )
		{	
			continue;
		}
		addrlen = sizeof(struct sockaddr_in);
		memset(&cliaddr, 0, sizeof(struct sockaddr_in));
		
		g_data.connfd[index] = accept(socket_fd, (struct sockaddr *)&cliaddr, &addrlen);
		if (g_data.connfd[index] == -1) {
			perror("accept error");
			if( free_node(index) == SECCESS || free_node(index) == SECCESS );
				printf("free node failed!\n");
			continue;
		}

		#ifdef PRINTF_SIGN
			memset(buf,0,sizeof(buf));
			printf("accept info: client addr is %s,port %d\n", \
					inet_ntop(AF_INET,&cliaddr.sin_addr,buf,sizeof(buf)),\
					ntohs(cliaddr.sin_port));
		#endif
	
		if ( pthread_create(&newthread , NULL, (void *)connect_process, (void *)&g_data.connfd[index]) )
            perror("connect pthread_create");
		else
		{
			g_data.status[index] = USED;
		}
	}
	
	pthread_mutex_destroy(&g_data.mutex);
	close(socket_fd);
	return 1;
}