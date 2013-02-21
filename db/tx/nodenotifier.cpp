/*
 * =====================================================================================
 *
 *       Filename:  nodenotifier.cpp
 *
 *    Description:  Implementation of the NodeNotifier, this will send and sync other log nodes
 *
 *        Version:  1.0
 *        Created:  12/05/2012 10:07:24 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Juan Pablo Crossley (Cross), crossleyjuan@gmail.com
 *   Organization:  djondb
 *
 * This file is part of the djondb project, for license information please refer to the LICENSE file,
 * the application and libraries are provided as-is and free of use under the terms explained in the file LICENSE
 * Its authors create this application in order to make the world a better place to live, but you should use it on
 * your own risks.
 * 
 * Also, be adviced that, the GPL license force the committers to ensure this application will be free of use, thus
 * if you do any modification you will be required to provide it for free unless you use it for personal use (you may 
 * charge yourself if you want), bare in mind that you will be required to provide a copy of the license terms that ensures
 * this program will be open sourced and all its derivated work will be too.
 * =====================================================================================
 */
#include "nodenotifier.h"

#include "transactiondefs.h"
#include "basetransaction.h"
#include "bsonoutputstream.h"

NodeNotifier::NodeNotifier() {
	_thread = new Thread(&NodeNotifier::sprocess);
	_lock = new Lock();
	_state = 0;
	_thread->start(this);
}

NodeNotifier::~NodeNotifier() {
	delete _thread;
	delete _lock;
}

void NodeNotifier::start() {
	_state = 1;
}

void NodeNotifier::stop() {
	// Sends notify to process the current elements
	_state = 2;
	_lock->notify();
}

void NodeNotifier::sendNotification(TransactionOperation* oper) {
	_operations.push(oper);
	_lock->notify();
}

void NodeNotifier::addNode(std::string host, int port) {
	NetworkOutputStream* nos = new NetworkOutputStream();
	nos->open(host.c_str(), port);

	_nodes.push_back(nos);
}

void NodeNotifier::sendTransactionOperation(TransactionOperation* operation) {
	for (std::list<NetworkOutputStream*>::iterator iter = _nodes.begin(); iter != _nodes.end(); iter++) {
		MemoryStream* ms = new MemoryStream();

		NetworkOutputStream* nos = *iter;
		ms->writeInt(operation->code);
		ms->writeInt(operation->status);
		ms->writeString(operation->db);
		ms->writeString(operation->ns);
		switch (operation->code) {
			case TXO_INSERT: 
			case TXO_UPDATE: {
									  BsonOper* oper = (BsonOper*)operation->operation;
									  BSONOutputStream* bos = new BSONOutputStream(ms);
									  bos->writeBSON(*oper->bson);
									  break;
								  };
			case TXO_REMOVE: {
									  RemoveOper* oper = (RemoveOper*)operation->operation;
									  ms->writeString(oper->key);
									  ms->writeString(oper->revision);
									  break;
								  };
			case TXO_DROPNAMESPACE: {
												break;
											};
		}

		nos->writeString(ms->toChars());
	}
}

void* NodeNotifier::sprocess(void* arg) {
	NodeNotifier* pthis = (NodeNotifier*)arg;
	pthis->process();
}

void NodeNotifier::process() {
	_lock->lock();

	while (_state > 0) {
		while (!_operations.empty()) {
			// Send
			TransactionOperation* oper = _operations.front();
			_operations.pop();
			sendTransactionOperation(oper);
		}
		if (_state == 1) {
			// Pass to stop state
			_state = 0;
		} else {
			_lock->wait();
		}
	}

	_lock->unlock();
}
