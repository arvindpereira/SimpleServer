/*
 * VerosimAUVInterface.cpp
 *
 *  Created on: Jun 21, 2013
 *      Author: arvind
 */

#include <iostream>
#include <signal.h>
#include <cstring>
#include <cstdlib>
#include <time.h>
#include <cmath>
#include "Server.h"
#include "Client.h"
#include "VerosimAUVCommProt.h"

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

using namespace VerosimAUVInterfaceModule;
int main()
{
	//VerosimAUVInterfaceModuleObject auvIM(
	//		VerosimAUVInterfaceModuleObject::AutopilotCommand);
	VerosimAUVInterfaceModuleAutopilotCommand auvAutoPilot(1,0,5);

	srand(time(NULL));
	captureQuitSignal();
	int i=0;
	int client_to_talk_to = 4;

	string receiveStr="";
	while(1) {
		float rand1 = rand()%100/100.0, rand2=rand()%180/180.0*M_PI;
		sleep(1);
		cout<<i++<<endl;
		VerosimAUVInterfaceModuleAutopilotCommand auvAutoPilot(1+rand1,0+rand2,10);
		//serv.send_frame(client_to_talk_to,(const char*)"Testing...",
		//		strlen("Testing..."));
		string autoPilotStr = auvAutoPilot.toString();
		serv.send_frame(client_to_talk_to, (char*)autoPilotStr.c_str(),
				autoPilotStr.length() );
	}
}

