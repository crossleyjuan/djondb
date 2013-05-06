// djondb_win.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "service.h"
#include "util.h"
#include <windows.h>

/**************************************************************************
Function: signal_handler

Description:
This function handles select signals that the daemon may
receive.  This gives the daemon a chance to properly shut
down in emergency situations.  This function is installed
as a signal handler in the 'main()' function.

Params:
@sig - The signal received

Returns:
returns void always
 **************************************************************************/
BOOL WINAPI signal_handler(DWORD dwCtrlType) {
	BOOL WINAPI result;
	switch	(dwCtrlType) {
		case CTRL_C_EVENT:
		case CTRL_BREAK_EVENT:
		case CTRL_CLOSE_EVENT:
			service_shutdown();
			result = TRUE;
			break;
		default:
			result = FALSE;
			break;
	}
	return result;
}

int _tmain(int argc, _TCHAR* argv[])
{
	Logger* log = getLogger(NULL);
	log->info("djondbd version %s is starting up.", VERSION);
	service_startup();

	/* Auto-reset, initially non-signaled event */
    ::CreateEvent(NULL, FALSE, FALSE, NULL);

    /* Add the break handler */
    ::SetConsoleCtrlHandler(signal_handler, TRUE);

	while(true) {
		if (!service_running()) {
			break;
		}
		Thread::sleep(5000);
	}
	return 0;
}

