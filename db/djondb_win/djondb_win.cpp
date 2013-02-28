// djondb_win.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "service.h"
#include "util.h"

bool __stopRunning;

int _tmain(int argc, _TCHAR* argv[])
{
	__stopRunning = false;
	Logger* log = getLogger(NULL);
	log->info("djondbd version %s is starting up.", VERSION);
	service_startup();

	while(true) {
		if (__stopRunning) {
			break;
		}
		Thread::sleep(30000);
	}
	delete log;
	return 0;
}

