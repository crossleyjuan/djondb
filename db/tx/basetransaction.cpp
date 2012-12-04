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
#include <stdlib.h>

BaseTransaction::BaseTransaction(Controller* controller) {
	_mainTransactionLog = true;
	_controller = controller;
	_transactionId = NULL;

	_dataDir = getSetting("DATA_DIR");

	loadControlFile();
}

BaseTransaction::BaseTransaction(Controller* controller, std::string transactionId) {
	_mainTransactionLog = false;
	_controller = controller;
	_transactionId = new std::string(transactionId);

	_dataDir = getSetting("DATA_DIR");

	loadControlFile();
}

void BaseTransaction::loadControlFile() {
	std::string controlFile = _mainTransactionLog ? "main.trc": *_transactionId + ".trc";
	std::string controlFileName = _dataDir + FILESEPARATOR + controlFile;
	bool existControl = existFile(controlFileName.c_str());

	char* flags;
	if (existControl) {
		flags = "rb+";
	} else {
		flags = "wb+";
	}
	_controlFile = new FileInputOutputStream(controlFileName.c_str(), flags); 
	_controlFile->seek(0);
	if (existControl) {
		_control.startPos 	  = _controlFile->readLong();
		_control.lastValidPos  = _controlFile->readLong();

		int files = _controlFile->readInt();
		for (int i = 0; i < files; i++) {
			std::string* fileName = _controlFile->readString();
			std::string logFileName = _dataDir + FILESEPARATOR + *fileName;
			FileInputOutputStream* logFile = new FileInputOutputStream(logFileName, "rb+");
			_control.logFiles.push_back(logFile);
			_control.currentFile = logFile;
			delete fileName;
		}
	} else {
		_control.startPos = 0;
		_controlFile->writeLong(0);
		_control.lastValidPos = 0;
		_controlFile->writeLong(0);

		std::string logfile = (_transactionId == NULL)? "main.tlo": *_transactionId + ".tlo";
		std::string logFileName = _dataDir + FILESEPARATOR + logfile;
		FileInputOutputStream* fios = new FileInputOutputStream(logFileName, "wb+");
		_control.logFiles.push_back(fios);
		_control.currentFile = fios;
		_controlFile->writeInt(1);
		_controlFile->writeString(logfile);
	}
}

BaseTransaction::BaseTransaction(const BaseTransaction& orig) {
	this->_controller = orig._controller;
	this->_transactionId = orig._transactionId;
	_controlFile = orig._controlFile; 
	_control = orig._control;
}

BaseTransaction::~BaseTransaction() {
	_controlFile->close();
	_control.currentFile->close();
	if (_controlFile) delete _controlFile;
	if (_control.currentFile) delete _control.currentFile;
	if (_transactionId) delete _transactionId;
}

void BaseTransaction::writeOperationToRegister(char* db, char* ns, const TransactionOperation& operation) {
	long statusPos = _control.currentFile->currentPos();

	_control.currentFile->writeChar(TXOS_DIRTY); // it's not a valid state yet, cannot be used
	_control.currentFile->writeChars(db, strlen(db));
	_control.currentFile->writeChars(ns, strlen(ns));
	MemoryStream ms;
	ms.writeInt(operation.code);
	BSONOutputStream bos(&ms);
	int code = operation.code;
	switch (code) {
		case TXO_INSERT: 
		case TXO_UPDATE: {
								  BsonOper* bsonOper = (BsonOper*)operation.operation;
								  bos.writeBSON(*bsonOper->bson);
							  };
							  break;
		case TXO_DROPNAMESPACE:
							  // Nothing needs to be done
							  break;
		case TXO_REMOVE:
							  {
								  RemoveOper* removeOper = (RemoveOper*)operation.operation;
								  ms.writeString(removeOper->key);
								  ms.writeString(removeOper->revision);
								  // Nothing needs to be done
							  };
							  break;
	};
	// writing the length will allow to jump the command if does not match the db and ns
	_control.currentFile->writeInt(ms.length());
	char* chrs = ms.toChars();
	_control.currentFile->writeChars(chrs, ms.length());
	free(chrs);

	long endFile = _control.currentFile->currentPos();

	long lastValidPos = statusPos; // the pos of the last valid record //flag is the start

	_control.currentFile->seek(statusPos);
	_control.currentFile->writeChar(TXOS_NORMAL); // Now the operation is ready to be used
	_control.currentFile->seek(endFile);

	// jumps the "startpos" to the "lastpos"
	_controlFile->seek(sizeof(long));
	_controlFile->writeLong(lastValidPos);
	_control.lastValidPos = lastValidPos;
	_control.currentFile->flush();
}

