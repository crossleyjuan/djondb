#ifndef NETWORKSERVICE_H_INCLUDED
#define NETWORKSERVICE_H_INCLUDED

/*
 * File:   networkservice.h
 * Author: JuanC
 *
 * Created on November 13, 2008, 5:48 PM
 */

//#include "network.h"

#define SERVER_PORT 1243

#include "util.h"

class NetworkServer;
class NetworkInputStream;
class NetworkOutputStream;

class NetworkService {
public:
    NetworkService();
    virtual ~NetworkService();

    void start();// throw (NetworkException*);

    void stop();// throw (NetworkException*);

	 bool running() const;
	 void setRunning(bool);
	 static int callbackListen(void* service, NetworkInputStream* const nis, NetworkOutputStream* const nos);
	 int executeRequest(NetworkInputStream* nis, NetworkOutputStream* nos);
private:
	 bool _running;
	 Logger* _log;
	 NetworkServer* _ntserver;
};


#endif // NETWORKSERVICE_H_INCLUDED
