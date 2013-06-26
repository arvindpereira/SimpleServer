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

	double x=10,y=10,z = 60;
	int tSwitch = 20; // when the switch to the spiral starts...
	double psi = 0.0;
	double psi_change_factor =0.05, z_change_factor = 0.1;
	double vel = 1.0;

	string receiveStr="";
	while(1) {
		float rand1 = rand()%100/100.0, rand2=rand()%180/180.0*M_PI;
		sleep(5);
		cout<<i++<<endl;
		if( i> tSwitch ) {
			/*psi = psi+ psi_change_factor;
			if( psi > M_PI ) {
				psi = -M_PI;
			} else if ( psi< -M_PI ) {
				psi = M_PI;
			}*/
			x=rand1*100; y = rand1*100;
			z = z - z_change_factor;
			if( !(z_change_factor < 2 || z_change_factor>60 ) ) {
				z_change_factor*=-1;
			}
		}
		cout<<"Path Segment Command: Vel="<<vel<<" m/s, x="<<x<<", y="<<y<<", z="<<z<<endl;
		//cout<<"Autopilot command: Vel="<<vel<<" m/s, Psi="<<psi*180/M_PI<<" deg, Depth="<<z<<" m."<<endl;
		//VerosimAUVInterfaceModuleAutopilotCommand auvAutoPilot(1+rand1,0+rand2*0.05,60);
		VerosimAUVInterfaceModulePathSegmentCharacterization auvPathSegment(vel,x,y,z);
		//VerosimAUVInterfaceModuleAutopilotCommand auvAutoPilot(vel,psi,z);

		//serv.send_frame(client_to_talk_to,(const char*)"Testing...",
		//		strlen("Testing..."));
		string auvPathSegmentStr = auvPathSegment.toString();
		//string autoPilotStr = auvAutoPilot.toString();
		serv.send_frame(client_to_talk_to, (char*)auvPathSegmentStr.c_str(), auvPathSegmentStr.length() );
		//serv.send_frame(client_to_talk_to, (char*)autoPilotStr.c_str(),
		//		autoPilotStr.length() );
	}
}

