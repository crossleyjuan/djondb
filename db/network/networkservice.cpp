// *********************************************************************************************************************
// file:
// author: Juan Pablo Crossley (crossleyjuan@gmail.com)
// created:
// updated:
// license:
// // This file is part of the djondb project, for license information please refer to the LICENSE file,
// the application and libraries are provided as-is and free of use under the terms explained in the file LICENSE
// Its authors create this application in order to make the world a better place to live, but you should use it on
// your own risks.
// 
// Also, be adviced that, the GPL license force the committers to ensure this application will be free of use, thus
// if you do any modification you will be required to provide it for free unless you use it for personal use (you may 
// charge yourself if you want), bare in mind that you will be required to provide a copy of the license terms that ensures
// this program will be open sourced and all its derivated work will be too.
// *********************************************************************************************************************

#include <string.h>
#include <iostream>
#include <sstream>
#include <map>

#include "util.h"
#include "network.h"
#include "networkservice.h"
#include "networkinputstream.h"
#include "networkoutputstream.h"
#include "command.h"
#include "commandreader.h"
#include "dbcontroller.h"
#include "basetransaction.h"
#include "transactionmanager.h"
#include "bson.h"
#include "networkserver.h"
#include <stdlib.h>
#include <boost/shared_ptr.hpp>
#include <memory>
#ifndef WINDOWS
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#else
#include <winsock.h>
#endif
//#include <sys/wait.h>

#include "defs.h"
//#include "dbjaguar.h"

// Windows does not have this definition
#ifdef WINDOWS
#define socklen_t int
#endif

int __networkservice_port = SERVER_PORT;

long processing;
// id listening socket
DBController* __dbController;
BaseTransaction* _baseTransaction;
TransactionManager* _transactionManager;

NetworkService::NetworkService() {
	_log = getLogger(NULL);
	_running = false;
	_ntserver = new NetworkServer(__networkservice_port);
}

NetworkService::~NetworkService() {
	_ntserver->stop();
	delete _ntserver;
}

void NetworkService::start() { //throw (NetworkException*) {
	if (running()) {
		throw new NetworkException(new string("The network service is already active. Try stopping it first"));
	}
	std::string serverPort = getSetting("SERVER_PORT");
	if (serverPort.length() > 0) {
		__networkservice_port = atoi(serverPort.c_str());
	}
	if (_log->isInfo()) _log->info("Starting network service. port: %d", __networkservice_port);

	__dbController = new DBController();
	__dbController->initialize();
	_baseTransaction = new BaseTransaction(__dbController);
	TransactionManager::initializeTransactionManager(_baseTransaction);
	_transactionManager = TransactionManager::getTransactionManager();
	setRunning(true);
	_ntserver->listen(this, &callbackListen);
}

void NetworkService::stop() { //throw (NetworkException*) {
	if (_log->isInfo()) _log->info("Shutting down the network service");
	if (!_running) {
		throw new NetworkException(new string("The network service is not running. Try starting it first"));
	}
	if (processing > 0) {
		cout << "Stop requested but still working" << endl;
	}
	int i = 0;
	while (processing > 0) {
		Thread::sleep(1000);
		i++;
		// if the time exceeded then shutdown anyway
		if (i > 10) {
			break;
		}
	}
	delete _transactionManager;
	delete _baseTransaction;
	__dbController->shutdown();
	setRunning(false);
	_ntserver->stop();

	delete __dbController;
}

bool NetworkService::running() const {
	return _running;
}

void NetworkService::setRunning(bool running) {
	_running = running;
}

int __status;

int NetworkService::callbackListen(void* service, NetworkInputStream* const nis, NetworkOutputStream* const nos) {
	NetworkService* instance = (NetworkService*)service;
	instance->executeRequest(nis, nos);
}

int NetworkService::executeRequest(NetworkInputStream* nis, NetworkOutputStream* nos) {
	Logger* log = getLogger(NULL);

	if (log->isDebug()) log->debug("Receiving request");

	// Checks version
	int commands = 0;
	std::auto_ptr<CommandReader> reader(new CommandReader(nis));

	// Reads command
	Command* cmd = reader->readCommand();
	commands++;
	BaseTransaction* transaction = _baseTransaction;
	if (cmd->options() != NULL) {
		if (cmd->options()->has("_transactionId")) {
			transaction = (BaseTransaction*)_transactionManager->getTransaction(cmd->options()->getString("_transactionId"));
		}
	}
	cmd->setDBController(transaction);
	try {
		if (cmd->commandType() != CLOSECONNECTION) {
			cmd->execute();
			cmd->writeResult(nos);
			nos->flush();
		} else {
			if (log->isDebug()) log->debug("Close command received");
		}
		if (cmd->commandType() == SHUTDOWN) {
			// Shutting down the server
			long l = 0;
			// the current request should be the only one alive
			while (processing > 1) {
				Thread::sleep(1000);
				l++;
				// if the time exceeded then shutdown anyway
				if (l > 10) {
					break;
				}
			}
			// Hack to avoid the count of the stop process
			processing--;
			stop();
			processing++;
		}
	} catch (const char *errmsg)
	{
		std::cerr << "error message from connection: " << errmsg << "\n";
		return 1;
	}

	if (log->isDebug()) log->debug("%d Executed.", commands);

	delete cmd;
}


