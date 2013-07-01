#include <iostream>
#include <stdexcept>
#include <sstream>
#include <iomanip>

#include "SignalTools.h"
#include "TimeTools.h"
#include "Client.h"

using namespace std;
using namespace ArvindsTools;

TCP_Client client;

/** Function that gracefully exits even if the program is quit using
 * Ctrl-C
 * @param signum the signal received. I'm not doing anything with this here, because
 * the assumption is that we want to quit anyway.
 */
void quitApp(int signum) {
	cerr<<"Quitting Application.";
	client.CloseSocket();
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

int main(int argc, char *argv[]) {
	SetupMyselfForExit();
	TimeTools myTimer;

	string s = argv[0];
	if (argc>1) s = argv[1];
	client.CreateSocket( s,"10000");

	ostringstream sout;
	while( true ) {
		usleep(100000);
		sout<<'\n'<< s <<"  "<<std::fixed<<setprecision(6)<<myTimer.timeSinceStart()<<"\n"; sout.flush();
		client.send_frame( sout.str() );
	}

	return 0;
}
