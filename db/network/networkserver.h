// =====================================================================================
// 
//  @file:  networkserver.h
// 
//  @brief:  This implements a network socket listener
// 
//  @version:  1.0
//  @date:     06/13/2013 01:01:50 PM
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
#ifndef NETWORKSERVER_INCLUDED_H
#define NETWORKSERVER_INCLUDED_H 

#include <map>
#ifndef WINDOWS
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#else
#include <winsock.h>
#endif

class NetworkOutputStream;
class NetworkInputStream;
class Thread;

class NetworkServer {
	public:
		NetworkServer(int port);
		NetworkServer(const NetworkServer& orig);
		virtual ~NetworkServer();

		void listen(void* objCallback, int (*callback)(void* objCallback, NetworkInputStream* const input, NetworkOutputStream* const output));

		void stop();
		bool running() const;
		int port() const;
		void process();
	private:
		void initialize();
		void cleanup();
		int createStreams(int sock);

	private:
		std::map<int, NetworkInputStream*>  _mapInputs;
		std::map<int, NetworkOutputStream*> _mapOutputs;

		int (*_callback)(void* objCallback, NetworkInputStream* input, NetworkOutputStream* output);
		void* _objectCallback;

		bool _running;
		int _port;
		int _processing;
		Thread* _listenerThread;
};
#endif /* NETWORKSERVER_INCLUDED_H */
