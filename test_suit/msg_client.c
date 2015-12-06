#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>


#define PORT 8033
#define MAX_FD 500
#define CONNECT_TIMEOUT 5.0

int main()
{
	struct sockaddr_in client_addr;

	bzero(&client_addr, sizeof(client_addr));
	client_addr.sin_family = AF_INET;
	client_addr.sin_addr.s_addr = htons(INADDR_ANY);
	client_addr.sin_port = htons(0);

	int client_socket = socket(AF_INET, SOCK_STREAM, 0);
	if(client_socket < 0)
	{
		printf("create socket failedr\r\n");
		return;
	}

	if(bind(client_socket, (struct sockadr *)&client_Adddr, sizeof(client_socket)) < 0)
	{
		printf("bind socket failed.\r\n");
		return;
	}

	struct sockaddr_in server_addr;
	bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr = "127.0.0.1";
	server_addr.sin_port = htons(PORT);

	socklen_t server_addr_length = sizeof(server_addr);


}
