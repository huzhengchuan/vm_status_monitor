#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ev.h>
#include <errno.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>

#define PORT 8033
#define MAX_FD 500
#define CONNECT_TIMEOUT 5.0

struct client_context
{

};

struct libev_context
{
	struct ev_io watcher;
	struct ev_timer timer;
	struct client_context context;
};

void accept_cb(EV_P_ ev_io *w, int revents);
void client_cb(EV_P_ ev_io *w, int revents);
void client_timer_cb(struct ev_loop *loop, ev_timer *w, int revents);

struct libev_context *client[MAX_FD];

int main()
{
	int sd;
	int t1; 
	struct ev_loop *loop = EV_DEFAULT; 
	struct sockaddr_in addr;
	struct ev_io accept_watcher;

	for(t1 = 0; t1 < MAX_FD; t1++)
	{
		client[t1] = NULL;
	}

	if(((sd = socket(PF_INET, SOCK_STREAM, 0)) < 0))
	{
		printf("socket error, code %d:%s \r\n", errno, strerror(errno));
		return 0;
	}
	
	printf("the sd=%d\r\n", sd);
	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(PORT);
	addr.sin_addr.s_addr = htons(INADDR_ANY);

	if(bind(sd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
	{
		printf("Bind error, code %d:%s\r\n", errno, strerror(errno));
		return 0;
	}

	if(listen(sd, 2) < 0)
	{
		printf("listen error, code %d:%s\r\n", errno, strerror(errno));
		return 0;
	}

	printf("server start working ::%d \r\n", PORT);

	ev_io_init(&accept_watcher, accept_cb, sd, EV_READ);
	ev_io_start(loop, &accept_watcher);
	
	ev_run(loop, 0);

	return 0;
}

void client_timer_cb(struct ev_loop *loop, ev_timer *w, int revents)
{
	printf("client not alive closer\r\n");
	int csd; 

	struct libev_context *clientcontext;
	clientcontext = (struct libev_context *)((char *)w - (int)(&((((struct libev_context *)0)->timer))));
	

	csd = clientcontext->watcher.fd;
	ev_timer_stop(loop, &(clientcontext->timer));
	free(clientcontext);
	clientcontext = NULL;
	close(csd);
	printf("context free ok\r\n");
}

void accept_cb(struct ev_loop *loop, ev_io *w, int revents)
{
	int client_sd = 0;
	struct sockaddr_in client_addr;
	socklen_t client_len = sizeof(struct sockaddr_in);

	if(EV_ERROR &revents)
	{
		printf("Error event in accpet, code %d:%s\r\n", errno, strerror(errno));
		return;
	}

	client_sd = accept(w->fd, (struct sockaddr *)(&client_addr), &client_len);
	if(client_sd < 0)
	{
		printf("Accept error, code %d:%s\r\n", errno, strerror(errno));
		return ;
	}

	if(client_sd > MAX_FD)
	{
		printf("Max fd thread\r\n");
		goto err_sd;
	}

	client[client_sd] = (struct libev_context *)malloc(sizeof(struct libev_context));
	if(client[client_sd] == NULL)
	{
		printf("Alloc context error, code %d:%s\r\n", errno, strerror(errno));
		goto err_sd;
	}

	printf("Context alloc ok for client_fd=%d \r\n", client_sd);

	ev_io_init(&(client[client_sd]->watcher), client_cb, client_sd, EV_READ);
	ev_io_start(loop, &(*client[client_sd]).watcher);
	
	ev_timer_init(&((*client[client_sd]).timer), client_timer_cb, CONNECT_TIMEOUT, 0);
	ev_timer_start(loop, &((*client[client_sd]).timer));
	printf("client connected success\r\n");

	return ;

err_sd:
	close(client_sd);
}

void client_cb(struct ev_loop *loop, ev_io *w, int revents)
{
	char buf[1024];
	ssize_t read;

	client[w->fd]->timer.repeat = CONNECT_TIMEOUT;
	
	ev_timer_again(loop, &((*client[w->fd]).timer));


	bzero(buf, 1024);
	if(EV_ERROR & revents)
	{
		printf("Error in read, code %d:%s \r\n", errno, strerror(errno));
		return;
	}

	read = recv(w->fd, buf, 1024, 0);
	if(read < 0)
	{
		printf("read Error, code %d:%s\r\n", errno, strerror(errno));
	}

	if(read == 0)
	{
		printf("Disconnected, code %d:%s\r\n", errno, strerror(errno));
		int t = w->fd;

		ev_io_stop(loop, &((*client[t]).watcher));

		ev_timer_stop(loop, &(client[t]->timer));
		free(client[t]);
		client[t] = NULL;
		close(w->fd);
		printf("context free ok\r\n");

	}

	printf("recv >%s\r\n", buf);
}



