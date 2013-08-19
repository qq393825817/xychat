/*
 * =====================================================================================
 *
 *       Filename:  chat_server.c
 *
 *    Description:  简单的聊天程序，服务端。实现简单的对客户端的处理，对客户端的控制。
 *
 *        Version:  1.0
 *        Created:  2013年08月13日 09时12分56秒
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
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <pthread.h>
#include "chat_server.h"
#include "daemon.c"

#define LISTEN         10
#define USENAME         0
#define PASSWORD        1
#define SERVER_PORT    10150
struct userinfo
{
	char username[32];
	char passwd[32];
};

struct userinfo users[] = {
	{"linux","linux"},
	{"lw","lw"},
	{" "," "}
};



int main(int argc,char *argv[])
{
	struct sockaddr_in server_addr;
	int socket_fd,client_socket_fd;
	int result;
	pthread_t thread;
	fd_set readfds;

	FD_ZERO(&readfds);
	FD_SET(client_socket_fd,&readfds);

	socket_create(&socket_fd,&server_addr);
	if(bind(socket_fd,(struct sockaddr *)&server_addr,sizeof(struct sockaddr_in)) < 0)	
	{
		perror("bind");
		exit(1);
	}
	if(listen(socket_fd,LISTEN) < 0)
	{
		perror("listen");
		exit(1);
	}
	while(1)
	{
		result = select(socket_fd+1,&readfds,NULL,NULL,NULL);
		switch(result)
		{
			case 0:
				break;
			case -1:
				perror("select");
				exit(1);
			default:
				if(FD_ISSET(socket_fd,&readfds))
				{
					pthread_create(&thread,NULL,(void *)pthread_deal_client,(void *)&socket_fd);
				}
		}
	}
	return 0;
}

void pthread_deal_client(void *arg)
{
	int socket_fd = (int)arg;
	int con_fd;
	struct sockaddr_in client_addr;
	socklen_t client_len;
	int ret,flag = 0;
	char recv_buf[256],login_info[32];

	printf("%d id has connected!!\n",socket_fd);

	client_len = sizeof(struct sockaddr_in);
	con_fd = accept(socket_fd,(struct sockaddr *)&client_addr,&client_len);
	if(con_fd < 0)
	{
		perror("accept");
		exit(1);
	}
	while(1)
	{
		if((ret = recv(con_fd,recv_buf,sizeof(recv_buf),0)) < 0)
		{
			perror("recv");
			exit(1);
		}

		flag = check_user(recv_buf);

		switch(flag)
		{
			case 1:
				strncpy(login_info,"y",1);
				if(send(con_fd,login_info,strlen(login_info),0) < 0)
				{
					perror("send");
					exit(1);
				}
				break;
			default:
				strncpy(login_info,"n",1);
				if(send(con_fd,login_info,strlen(login_info),0) < 0)
				{
					perror("send");
					exit(1);
				}
		}
		if(flag == 1)
		{
			break;
		}
	}

	if((send(con_fd,"welcome use xychat",20,0)) < 0)
	{
		perror("send");
		exit(1);
	}
	/*
	while(1)
	{
		
	}
	*/

	pthread_exit(0);
}

int check_user(char *recv_buf)
{
	int i,user_len,users_len,username_len,passwd_len;
	char username[32];
	char passwd[32];
	int flag = 0;

	user_len = strlen(recv_buf);
	for(i=0;i<256;i++)
	{
		if(recv_buf[i] == '&')
		{
			break;
		}
	}
	strncpy(username,recv_buf,i);
	strncpy(passwd,recv_buf+i+1,user_len-i-1);

	username_len = strlen(username);
	passwd_len = strlen(passwd);

	users_len = sizeof(users) / sizeof(users[0]);
	for(i=0; i<users_len; i++)
	{
		if((strncmp(users[i].username,username,username_len) == 0) &&  (strncmp(users[i].username," ",1) != 0))
		{
			flag = 1;
			if(strncmp(users[i].passwd,passwd,passwd_len) == 0)
			{
				return flag;
			}
			else
			{
				flag = 2;
				return flag;
			}
		}
	}
	if(i == users_len-1)
	{
		flag = 2;
		return flag;
	}

	return -1;
}

void socket_create(int *socket_fd,struct sockaddr_in *server_addr)
{
	memset(server_addr,0,sizeof(struct sockaddr_in));
	server_addr->sin_family = AF_INET;
	server_addr->sin_port = htons(SERVER_PORT);
	server_addr->sin_addr.s_addr = htonl(INADDR_ANY);

	(*socket_fd) = socket(AF_INET,SOCK_STREAM,0);
	if((*socket_fd) < 0)
	{
		perror("socket create");
		exit(1);
	}
}
