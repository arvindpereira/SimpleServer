/*
 * VerosimAUVInterface.cpp
 *
 *  Created on: Jun 21, 2013
 *      Author: arvind
 */

#include <iostream>
#include <signal.h>
#include "Server.h"


using std::cerr;
using std::cout;
using std::endl;

TCP_Server serv;
struct sigaction sa;

void quitApp(int signum) {
	cerr<<"Quitting Application.";
	serv.StopServer();
	sleep(1);
	exit(0);
}

void captureQuitSignal()
{
	memset(&sa, 0, sizeof(sa));
	sa. sa_handler = &quitApp;
	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGTERM, &sa, NULL);
	sigaction(SIGKILL, &sa, NULL);
}

int main()
{
	captureQuitSignal();
	int i=0;
	while(1) {
		sleep(1);
		cout<<i++<<endl;
		serv.send_frame(4,'A','B',(unsigned char*)"Testing...",20);
	}
}

