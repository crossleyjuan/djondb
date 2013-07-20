// =====================================================================================
// 
//  @file:  networkserver.cpp
// 
//  @brief:  This implements a network listener
// 
//  @version:  1.0
//  @date:     06/13/2013 01:09:39 PM
//  Compiler:  g++
// 
//  @author:  Juan Pablo Crossley (Cross), crossleyjuan@gmail.com
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
// =====================================================================================
#include "networkserver.h"

#include "networkinputstream.h"
#include "networkoutputstream.h"
#include "util.h"

// Windows does not have this definition
#ifdef WINDOWS
#define socklen_t int
#endif

void* startListen(void* arg);

NetworkServer::NetworkServer(int port) {
	_callback = NULL;
	_running = false;
	_port = port;
	_processing = 0;
	initialize();
}

NetworkServer::~NetworkServer() {
	cleanup();
}

void NetworkServer::listen(void* objCallback, int (*callback)(void* objCallback, NetworkInputStream* const input, NetworkOutputStream* const output)) {
	_callback = callback;
	_objectCallback = objCallback;

	_running = true;
	_listenerThread = new Thread(startListen);
	_listenerThread->start(this);
}

void NetworkServer::stop() {
	_running = false;
	_listenerThread->join();
}

void NetworkServer::initialize() {
}

bool NetworkServer::running() const {
	return _running;
}

int NetworkServer::port() const {
	return _port;
}

void NetworkServer::cleanup() {
	for (std::map<int, NetworkInputStream*>::iterator i = _mapInputs.begin(); i != _mapInputs.end(); i++) {
		int sock = i->first;
		NetworkInputStream* nis = i->second;
		if (nis->available() > 0) {
			if (_callback(_objectCallback, i->second, _mapOutputs.find(sock)->second) < 0) {
				std::map<int, NetworkOutputStream*>::iterator itNos = _mapOutputs.find(sock);
				std::map<int, NetworkInputStream*>::iterator itNis = _mapInputs.find(sock);
				NetworkOutputStream* nos = itNos->second;
				delete nos;
				delete nis;
				_mapInputs.erase(itNis);
				_mapOutputs.erase(itNos);
			}
		}
	}
	_mapInputs.clear();
	_mapOutputs.clear();
}

void* startListen(void* arg) {
	NetworkServer* server = (NetworkServer*)arg;

	server->process();

	return NULL;
}

void NetworkServer::process() {
#ifdef WINDOWS
	WORD wVersionRequested;
	WSADATA wsaData;
	int err; 
	/* Use the MAKEWORD(lowbyte, highbyte) macro declared in Windef.h */
	wVersionRequested = MAKEWORD(2, 2);

	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) {
		/* Tell the user that we could not find a usable */
		/* Winsock DLL.                                  */
		printf("WSAStartup failed with error: %d\n", err);
		return;
	}
#endif

	int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	Logger* log = getLogger(NULL);

	if (sock < 0) {
		log->error(std::string("Error creating the socked"));
	}

	fd_set master;    // master file descriptor list
	fd_set read_fds;  // temp file descriptor list for select()
	FD_ZERO(&master);    // clear the master and temp sets
	FD_ZERO(&read_fds);

	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(_port); // the port should be converted to network byte order
	addr.sin_addr.s_addr = INADDR_ANY; // Server address, any to take the current ip address of the host
	int reuse = 1;
	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *) & reuse, sizeof (reuse)) < 0) {
		log->error(std::string("Setting SO_REUSEADDR error"));
	}
	if (::bind(sock, (sockaddr *) &addr, sizeof (addr)) < 0) {
		log->error(std::string("Error binding"));
	}
	::listen(sock, 5);
	if (log->isInfo()) log->info("Accepting new connections");

	// Sets the nonblocking option for this socket
	int flags;

	/* If they have O_NONBLOCK, use the Posix way to do it */
#if defined(O_NONBLOCK)
	/* Fixme: O_NONBLOCK is defined but broken on SunOS 4.1.x and AIX 3.2.5. */
	if (-1 == (flags = fcntl(sock, F_GETFL, 0)))
		flags = 0;
	fcntl(sock, F_SETFL, flags | O_NONBLOCK);
#else
#ifndef WINDOWS
	/* Otherwise, use the old way of doing it */
	flags = 1;
	ioctl(sock, FIOBIO, &flags);
#else
	u_long f = 1;
	ioctlsocket(sock, FIONBIO, &f);
