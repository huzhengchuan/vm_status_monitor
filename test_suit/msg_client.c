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
#define FILE_NAME_MAX_SIZE 128
#define BUFFER_SIZE  256

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

	if(bind(client_socket, (struct sockaddr *)&client_addr, sizeof(client_addr)) < 0)
	{
		printf("bind socket failed.\r\n");
		return;
	}

	struct sockaddr_in server_addr;
	bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	inet_aton("127.0.0.1", &server_addr.sin_addr);
	server_addr.sin_port = htons(PORT);

	socklen_t server_addr_length = sizeof(server_addr);

	if(connect(client_socket, (struct sockaddr *)&server_addr, server_addr_length) < 0)
	{
		printf("can not connect the server.");
		return ;
	}

	FILE *fp = fopen("log.txt", "w");
	if(NULL == fp)
	{
		printf("FIle can not open to write\r\n");
		return;
	}

	int length = 0;
	char buffer[BUFFER_SIZE + 1];

	for(;;)
	{

		printf("please input cmd to server:\t");
		
		bzero(buffer, BUFFER_SIZE + 1);
		scanf("%s", buffer);

		send(client_socket, buffer, BUFFER_SIZE, 0);
	
		while(length = recv(client_socket , buffer, BUFFER_SIZE, 0))
		{
			if(length < 0)
			{
				printf("receive data from server failed.\r\n");
				break;
			}
			int write_length = fwrite(buffer, sizeof(char), length, fp);
			if(write_length < length)
			{
				printf("File write failed\r\n");
				break;
			}
			bzero(buffer, BUFFER_SIZE);
		}
	}

	printf("receive file from server success\r\n");

	fclose(fp);
	close(client_socket);
	return;

}
