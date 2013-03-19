/*
 * =====================================================================================
 *
 *       Filename:  basetransaction.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  09/26/2012 08:26:29 PM
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
#include "basetransaction.h"
#include "controller.h"
#include "settings.h"
#include "fileinputoutputstream.h"
#include "bsonoutputstream.h"
#include "bsoninputstream.h"
#include "memorystream.h"
#include "util.h"
#include "linkedmap.hpp"
#include "filterparser.h"
#include "expressionresult.h"
#include "txbuffermanager.h"
#include "txbuffer.h"
#include "dbcontroller.h"
#include <stdlib.h>

BaseTransaction::BaseTransaction(Controller* controller) {
	_mainTransactionLog = true;
	_controller = controller;
	_transactionId = NULL;

	loadControlFile();
}

BaseTransaction::BaseTransaction(Controller* controller, std::string transactionId) {
	_mainTransactionLog = false;
	_controller = controller;
	_transactionId = new std::string(transactionId);

	loadControlFile();
}

void BaseTransaction::loadControlFile() {
	std::string file = _mainTransactionLog ? "main": *_transactionId;

	_bufferManager = new TxBufferManager(_controller, file.c_str(), _mainTransactionLog);
	if (_mainTransactionLog) {
		_bufferManager->startMonitor();
	}
}

BaseTransaction::BaseTransaction(const BaseTransaction& orig) {
	this->_controller = orig._controller;
	this->_transactionId = orig._transactionId;
	this->_bufferManager = orig._bufferManager;
}

BaseTransaction::~BaseTransaction() {
	_bufferManager->stopMonitor();
	while (_bufferManager->runningMonitor()) {
		Thread::sleep(30);
	}
	delete _bufferManager;
	if (_transactionId != NULL) delete _transactionId;
}

std::list<TransactionOperation*>* BaseTransaction::findOperations(char* db, char* ns) {
	std::list<TransactionOperation*>* result = new std::list<TransactionOperation*>();
	std::vector<TxBuffer*> buffers = _bufferManager->getActiveBuffers();
	for (std::vector<TxBuffer*>::iterator i = buffers.begin(); i != buffers.end(); i++) {
		TxBuffer* buffer = *i;
		buffer->seek(0);
		while (!buffer->eof()) {
			TransactionOperation* operation = _bufferManager->readOperationFromRegister(buffer, db, ns);
			if (operation != NULL) {
				result->push_back(operation);
			}
		}
	}
	return result;
}

BSONObj* BaseTransaction::insert(char* db, char* ns, BSONObj* bson) {
	TransactionOperation oper;
	oper.code = TXO_INSERT;
	oper.db = db;
	oper.ns = ns;
	BsonOper* insertOper = new BsonOper();
	insertOper->bson = bson; 
	oper.operation = insertOper;
	DBController::fillRequiredFields(bson);

	_bufferManager->writeOperationToRegister(db, ns, oper);

	delete insertOper;
}

bool BaseTransaction::dropNamespace(char* db, char* ns) {
	TransactionOperation oper;
	oper.code = TXO_DROPNAMESPACE;
	oper.operation = NULL;
	oper.db = NULL;
	oper.ns = NULL;

	_bufferManager->writeOperationToRegister(db, ns, oper);
}

void BaseTransaction::update(char* db, char* ns, BSONObj* bson) {
	TransactionOperation oper;
	oper.code = TXO_UPDATE;
	oper.db = db;
	oper.ns = ns;
	BsonOper* bsonOper = new BsonOper();
	bsonOper->bson = bson; 
	oper.operation = bsonOper;

	_bufferManager->writeOperationToRegister(db, ns, oper);

	delete bsonOper;
}

void BaseTransaction::remove(char* db, char* ns, char* documentId, char* revision) {
	TransactionOperation oper;
	oper.code = TXO_REMOVE;
	oper.db = db;
	oper.ns = ns;
	RemoveOper* removeOper = new RemoveOper();
	removeOper->key = documentId;
	removeOper->revision = revision;
	oper.operation = removeOper;

	_bufferManager->writeOperationToRegister(db, ns, oper);

	delete removeOper;
}

bool compareStrings(std::string s1, std::string s2) {
	return s1.compare(s2) == 0;
}

BSONArrayObj* BaseTransaction::find(char* db, char* ns, const char* select, const char* filter) throw (ParseException) {
	std::list<TransactionOperation*>* operations = findOperations(db, ns);
	LinkedMap<std::string, BSONObj*> map(compareStrings);

	BSONArrayObj* origList = _controller->find(db, ns, select, filter);
	if (origList != NULL) {
		for (BSONArrayObj::iterator itOrig = origList->begin(); itOrig != origList->end(); itOrig++) {
			BSONObj* obj = *itOrig;
			map.add(obj->getString("_id"), obj);
		}
	}

	FilterParser* parser = FilterParser::parse(filter);

	for (std::list<TransactionOperation*>::iterator i = operations->begin(); i != operations->end(); i++) {
		TransactionOperation* operation = *i;
		switch (operation->code) {
			case TXO_INSERT: 
				{
					BsonOper* insert = (BsonOper*)operation->operation;
					BSONObj* bson = insert->bson;

					bool match = false;
					ExpressionResult* expresult = parser->eval(*bson);
					if (expresult->type() == ExpressionResult::RT_BOOLEAN) {
						bool bres = *expresult;
						match = bres;
					}
					delete expresult;

					if (match) {
						map.add(bson->getString("_id"), bson->select(select));
					}
					delete insert;
					delete bson;
				};
				break;
			case TXO_UPDATE: 
				{
					BsonOper* update = (BsonOper*)operation->operation;
					BSONObj* bson = update->bson;

					bool match = false;
					ExpressionResult* expresult = parser->eval(*bson);
					if (expresult->type() == ExpressionResult::RT_BOOLEAN) {
						bool bres = *expresult;
						match = bres;
					}
					delete expresult;

					if (match) {
						map.add(bson->getString("_id"), bson->select(select));
					}
					delete update;
					delete bson;
				};
				break;
			case TXO_REMOVE: 
				{
					RemoveOper* remove = (RemoveOper*)operation->operation;
					const std::string id = remove->key;
					const std::string revision = remove->revision;
					BSONObj* obj = map[id];
					if (obj != NULL) {
						if (obj->getString("_revision").compare(revision) == 0) {
							map.erase(id);
						} else {
							// Error?
						}
					}
					delete remove;
				};
				break;
			case TXO_DROPNAMESPACE:
				{
					map.clear();
				};
				break;
		}
		free(operation->db);
		free(operation->ns);
		delete operation;
	}

	BSONArrayObj* result = new BSONArrayObj();
	for (LinkedMap<std::string, BSONObj*>::iterator it = map.begin(); it != map.end(); it++) {
		BSONObj* r = it->second;
		result->add(*r);
	}
	map.clear();
	delete parser;
	delete operations;
	delete origList;

	return result;
}

BSONObj* BaseTransaction::findFirst(char* db, char* ns, const char* select, const char* filter) throw (ParseException) {
	BSONArrayObj* fullList = find(db, ns, select, filter);

	BSONObj* result = NULL;
	if (fullList != NULL) {
		for (BSONArrayObj::iterator it = fullList->begin(); it != fullList->end(); it++) {
			result = new BSONObj(**it);
			break;
		}
		delete fullList;
	}

	return result;
}

std::vector<std::string>* BaseTransaction::dbs() const {
	return _controller->dbs();
}

std::vector<std::string>* BaseTransaction::namespaces(const char* db) const {
	return _controller->namespaces(db);
}

Controller* BaseTransaction::controller() const {
	return _controller;
}

void BaseTransaction::addBuffers(std::vector<TxBuffer*> buffers) {
	_bufferManager->addBuffers(buffers);
}

void BaseTransaction::join() {
	_bufferManager->join();
}