#endif
#endif

	int fdmax = sock;
	FD_SET(sock, &master); // add to master set

	while (running()) {
		fd_set read;
		FD_ZERO(&read);
		FD_SET(sock, &read);
		timeval val;
		val.tv_sec = 1;
		val.tv_usec = 0;
		read_fds = master;
		int newsocket = select(fdmax + 1, &read_fds, NULL, NULL, &val);
		for (int i = 0; i <= fdmax; i++) {
			if (FD_ISSET(i, &read_fds)) {
				// the max is the listener
				if (i == sock) {
					sockaddr_in cliaddr;
					socklen_t clilen = sizeof (cliaddr);
					int newsocket = accept(sock, (sockaddr *) & cliaddr, &clilen);

					if (newsocket == -1) {
						log->debug("Error accepting");
					} else {
						FD_SET(newsocket, &master); // add to master set
						if (newsocket > fdmax) {    // keep track of the max
							fdmax = newsocket;
						}
						if (log->isDebug()) log->debug(1, "Accepted socket %d", newsocket);

					}
				} else {
					_processing++;
					if (createStreams(i) < 0)  {
						FD_CLR(i, &master);
						if (i == fdmax) {
							fdmax--;
						}
					} else {
						std::map<int, NetworkInputStream*>::iterator itNis = _mapInputs.find(i);
						std::map<int, NetworkOutputStream*>::iterator itNos = _mapOutputs.find(i);
						NetworkInputStream* nis = itNis->second;
						NetworkOutputStream* nos = itNos->second;
						_callback(_objectCallback, nis, nos);
					}
					_processing--;
				}
			}
			// check if there's something pending on any socket
			for (std::map<int, NetworkInputStream*>::iterator i = _mapInputs.begin(); i != _mapInputs.end(); i++) {
				int sock = i->first;
				NetworkInputStream* nis = i->second;
				if (nis->available() > 0) {
					if (createStreams(sock) < 0)  {
						FD_CLR(sock, &master);
						if (sock == fdmax) {
							fdmax--;
						}
					} else {
						std::map<int, NetworkInputStream*>::iterator itNis = _mapInputs.find(sock);
						std::map<int, NetworkOutputStream*>::iterator itNos = _mapOutputs.find(sock);
						NetworkInputStream* nis = itNis->second;
						NetworkOutputStream* nos = itNos->second;
						_callback(_objectCallback, nis, nos);
					}
				}
			}
		}
	}

	shutdown(sock, 2);
	cleanup();
	log->debug("Ending network server");

#ifdef WINDOWS
	WSACleanup();
#endif
	//    pthread_exit(arg);
	for (std::map<int, NetworkInputStream*>::iterator i = _mapInputs.begin(); i != _mapInputs.end(); i++) {
		NetworkInputStream* nis = i->second;
		delete nis;
	}
	_mapInputs.clear();
	for (std::map<int, NetworkOutputStream*>::iterator i = _mapOutputs.begin(); i != _mapOutputs.end(); i++) {

		NetworkOutputStream* nos = i->second;
		delete nos;
	}
	_mapOutputs.clear();
}

int NetworkServer::createStreams(int sock) {
	NetworkInputStream* nis = NULL;
	NetworkOutputStream* nos = NULL;
	std::map<int, NetworkInputStream*>::iterator itNis = _mapInputs.find(sock);
	std::map<int, NetworkOutputStream*>::iterator itNos = _mapOutputs.find(sock);
	Logger* log = getLogger(NULL);
	if (_mapInputs.find(sock) == _mapInputs.end()) {
		nis = new NetworkInputStream(sock);
		nos = new NetworkOutputStream(sock);
		nos->setNonblocking();
		nos->disableNagle();
		nis->setNonblocking();
		_mapInputs.insert(pair<int, NetworkInputStream*>(sock, nis));
		_mapOutputs.insert(pair<int, NetworkOutputStream*>(sock, nos));
		itNis = _mapInputs.find(sock);
		itNos = _mapOutputs.find(sock);
	} else {
		nis = itNis->second;
		nos = itNos->second;
	}

	if (log->isDebug()) log->debug("Receiving request");

	if (nis->isClosed()) {
		if (log->isDebug()) log->debug("The connection was closed and nothing is available to be processed");
		_mapInputs.erase(itNis);
		_mapOutputs.erase(itNos);
		delete nis;
		delete nos;
		return -1;
	}

	return 0;
}
