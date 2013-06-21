/*
 * Server.cpp
 *
 *  Created on: Jun 21, 2013
 *      Author: arvind
 */
#include <iostream>
#include <map>
#include "string.h"
#include "Server.h"

using namespace std;

TCP_Server::TCP_Server() {
	StartServer();
	sleep(1);
}

void TCP_Server::StartServer() {
	CreateSocket();
	numClientConnected = 0;
	terminateListener = false;
	cerr<<"Server Started"<<endl;
	int res=pthread_create(&listener,NULL,listen_thread, this );
	if(res!=0)
	{
		perror("Listen thread creation failed.");
		exit(EXIT_FAILURE);
	}
}

void TCP_Server::StopServer() {
	terminateListener = true;
	cerr<<"Stopping Server"<<endl;
	void *thread_result;
	int res = pthread_join(listener, &thread_result);
	cerr<<"Server Stopped";
}


void TCP_Server::CreateSocket()
{
	unlink("server_socket");
	server_sockfd=socket(AF_INET, SOCK_STREAM,0);

	server_address.sin_family=AF_INET;
	server_address.sin_addr.s_addr=htonl(INADDR_ANY);
	server_address.sin_port=htons(10000);
	server_len=sizeof(server_address);

	bind(server_sockfd, (struct sockaddr*)&server_address, server_len);

	// Create a connection queue and initialize readfds to handle input
	// from server_sockfd
	listen(server_sockfd,5);

	printf("Listen queue created... Awaiting clients...\n");
	FD_ZERO(&readfds);	// Initialize readfds with no fds...
	FD_SET(server_sockfd,&readfds);	// readfds has server_sockfd...
}

unsigned int TCP_Server::calc_crc(unsigned char *ptr, int count) {
  unsigned int crc,i;

  crc=0;
  while(--count>=0) {
    crc^=((int)*ptr)<<8; ptr++;
    for(i=0;i<8;i++)
      {
	if(crc&0x8000)
	  crc^=0x1021;
	else crc<<=1;
      }
  }
  return crc;
}

int TCP_Server::send_frame(int fd,unsigned char cmd,unsigned char dest_id,unsigned char *data,int len)
{
	int i;
	unsigned char frame_2_send[1024];
	memset(frame_2_send, 0, 1024);

	frame_2_send[0]=0xa3; // Our Protocol's SOF
	frame_2_send[1]=0x27; //DBEX_SERVER;
	frame_2_send[2]=dest_id;
	frame_2_send[3]=cmd;
	frame_2_send[4]=((len+6)/256)&0xff;
	frame_2_send[5]=(len+6)&0xff;
	for(i=6;i<(len+6);i++,data++)
	  frame_2_send[i]=*data;

	printf("Size = %d",i);
	int chksum=calc_crc(frame_2_send,len+6);
	frame_2_send[i]=(chksum>>8)&0xff;
	frame_2_send[i+1]=chksum&0xff;

	printf("\nSending... ");
	write(fd,frame_2_send,i+2);
	for(i=0;i<len+8;i++)
		printf("%d,",frame_2_send[i]);
}

int TCP_Server::add_client(int fd)
{
    client_len=sizeof(client_address);
    client_sockfd=accept(server_sockfd,(struct sockaddr*)&client_address, &client_len);
    FD_SET(client_sockfd, &readfds);
	++numClientConnected;
    printf("adding client on fd %d\n",client_sockfd);
    if( ClientTable.find(client_sockfd)!=ClientTable.end() ) {
    	cerr<<"This client already exists."<<client_sockfd;
    }
    else {
    	ClientInfo client; client.client_fd = client_sockfd;
    }
	for(map<int,ClientInfo>::iterator it=ClientTable.begin();it!=ClientTable.end();++it)
		printf("\nClient[%d] is at fd=%d ",it->first,it->second.client_fd);

    return client_sockfd;
}

void TCP_Server::remove_client(int fd)
{
	close(fd);
	FD_CLR(fd,&readfds);
	--numClientConnected;

}


void *TCP_Server::listen_thread(void *arg)
{
	TCP_Server *servInstance=reinterpret_cast<TCP_Server*>(arg);

	while( !(servInstance->terminateListener) )
	{
		usleep(10);
		servInstance->testfds=servInstance->readfds;
		struct timeval timeout; timeout.tv_sec=1; timeout.tv_usec=500000;
		int result=select(FD_SETSIZE,&(servInstance->testfds),(fd_set*)0,(fd_set*)0,&timeout);
		for(int fd=0; fd<FD_SETSIZE; fd++)
		{
		  if( FD_ISSET(fd,&(servInstance->testfds)) )
		  {
			if( fd== servInstance->server_sockfd )
			{
			   servInstance->add_client(fd);
			   //StartDebug
			   for(map<int,ClientInfo>::iterator it=servInstance->ClientTable.begin();
					   it!=servInstance->ClientTable.end();++it) {
			   		printf("\nClient[%d] is at fd=%d ",it->first,it->second.client_fd);
			   }
			}
		  	else
			{
		  	  int nread = 0;
			  ioctl(fd,FIONREAD,&nread);
			  if(nread==0)
			  {
				servInstance->remove_client(fd);
			  }
			  else {
				  servInstance->read_client(fd); //display_once=0;
			  }
			}
		   }
		}
	}
	if(servInstance->terminateListener)
	  cerr<<"Thread is exiting..."<<endl;
	pthread_exit(NULL);
}

void TCP_Server::CloseSocket() {
	for(map<int,ClientInfo>::iterator it=ClientTable.begin();
						   it!=ClientTable.end();++it)
		remove_client(it->first);
	close(server_sockfd);
}

void TCP_Server::read_client(int fd) {
	int len,client,i;
	memset(buf,0,MAX_BUFSIZE);
	len=read(fd,&buf,MAX_BUFSIZE);

	printf("\nrecieved... %s (%d bytes) from fd %d\n",buf,len,fd);
	for(i=0;i<len;i++)
	{
		printf("%x,",buf[i]);
	}

	check_command(fd,len,(unsigned char*)buf);
}

void TCP_Server::check_command(int fd, int len, unsigned char *buf ) {
	for(int i=0;i<len;i++)
		cout<<buf[i];
	cout<<endl;
}


