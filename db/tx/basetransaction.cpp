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
	std::string logFileName = _mainTransactionLog ? "main.log": *_transactionId + ".log";

	std::string controlFileName = _dataDir + FILESEPARATOR + controlFile;
	logFileName = _dataDir + FILESEPARATOR + logFileName;

	bool existControl = existFile(controlFileName.c_str());
	bool existLogFile = existFile(logFileName.c_str());

	char* flags;
	if (existControl) {
		flags = "rb+";
	} else {
		flags = "wb+";
	}
	_controlFile = new FileInputOutputStream(controlFileName.c_str(), flags); 
	_controlFile->seek(0);

	if (existLogFile) {
		flags = "rb+";
	} else {
		flags = "wb+";
	}
	FileInputOutputStream* logFile = new FileInputOutputStream(logFileName.c_str(), flags); 

	if (existControl) {
		_control.startPos 	  = _controlFile->readLong();
		_control.lastValidPos  = _controlFile->readLong();
		_control.logFile = logFile;

		_control.maximumBufferSize = _controlFile->readInt();

		__int32 buffers = _controlFile->readInt();
		for (__int32 i = 0; i < buffers; i++) {
			TransactionBuffer* buffer = new TransactionBuffer();
			buffer.startPos = _controlFile->readLong();
			buffer.length = _controlFile->readLong();

			_control.logBuffers.push_back(buffer);
		}
	} else {
		_control.startPos = 0;
		_controlFile->writeLong(0);
		_control.lastValidPos = 0;
		_controlFile->writeLong(0);
		
		_control.maximumBufferSize = 64*1024*1024;

		TransactionBuffer* buffer = new TransactionBuffer();
		buffer.startPos = 0;
		buffer.length = 0;
		_control.logBuffers.push_back(buffer);

		// 1 buffer will be initialized
		_controlFile->writeInt(1);
		_controlFile->writeLong(0);
		_controlFile->writeLong(0);
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
	_control.logFile->close();
	if (_controlFile) delete _controlFile;
	if (_control.logFile) delete _control.logFile;
	if (_transactionId) delete _transactionId;
}

void BaseTransaction::writeOperationToRegister(char* db, char* ns, const TransactionOperation& operation) {
	__int64 statusPos = _control.logFile->currentPos();

	MemoryStream buffer;

	buffer->writeChar(TXOS_NORMAL);
	buffer.writeChars(db, strlen(db));
	buffer.writeChars(ns, strlen(ns));
	MemoryStream ms;
	ms.writeInt(operation.code);
	ms.writeString(db);
	ms.writeString(ns);
	BSONOutputStream bos(&ms);
	__int32 code = operation.code;
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
	buffer.writeInt(ms.size());
	char* chrs = ms.toChars();
	buffer.writeChars(chrs, ms.size());
	free(chrs);

	__int64 lastValidPos = statusPos; // the pos of the last valid record //flag is the start

	__int64 bufferSize = buffer.size();
	TxBuffer* txBuffer = _bufferManager->getBuffer(bufferSize);	
	char* chrs = buffer.toChars();
	txBuffer->writeChars(chrs, bufferSize);

	free(chrs);

	// jumps the "startpos" to the "lastpos"
	_controlFile->seek(sizeof(__int64));
	_controlFile->writeLong(lastValidPos);
	_control.lastValidPos = lastValidPos;
	_control.logFile->flush();
}

TransactionOperation* BaseTransaction::readOperationFromRegister(char* db, char* ns) {
	OPERATION_STATUS status = (OPERATION_STATUS)_control.logFile->readChar();
	char* rdb = _control.logFile->readChars();
	char* rns = _control.logFile->readChars();

	TransactionOperation* result = NULL;
	__int32 length = _control.logFile->readInt();
	if (!(status & TXOS_NORMAL)) {
		goto jumpoperation;
	};
	if ((strcmp(rdb, db) == 0) && (strcmp(rns, ns) == 0)) {
		char* stream = _control.logFile->readChars();
		MemoryStream ms(stream, length);
		ms.seek(0);
		TRANSACTION_OPER code = (TRANSACTION_OPER)ms.readInt();
		BSONInputStream bis(&ms);

		result = new TransactionOperation();
		result->status = status;
		result->code = code;
		result->db = ms.readString();
		result->ns = ms.readString();
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
		_control.logFile->seek(_control.logFile->currentPos() + length);
	}
	return result;
}

std::list<TransactionOperation*>* BaseTransaction::findOperations(char* db, char* ns) {
	std::list<TransactionOperation*>* result = new std::list<TransactionOperation*>();
	for (std::queue<TransactionBuffer*>::iterator i = _control.logBuffers.begin(); i != _control.logBuffers.end(); i++) {
		TransactionBuffer* buffer = *i;
		__int32 lastPos = _control.logFile->currentPos();
		_control.logFile->seek(buffer->startPos);
		__int32 currentPos = lastPos;
		while (!_control.logFile->eof() && (currentPos < buffer->length)) {
			TransactionOperation* operation = readOperationFromRegister(db, ns);
			if (operation != NULL) {
				result->push_back(operation);
			}
		}
		_control.logFile->seek(lastPos);
	}
	return result;
}

BSONObj* BaseTransaction::insert(char* db, char* ns, BSONObj* bson) {
	TransactionOperation oper;
	oper.code = TXO_INSERT;
	oper.db = new std::string(db);
	oper.ns = new std::string(ns);
	BsonOper* insertOper = new BsonOper();
	insertOper->bson = new BSONObj(*bson); 
	oper.operation = insertOper;

	writeOperationToRegister(db, ns, oper);

	delete oper.db;
	delete oper.ns;
}

bool BaseTransaction::dropNamespace(char* db, char* ns) {
	TransactionOperation oper;
	oper.code = TXO_DROPNAMESPACE;
	oper.operation = NULL;
	oper.db = NULL;
	oper.ns = NULL;

	writeOperationToRegister(db, ns, oper);
}

void BaseTransaction::update(char* db, char* ns, BSONObj* bson) {
	TransactionOperation oper;
	oper.code = TXO_UPDATE;
	oper.db = new std::string(db);
	oper.ns = new std::string(ns);
	BsonOper* bsonOper = new BsonOper();
	bsonOper->bson = new BSONObj(*bson); 
	oper.operation = bsonOper;

	writeOperationToRegister(db, ns, oper);

	delete oper.db;
	delete oper.ns;
}

void BaseTransaction::remove(char* db, char* ns, const std::string& documentId, const std::string& revision) {
	TransactionOperation oper;
	oper.code = TXO_REMOVE;
	oper.db = new std::string(db);
	oper.ns = new std::string(ns);
	RemoveOper* removeOper = new RemoveOper();
	removeOper->key = std::string(documentId);
	removeOper->revision = std::string(revision);

	writeOperationToRegister(db, ns, oper);

	delete oper.db;
	delete oper.ns;
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

void BaseTransaction::validateCurrentFile() {
	// 64Mb
	if (_control.lastValidPos >= 64*1024*1024) {
		createNewCurrentFile();
	}
}

void BaseTransaction::createNewCurrentFile() {
	// Jumps start and last valid pos
	_controlFile->seek(sizeof(__int64) * 2);

	std::string logfile = (_transactionId == NULL)? "main.tlo": *_transactionId + ".tlo";
	std::string logFileName = _dataDir + FILESEPARATOR + logfile;
	FileInputOutputStream* logFile = new FileInputOutputStream(logFileName, "rb+");
}
