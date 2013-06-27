#include <iostream>
#include <iomanip>
#include <signal.h>
#include <cstring>
#include <cstdlib>
#include <time.h>
#include <cmath>
#include "Server.h"
#include "Client.h"
#include "VerosimAUVCommProt.h"
#include "CommandCallbackHandler.h"
#include "SignalTools.h"
#include "TimeTools.h"

#include <vector>
#include <map>

using namespace std;
using namespace ArvindsTools;
using namespace VerosimAUVInterfaceModule;

using namespace ArvindsTools;
using std::cerr;
using std::cout;
using std::endl;

// A dummy class to show how a callback could be performed
class Dummy {
public:
	static void JustPrintItOut( string s ) {
		cout<<"Dummy says packet received has :"<<s<<endl;
	}
};

typedef void (*VSMoosCallbacktoDummyFunction)(string str);

/** Demo class to show how a callback may be registered. Notice how it is static though, which
 * means the callback is **NOT** threadsafe! :(
 *
 *  Turns out that the best way to setup callbacks in C++ at the moment is to vomit an exception (Yikes!!!!)
 *  like so: (see http://stackoverflow.com/questions/12671628/passing-non-static-member-function-pointer-in-c)
 *  void f(void(*fp)()) { fp(); }
 *	void mah_func() {
 *   	try {
 *       	throw;
 *   	} catch(my_class* m) {
 *       	m->func();
 *   	}
 *	}
 *	int main() {
 *   	my_class m;
 *   	try {
 *       	throw &m;
 *   	} catch(my_class* p) {
 *       	f(mah_func);
 *   	}
 *	}
 *
 * This is another situation where I wish C++ had a better construct to achieve passing class methods as
 * callbacks. The fact that C++ does not support this is rather annoying!!!
 *
 * As an aside, one might also want to use std::bind(1st/2nd) if one wants to use object-based callbacks.
 *
 */
class VSMoosCallbacks : public CommandCallbackHandler {
	VSMoosCallbacktoDummyFunction dummyFunc;
public:
	void AddFunction( std::string _funcName, VSMoosCallbacktoDummyFunction _myFunc ) {
		funcName       = _funcName;
		dummyFunc         = _myFunc;
		funcRegistered = true;
	}

	bool CallFunction( std::string str ) {
		if( funcRegistered ) {
			dummyFunc( str );
			return true;
		}
		return false;
	}
};





/** Boiler plate code for creation of the actual connection to MOOS-IVP or
 *  ROS connections using the TCP_Server. My recommendation is to proceed
 *  as shown here. One can either use callbacks as shown below, or
 *  just use something like the switch case statement.
 */
class VerosimMoosInterface : public TCP_Server
{
	vector<VSMoosCallbacks> callbackTable;

	VerosimAUVInterfaceModuleHandler *modHandler; // used to instantiate objects

public:
	VerosimMoosInterface() : TCP_Server()
	{
		modHandler = new VerosimAUVInterfaceModuleHandler();
	}

	bool addCallback( string funcName, VSMoosCallbacktoDummyFunction func ) {
		VSMoosCallbacks newCallback;
		newCallback.AddFunction(funcName, func);
		callbackTable.push_back(newCallback);
	}

	void check_command(int fd, int len, unsigned char *buf) {
		char *sbuf = (char*)buf;
		string packetReceived = sbuf;
		VerosimAUVInterfaceModuleObject* modObj = modHandler->fromString( packetReceived );

		//cout<<"RXed"<<packetReceived<<endl;
		switch( modObj->getType() )
		{
			case VerosimAUVInterfaceModuleObject::State:
			{
				cout<<"State packet received :"<<endl;
				VerosimAUVInterfaceModuleState *state = dynamic_cast<VerosimAUVInterfaceModuleState*>(modObj);
				if( state != NULL ) {
					cout<<state->toString();
					// *** Publish the State to MOOS ***
					// sendToMoosDB( state );
				}
			}
			break;
			case VerosimAUVInterfaceModuleObject::PathSegment:
			{
				cout<<"Path segment received :"<<endl;
				VerosimAUVInterfaceModulePathSegmentCharacterization *pathSegment =
						dynamic_cast<VerosimAUVInterfaceModulePathSegmentCharacterization*>(modObj);
				if( pathSegment != NULL ) {
					cout<<pathSegment->toString();
					// Server should not necessarily receive path segments from clients, but
					// assuming this can happen, we could do something with it here.
					// Perhaps relay it Verosim (if it came from another client instead).
				}
			}
			break;
			case VerosimAUVInterfaceModuleObject::AutopilotCommand:
			{
				cout<<"AutopilotCommand received"<<endl;
				VerosimAUVInterfaceModuleAutopilotCommand * autopilotCommand =
						dynamic_cast<VerosimAUVInterfaceModuleAutopilotCommand*>(modObj);
				if( autopilotCommand != NULL ) {
					cout<<autopilotCommand->toString();
					// Server should not necessarily receive autopilot commands from clients, but
					// assuming this can happen, we could do something with it here.
					// Perhaps relay it Verosim (if it came from another client instead).
				}
			}
			break;
			default:
			{
				cout<<"Invalid/Unknown command received"<<endl;
				cout<<"Checking Callbacks...";
				for(int i=0;i<callbackTable.size();i++) {
					callbackTable[i].CallFunction( packetReceived );
				}
				break;
			}
		}
	}

} myServer ; // the server instance we are using.


