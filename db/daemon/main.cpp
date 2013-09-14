// *********************************************************************************************************************
// file:
// author: Juan Pablo Crossley (crossleyjuan@gmail.com)
// created:
// updated:
// license:
// 
// This file is part of the djondb project, for license information please refer to the LICENSE file,
// the application and libraries are provided as-is and free of use under the terms explained in the file LICENSE
// Its authors create this application in order to make the world a better place to live, but you should use it on
// your own risks.
// 
// Also, be adviced that, the GPL license force the committers to ensure this application will be free of use, thus
// if you do any modification you will be required to provide it for free unless you use it for personal use (you may 
// charge yourself if you want), bare in mind that you will be required to provide a copy of the license terms that ensures
// this program will be open sourced and all its derivated work will be too.
// *********************************************************************************************************************

/*
 *  * Example daemon shell code for all of the requirements of a basic
 *   * linux daemon written in C.
 *    *
 *     * To use this code, search for 'TODO' and follow the directions.
 *
 *       * To compile this file:
 *        *      gcc -o [daemonname] thisfile.c
 *         *
 *          * Substitute gcc with cc on some platforms.
 *           *
 *            * Peter Lombardo (peter AT lombardo DOT info)
 *             * 5/1/2006
 *              *
 *               */

#include "service.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include "util.h"
#ifdef _WIN32
	#define WINDOWS
#else
	#include "config.h"
#endif
#ifndef WINDOWS
	#include <unistd.h>
	#include <syslog.h>
#endif

#define DAEMON_NAME "djondbd"
#define PID_FILE "/var/run/djondbd.pid"

bool __stopRunning;

/**************************************************************************
  Function: Print Usage

  Description:
  Output the command-line options for this daemon.

  Params:
  @argc - Standard argument count
  @argv - Standard argument array

  Returns:
  returns void always
 **************************************************************************/
void PrintUsage(int argc, char *argv[]) {
	if (argc >=1) {
		printf("Usage: %s [-h] [-n] [-p PORT] [-d directory]\n", argv[0]);
		printf("  Options:\n");
		printf("      -n\t\tDon't fork off as a daemon.\n");
		printf("      -h\t\tShow this help screen.\n");
		printf("      -p port\t\tChange the default port (1243).\n");
		printf("      -d directory\tUse this folder as default data dir.");
		printf("\n");
	}
}

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
void signal_handler(int sig) {

	switch(sig) {
		case SIGHUP:
			syslog(LOG_WARNING, "Received SIGHUP signal.");
			syslog(LOG_INFO, "%s daemon exiting", DAEMON_NAME);
			service_shutdown();
			__stopRunning = true;

			exit(0);
			break;
		case SIGTERM:
        case SIGINT:
        case SIGKILL:
        case SIGSTOP:
			syslog(LOG_WARNING, "Received SIGTERM | SIGINT signal.");
			service_shutdown();
			__stopRunning = true;

			exit(0);
			break;

		default:
			syslog(LOG_WARNING, "Unhandled signal %s", strsignal(sig));
			break;
	}
}

/**************************************************************************
Function: main

Description:
The c standard 'main' entry point function.

Params:
@argc - count of command line arguments given on command line
@argv - array of arguments given on command line

Returns:
returns integer which is passed back to the parent process
 **************************************************************************/
int main(int argc, char *argv[]) {

	Logger* log = getLogger(NULL);

	__stopRunning = false;
#if defined(DEBUG)
	int daemonize = 0;
#else
	int daemonize = 1;
#endif

	// Setup signal handling before we start
	signal(SIGHUP, signal_handler);
	signal(SIGTERM, signal_handler);
	signal(SIGINT, signal_handler);
	signal(SIGKILL, signal_handler);
	signal(SIGSTOP, signal_handler);
	signal(SIGQUIT, signal_handler);

	int c;
	while( (c = getopt(argc, argv, "nhp:d:|help")) != -1) {
		switch(c){
			case 'h':
				PrintUsage(argc, argv);
				exit(0);
				break;
			case 'n':
				daemonize = 0;
				break;
			case 'd': {
							 char* datadir = optarg;
							 setSetting("DATA_DIR", datadir);
							 break;
						 }
			case 'p': {
							 char* port = optarg;
							 setSetting("SERVER_PORT", port);
							 break;
						 }
			case '?':
						 if (optopt == 'c') {
							 PrintUsage(argc, argv);
							 exit(0);
							 break;
						 }

			default:
						 PrintUsage(argc, argv);
						 exit(0);
						 break;
		}
	}

	log->info("djondbd version %s is starting up.", VERSION);

	// Setup syslog logging - see SETLOGMASK(3)
#if defined(DEBUG)
	setlogmask(LOG_UPTO(LOG_DEBUG));
	openlog(DAEMON_NAME, LOG_CONS | LOG_NDELAY | LOG_PERROR | LOG_PID, LOG_USER);
#else
	setlogmask(LOG_UPTO(LOG_INFO));
	openlog(DAEMON_NAME, LOG_CONS, LOG_USER);
#endif

	/* Our process ID and Session ID */
	pid_t pid, sid;

	if (daemonize) {
		setDaemon(true);
		syslog(LOG_INFO, "starting the daemonizing process");

		/* Fork off the parent process */
		pid = fork();
		if (pid < 0) {
			exit(EXIT_FAILURE);
		}
		/* If we got a good PID, then
			we can exit the parent process. */
		if (pid > 0) {
			exit(EXIT_SUCCESS);
		}

		/* Change the file mode mask */
		umask(0);

		/* Create a new SID for the child process */
		sid = setsid();
		if (sid < 0) {
			/* Log the failure */
			exit(EXIT_FAILURE);
		}

		/* Change the current working directory */
		if ((chdir("/")) < 0) {
			/* Log the failure */
			exit(EXIT_FAILURE);
		}

		/* Close out the standard file descriptors */
		close(STDIN_FILENO);
		close(STDOUT_FILENO);
		close(STDERR_FILENO);
	}
	setDaemon(false);

	service_startup();

	while(true) {
		if (__stopRunning) {
			break;
		}
		sleep(1);

		if (!service_running()) {
			syslog(LOG_INFO, "service down");
			exit(0);
			break;
		}
	}

	delete log;
}
