#include "service.h"

#define MAX_BACKLOG 10
unsigned short int PORT = 9999;
static int socket_fd;
volatile int connfd = -1;

int service_init()
{
	int s_ret;
	int opt = 1;
	struct sockaddr_in servaddr;
	socklen_t addrlen;

	socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_fd == -1) {
		perror("server->socket:");
		return 0;
	}

#ifdef PRINTF_SIGN
printf("Socket create success\n");
#endif
	//setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

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

	s_ret = listen(socket_fd, MAX_BACKLOG);
	if (s_ret == -1) {
		perror("server->listen:");
		close(socket_fd);
		return 0;
	}
#ifdef PRINTF_SIGN
printf("server listend\n");
#endif
	return 1;
}

int send_data(const char *data)
{
	int len,l,ret;
	len = l = strlen(data);
	for(;len > 0;l = l - ret)
	{
		ret = write(connfd, data, strlen(data));
		if (-1 == ret )
		{
			perror("server->write");
			return 0;
		}
	}
		return 1;
}

int service_run()
{
	socklen_t addrlen;
	struct sockaddr_in cliaddr;
	char buf[1024];
	while (1) {
		addrlen = sizeof(struct sockaddr_in);
		memset(&cliaddr, 0, sizeof(struct sockaddr_in));
		connfd = accept(socket_fd, (struct sockaddr *)&cliaddr, &addrlen);
		if (connfd == -1) {
			perror("accept error");
			break;
			//continue;
		}
#ifdef PRINTF_SIGN
	memset(buf,0,sizeof(buf));
	printf("accept info: client addr is %s,port %d\n", \
			inet_ntop(AF_INET,&cliaddr.sin_addr,buf,sizeof(buf)),\
			ntohs(cliaddr.sin_port));
#endif

#ifdef PRINTF_SIGN
		printf("connfd closed\n");
		connfd = -1;
#endif
	}
	return 1;
}