/** Function that gracefully exits even if the program is quit using
 * Ctrl-C
 * @param signum the signal received. I'm not doing anything with this here, because
 * the assumption is that we want to quit anyway.
 */
void quitApp(int signum) {
	cerr<<"Quitting Application.";
	myServer.StopServer();
	SingletonSignalHandler::getInstance()->endSignalCapture();
	sleep(1);
	cout<<"\nDone cleaning up.\nGoodbye..."<<endl;
	exit(0);
}

/** Use some of my funky signal tools to ensure that the program quits gracefully when
 * Ctrl-C or Ctrl-Z are pressed.
 */
void SetupMyselfForExit() {
	SingletonSignalHandler::getInstance()->captureSignal( InterruptSignal, ::quitApp );
	SingletonSignalHandler::getInstance()->captureSignal( TerminateSignal, ::quitApp );
}

/** A simple random location generator. Should keep the AUV chasing ghosts for a long time
 *	@param x -> desired x location (passed by reference)
 *	@param y -> desired y location (passed by reference)
 *	@param z -> desired depth (passed by reference)
 *	@param t -> time since the start of the program
 *	@return string to be sent to Verosim
 *
 * */
string generateRandomPathSegment( double &x, double &y, double &z, double t )
{
	double rand1 = rand()%100/100.0, rand2=rand()%180/180.0*M_PI;
	double tSwitch = 2; //Switch to random commands after 20 seconds
	const int RandomJumpRange = 100.0;
	const double minDepth = 2.0, maxDepth = 60.0;
	double z_change_factor = 0.1;
	double vel = 1.0;
	static double lastT; double sampleTime = 20;

	if( t > tSwitch ) {
		if( t - lastT > sampleTime ) {
			lastT = t;
			x=rand1*RandomJumpRange; y = rand2*RandomJumpRange;
			z = z - z_change_factor;
			if( !(z_change_factor < minDepth || z_change_factor> maxDepth ) ) {
				z_change_factor*=-1;
			}
		}

	}
	VerosimAUVInterfaceModulePathSegmentCharacterization auvPathSegment(vel,x,y,z);
	return auvPathSegment.toString();
}

/** Helix generator - should keep the AUV winding up and winding down in the
 * vertical axis between two depths.
 * @param psi
 * @param z
 * @param t
 * @return string to be sent to Verosim.
 */
string generateHelixUsingAutopilotCommand( double &psi, double &z, double t ) {
	double psi_change_factor =0.05, z_change_factor = 0.1;
	double tSwitch = 20; //Switch to random commands after 20 seconds
	const double minDepth = 2.0, maxDepth = 60.0;
	double vel = 1.0;
	static double lastT;
	double sampleTime = 1.0;

	if( t > tSwitch ) {
		if( t - lastT > sampleTime ) {
			lastT = t;
			psi = psi+ psi_change_factor;
			if( psi > M_PI ) {
				psi = -M_PI;
			} else if ( psi< -M_PI ) {
				psi = M_PI;
			}
			z = z - z_change_factor;
			if( !(z_change_factor < 2 || z_change_factor>60 ) ) {
				z_change_factor*=-1;
			}
		}
	}

	VerosimAUVInterfaceModuleAutopilotCommand auvAutoPilotCommand(vel,psi,z);
	return auvAutoPilotCommand.toString();
}


int main() {
	// Capture quit signals
	SetupMyselfForExit();

	// Get our neat timer.
	TimeTools myTimer;

	// Initialize our randomizer
	srand(time(NULL));

	// Setup a callback function just for fun
	Dummy dumbDum;
	VSMoosCallbacktoDummyFunction fPtr = &dumbDum.JustPrintItOut;

	myServer.addCallback(string("Just a Dummy"), fPtr);

	double x=0,y=0,z=0,psi=0.0;

	// Create a table to get information about the clients that are connected.
	map<int, ClientInfo> clTable;
	map<int, ClientInfo>::iterator clt_iter;
	while ( true ) {
		usleep( 100000 ); cout.width(14);
		clTable = myServer.getClientTable();
		// Get an autopilot command
		string autoPilotCmd = generateHelixUsingAutopilotCommand(psi,z,myTimer.timeSinceStart());
		string pathSegmentCmd = generateRandomPathSegment( x,y,z,myTimer.timeSinceStart());

		for( clt_iter=clTable.begin();clt_iter!=clTable.end(); clt_iter++ ) {
			pair<int,ClientInfo> clientInfo = *clt_iter; // Could also use information from ClientInfo instead of broadcasting.
			//myServer.send_frame(clientInfo.first, autoPilotCmd.c_str(), autoPilotCmd.length());
			myServer.send_frame(clientInfo.first, pathSegmentCmd.c_str(), pathSegmentCmd.length());
		}
		cout<<std::fixed<<std::setprecision(6)<<myTimer.timeSinceStart()<<'\r'; cout.flush();
	}
}
