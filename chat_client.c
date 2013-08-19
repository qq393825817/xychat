/*
 * =====================================================================================
 *
 *       Filename:  chat_client.c
 *
 *    Description:  简单的聊天程序，客户端。实现简单跟服务端之间的联系，发送消息
 *
 *        Version:  1.0
 *        Created:  2013年08月13日 15时26分07秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  lvwei
 *        Company:  none
 *
 * =====================================================================================
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <error.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include "chat_client.h"
#include "daemon.c"

int main( int argc, char *argv[] )
{
	struct sockaddr_in client_addr;
	int socket_fd;
	int link_number = 0;

	if(argc != 5)
	{
		printf("Usage : -p server_port -i server_address\n");
		exit(1);
	}
	socket_create(&socket_fd,&client_addr,argc,argv);
	connect_server(&client_addr,socket_fd);
	while(link_number < 3)
	{
		if(input_userinfo(socket_fd) == 0)
		{
			printf("userinfo correct!\n");
			break;
		}
		link_number++;
	}
	if(link_number == 3)
	{
		printf("too many times to try!\n");
		exit(1);
	}
	return 0;
}

void connect_server(struct sockaddr_in *client_addr,int socket_fd)
{
	if(connect(socket_fd,(struct sockaddr *)client_addr,sizeof(struct sockaddr_in)) < 0)
	{
		perror("connect");
		exit(1);
	}

	input_userinfo(socket_fd);
}
void get_userinfo(char *userinfo,int len)
{
	int i;
	char c;

	if(userinfo == NULL)
	{
		perror("userinfo");
		exit(1);
	}

	i = 0;
	while(((c = getchar()) != '\n') && (c != EOF) && (i < len-1))
	{
		userinfo[i++] = c;
	}
	userinfo[i] = '\0';
}
int input_userinfo(int socket_fd)
{
	char username[32];
	char passwd[32];
	char user_info[65];
	char recv_buf[256];

	printf("username:\n");
	get_userinfo(username,32);
	printf("passwd:\n");
	get_userinfo(passwd,32);
	strncpy(user_info,username,strlen(username));
	user_info[strlen(username)] = '&';
	strncpy(user_info+strlen(username)+1,passwd,strlen(passwd));

	if((send(socket_fd,user_info,strlen(user_info),0)) < 0)
	{
		perror("send");
		exit(1);
	}
	if((recv(socket_fd,recv_buf,sizeof(recv_buf),0)) < 0)
	{
		perror("recv");
		exit(1);
	}

	if(recv_buf[0] == 'y');
	{
		printf("login succeed\n");
		return 1;
	}
	if(recv_buf[0] == 'n')
	{
		printf("user infomation error\n");
		return 0;
	}
	return -1;

}

void socket_create(int *socket_fd,struct sockaddr_in *client_addr,int argc,char *argv[])
{
	int i;

	memset(client_addr,0,sizeof(struct sockaddr_in));
	client_addr->sin_family = AF_INET;
	for(i=1 ; i<argc ;i++)
	{
		if(strncmp(argv[i],"-p",2) == 0)
		{
			client_addr->sin_port = htonl(atoi(argv[i+1]));
			continue;
		}
		if(strncmp(argv[i],"-i",2) == 0)
		{
			if(inet_aton(argv[i+1],&client_addr->sin_addr) == 0)
			{
				printf("invalid server ip address \n");
				exit(1);
			}
			continue;
		}
	}

	if(client_addr->sin_port == 0 || client_addr->sin_addr.s_addr == 0)
	{
		printf("invalid ip or port \n");
		exit(1);
	}

	(*socket_fd) = socket(AF_INET,SOCK_STREAM,0);
	if((*socket_fd) < 0)
	{
		perror("socket create");
		exit(1);
	}
}