TransactionOperation* BaseTransaction::readOperationFromRegister(char* db, char* ns) {
	OPERATION_STATUS status = (OPERATION_STATUS)_control.currentFile->readChar();
	char* rdb = _control.currentFile->readChars();
	char* rns = _control.currentFile->readChars();

	TransactionOperation* result = NULL;
	int length = _control.currentFile->readInt();
	if (!(status && TXOS_NORMAL)) {
		goto jumpoperation;
	};
	if ((strcmp(rdb, db) == 0) && (strcmp(rns, ns) == 0)) {
		char* stream = _control.currentFile->readChars();
		MemoryStream ms(stream, length);
		TRANSACTION_OPER code = (TRANSACTION_OPER)ms.readInt();
		BSONInputStream bis(&ms);

		result = new TransactionOperation();
		result->status = status;
		result->code = code;
		result->operation = NULL;
		switch (code) {
			case TXO_INSERT: 
			case TXO_UPDATE: {
									  BsonOper* bsonOper = new BsonOper();
									  bsonOper->bson = bis.readBSON();
									  result->operation = bsonOper;
									  break;
								  };
			case TXO_DROPNAMESPACE:
								  {
									  break;
								  };
			case TXO_REMOVE:
								  {
									  RemoveOper* removeOper = new RemoveOper();
									  std::string* key = ms.readString();
									  removeOper->key = *key;
									  delete key;
									  std::string* revision = ms.readString();
									  removeOper->revision = *revision;
									  delete revision;
									  result->operation = removeOper;
									  break;
								  };
		};
	} else {
jumpoperation:
		_control.currentFile->seek(_control.currentFile->currentPos() + length);
	}
	return result;
}

std::list<TransactionOperation*>* BaseTransaction::findOperations(char* db, char* ns) {
	std::list<TransactionOperation*>* result = new std::list<TransactionOperation*>();
	for (std::vector<FileInputOutputStream*>::iterator i = _control.logFiles.begin(); i != _control.logFiles.end(); i++) {
		FileInputOutputStream* file = *i;
		_control.currentFile = file;
		int currentPos = _control.currentFile->currentPos();
		_control.currentFile->seek(0);
		while (!_control.currentFile->eof()) {
			TransactionOperation* operation = readOperationFromRegister(db, ns);
			if (operation != NULL) {
				result->push_back(operation);
			}
		}
		_control.currentFile->seek(currentPos);
	}
	return result;
}

BSONObj* BaseTransaction::insert(char* db, char* ns, BSONObj* bson) {
	TransactionOperation oper;
	oper.code = TXO_INSERT;
	BsonOper* insertOper = new BsonOper();
	insertOper->bson = new BSONObj(*bson); 
	oper.operation = insertOper;

	writeOperationToRegister(db, ns, oper);

	_controller->insert(db, ns, bson);
}

bool BaseTransaction::dropNamespace(char* db, char* ns) {
	TransactionOperation oper;
	oper.code = TXO_DROPNAMESPACE;
	oper.operation = NULL;

	writeOperationToRegister(db, ns, oper);

	_controller->dropNamespace(db, ns);
}

void BaseTransaction::update(char* db, char* ns, BSONObj* bson) {
	TransactionOperation oper;
	oper.code = TXO_UPDATE;
	BsonOper* bsonOper = new BsonOper();
	bsonOper->bson = new BSONObj(*bson); 
	oper.operation = bsonOper;

	writeOperationToRegister(db, ns, oper);

	_controller->update(db, ns, bson);
}

void BaseTransaction::remove(char* db, char* ns, const std::string& documentId, const std::string& revision) {
	TransactionOperation oper;
	oper.code = TXO_REMOVE;
	RemoveOper* removeOper = new RemoveOper();
	removeOper->key = std::string(documentId);
	removeOper->revision = std::string(revision);

	writeOperationToRegister(db, ns, oper);

	_controller->remove(db, ns, documentId, revision);
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
						bool* bres = (bool*)expresult->value();
						match = *bres;
					}
					delete expresult;

					if (match) {
						map.add(bson->getString("_id"), bson->select(select));
					}
				};
				break;
			case TXO_UPDATE: 
				{
					BsonOper* update = (BsonOper*)operation->operation;
					BSONObj* bson = update->bson;

					bool match = false;
					ExpressionResult* expresult = parser->eval(*bson);
					if (expresult->type() == ExpressionResult::RT_BOOLEAN) {
						bool* bres = (bool*)expresult->value();
						match = *bres;
					}
					delete expresult;

					if (match) {
						map.add(bson->getString("_id"), bson->select(select));
					}
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
				};
				break;
			case TXO_DROPNAMESPACE:
				{
					map.clear();
				};
				break;
		}
	}

	BSONArrayObj* result = new BSONArrayObj();
	for (LinkedMap<std::string, BSONObj*>::iterator it = map.begin(); it != map.end(); it++) {
		result->add(*it->second);
	}
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
			result = *it;
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

