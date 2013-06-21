/*
 * Server.h
 *
 *  Created on: Jun 21, 2013
 *      Author: Arvind A. de Menezes Pereira
 */

#ifndef SERVER_H_
#define SERVER_H_

#include <iostream>
#include <cstdio>
//#include <cunistd>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <pthread.h>
#include <string.h>
#include <vector>
#include <map>

using std::vector;
using std::map;

#define MAX_BUFSIZE 4096

typedef struct
{
	int client_fd;
	int client_id;
	long int read_subscription_flags;
	long int write_subscription_flags;
	char client_name[40];
}ClientInfo;

class TCP_Server {
	int numClients;
	int server_sockfd, client_sockfd;
	struct sockaddr_in server_address;
	struct sockaddr_in client_address;
	socklen_t server_len, client_len;
	fd_set readfds, testfds;
	unsigned char buf[MAX_BUFSIZE];
	int numClientConnected;
	map<int,int> client_fds;
	map<int,ClientInfo> ClientTable;
	bool terminateListener;
	pthread_t listener;
public:
	TCP_Server();
	~TCP_Server() {}
	unsigned int calc_crc(unsigned char *ptr, int count);
	int send_frame(int fd,unsigned char cmd,unsigned char dest_id,unsigned char *data,int len);
	int add_client(int fd);
	void remove_client(int fd);
	void CreateSocket();
	void CloseSocket();
	void StartServer();
	void StopServer();

	virtual void read_client(int fd);
	virtual void check_command(int, int, unsigned char *);
	static void *listen_thread(void *arg);
 };



#endif /* SERVER_H_ */
