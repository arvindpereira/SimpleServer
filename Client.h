#ifndef __SIMPLE_CLIENT_H__
#define __SIMPLE_CLIENT_H__

#ifdef _WINDOWS_
	#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN
	#endif
	#include <Windows.h>
	#include <winsock2.h>
	#include <ws2tcpip.h>
	#include <iphlpapi.h>
	// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
	#pragma comment (lib, "Ws2_32.lib")
	#pragma comment (lib, "Mswsock.lib")
	#pragma comment (lib, "AdvApi32.lib")
#else  // This is probably being compiled for Linux or OS-X
	#include <unistd.h>
	#include <cerrno>
	#include <string.h>
	#include <netdb.h>
	#include <sys/types.h>
	#include <netinet/in.h>
	#include <sys/socket.h>
	#include <arpa/inet.h>
	#include <pthread.h>
	#include <cstdio>
	#include <fcntl.h> // non blocking socket
#endif
// Cross platform headers...
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>


using std::cout;
using std::cerr;
using std::endl;
using std::string;

// size of our buffer
#define MAX_CLIENT_BUFSIZE 4096
// port to connect sockets through
#define DEFAULT_PORT "10000"


class TCP_Client {
#ifdef _WINDOWS_
	WSADATA wsaData; // Windows Socket data object
	struct addrinfo *result, *ptr, hints;
	int iResult;
	SOCKET ClientSocket;

public:
	TCP_Client() {
		// Initialize Winsock
		iResult = WSAStartup( MAKEWORD(2,2), &wsaData ); // initiate use of WS2_32.dll
		if( iResult != 0 ) {
			std::cerr<<"WSA Startup failed "<<iResult<<std::endl;
			exit(EXIT_FAILURE);
		}
	}

	bool CreateSocket( string ip_addr, string port  ) {
		ZeroMemory( &hints, sizeof(hints) );
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;

		// Resolve the server address and port
		iResult = getaddrinfo(ip_addr.c_str(), DEFAULT_PORT, &hints, &result);
		if (iResult != 0) {
		    printf("getaddrinfo failed: %d\n", iResult);
		    WSACleanup();
		    return false;
		}
		ClientSocket = INVALID_SOCKET;
		// Attempt to connect to the first address returned by
		// the call to getaddrinfo
		ptr=result;

		// Create a SOCKET for connecting to server
		ClientSocket = socket(ptr->ai_family, ptr->ai_socktype,
		    ptr->ai_protocol);
		if (ClientSocket == INVALID_SOCKET) {
		    printf("Error at socket(): %ld\n", WSAGetLastError());
		    freeaddrinfo(result);
		    WSACleanup();
		    return false;
		}

		// Connect to server.
		iResult = connect( ClientSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
		    closesocket(ClientSocket);
		    ClientSocket = INVALID_SOCKET;
		}

		// Should really try the next address returned by getaddrinfo
		// if the connect call failed
		freeaddrinfo(result);

		if (ClientSocket == INVALID_SOCKET) {
		    printf("Unable to connect to server!\n");
		    WSACleanup();
		    return false;
		}

		// Great, we're done opening a socket to the server!
		return true;
	}

	int send_frame( string data ){
		// Send an initial buffer
		iResult = send(ClientSocket, data.c_str(), data.length(), 0);
		if (iResult == SOCKET_ERROR) {
		    printf("send failed: %d\n", WSAGetLastError());
		    closesocket(ClientSocket);
		    WSACleanup();
		    return -1;
		}

		printf("Bytes Sent: %ld\n", iResult);
		return iResult;
	}

	int receive( string &receivedData ) {
		int recvbuflen = MAX_CLIENT_BUF_SIZE-1;
		char recvbuf[MAX_CLIENT_BUFSIZE];


		iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0)
		        cout<<"Bytes received: "<<iResult<<'\n';
		else if (iResult == 0)
		        cout<<"Connection Closed\n";
		else
		        cout<<"recv failed: "<<WSAGetLastError()<<'\n';
	}

	int CloseSocket() {
		// shutdown the send half of the connection since no more data will be sent
		iResult = shutdown(ConnectSocket, SD_SEND);
		if (iResult == SOCKET_ERROR) {
		    printf("shutdown failed: %d\n", WSAGetLastError());
		    closesocket(ConnectSocket);
		    WSACleanup();
		    return 1;
		}
		// cleanup
		closesocket(ConnectSocket);
		WSACleanup();

		return 0;
	}


#else // Linux and OS-X (taken straight out of beej.us)
	int sockfd, numbytes;
	char buf[MAX_CLIENT_BUFSIZE];
	struct addrinfo hints, *servinfo, *p;
	int rv;
	char s[INET6_ADDRSTRLEN];
private:
	// get sockaddr, IPv4 or IPv6:
	void *get_in_addr(struct sockaddr *sa)
	{
	    if (sa->sa_family == AF_INET) {
	        return &(((struct sockaddr_in*)sa)->sin_addr);
	    }

	    return &(((struct sockaddr_in6*)sa)->sin6_addr);
	}

public:
	TCP_Client() {
	    memset(&hints, 0, sizeof hints);
	    hints.ai_family = AF_UNSPEC;
	    hints.ai_socktype = SOCK_STREAM;

	}

	bool CreateSocket( string ip_addr, string port ) {
	    if ((rv = getaddrinfo((char*)ip_addr.c_str(), (char*)port.c_str(), &hints, &servinfo)) != 0) {
	        cerr<<"getaddrinfo: "<<gai_strerror(rv)<<endl;
	        return false;
	    }
	    // loop through all the results and connect to the first we can
		for(p = servinfo; p != NULL; p = p->ai_next) {
			if ((sockfd = socket(p->ai_family, p->ai_socktype,
					p->ai_protocol)) == -1) {
				perror("client: socket");
				continue;
			}

			if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
				close(sockfd);
				perror("client: connect");
				continue;
			}

			break;
		}

		if ( p==NULL ) {
			cerr<<"client: failed to connect\n";
			return false;
		}
		inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
		            s, sizeof s);
		cout<<"client: connecting to "<< s<<endl;
		freeaddrinfo(servinfo); // all done with this structure
	}

	int receive( string &rx_data ) {
		if ((numbytes = recv(sockfd, buf, MAX_CLIENT_BUFSIZE-1, 0)) == -1) {
		        perror("recv");
		        exit(1);
		}
		rx_data=string(buf);
	}

	int send_frame( string tx_data ) {
		if ((numbytes = send(sockfd, (char*)tx_data.c_str(), tx_data.length(), 0)) == -1) {
			perror("send");
			exit(1);
		}
	}


#endif


};






#endif /* __SIMPLE_CLIENT_H__ */
