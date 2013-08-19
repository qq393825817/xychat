#ifndef _CHAT_SERVER
#define _CHAT_SERVER
void socket_create(int *socket_fd,struct sockaddr_in *server_addr);
void pthread_deal_client(void *arg);
int check_user(char *recv_buf);
#endif
