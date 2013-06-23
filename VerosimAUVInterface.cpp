/*
 * VerosimAUVInterface.cpp
 *
 *  Created on: Jun 21, 2013
 *      Author: arvind
 */

#include <iostream>
#include <signal.h>
#include <cstring>
#include <sqlite3.h>
#include "Server.h"
#include "ItemList.h"

using std::cerr;
using std::cout;
using std::endl;

TCP_Server serv;
struct sigaction sa;
sqlite3 *db;
char *zErrMsg = 0;

void quitApp(int signum) {
	cerr<<"Quitting Application.";
	serv.StopServer();
	sqlite3_close(db);
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

	int rc = sqlite3_open("Test.db", &db);
	if( rc ) {
		cerr<<"Can't open database."<<sqlite3_errmsg(db);
		sqlite3_close(db);
		return EXIT_FAILURE;
	}
	sqlite3_close(db);

	captureQuitSignal();
	int i=0;
	while(1) {
		sleep(1);
		cout<<i++<<endl;

		serv.send_frame(4,(const char*)"Testing...",strlen("Testing..."));
	}
}

