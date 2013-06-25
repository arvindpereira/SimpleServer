/*
 * VerosimAUVInterface.cpp
 *
 *  Created on: Jun 21, 2013
 *      Author: arvind
 */

#include <iostream>
#include <signal.h>
#include <cstring>
#include "Server.h"
#include "Client.h"

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
	int client_to_talk_to = 4;

	string receiveStr="";
	while(1) {
		sleep(1);
		cout<<i++<<endl;

		serv.send_frame(client_to_talk_to,(const char*)"Testing...",
				strlen("Testing..."));
	}
}

