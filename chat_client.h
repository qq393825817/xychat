#ifndef _CHAT_CLIENT
#define _CHAT_CLIENT
	void socket_create(int *socket_fd,struct sockaddr_in *server_addr,int argc,char *argv[]);
	void connect_server(struct sockaddr_in *client_addr,int socket_fd);
	int input_userinfo(int socket_fd);
#endif

