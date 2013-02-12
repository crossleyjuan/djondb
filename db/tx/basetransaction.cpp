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

	_bufferManager = new TxBufferManager(file.c_str());
}

BaseTransaction::BaseTransaction(const BaseTransaction& orig) {
	this->_controller = orig._controller;
	this->_transactionId = orig._transactionId;
	this->_bufferManager = orig._bufferManager;
}

BaseTransaction::~BaseTransaction() {
	delete _bufferManager;
	if (_transactionId) delete _transactionId;
}

void BaseTransaction::writeOperationToRegister(char* db, char* ns, const TransactionOperation& operation) {
	MemoryStream buffer;

	buffer.writeChar(TXOS_NORMAL);
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

	__int64 bufferSize = buffer.size();

	TxBuffer* txBuffer = _bufferManager->getBuffer(bufferSize + sizeof(__int64));	
	chrs = buffer.toChars();
	txBuffer->writeLong(bufferSize);
	txBuffer->writeChars(chrs, bufferSize);

	txBuffer->flush();

	free(chrs);
}

TransactionOperation* BaseTransaction::readOperationFromRegister(TxBuffer* buffer) {
	return readOperationFromRegister(buffer, NULL, NULL);
}

TransactionOperation* BaseTransaction::readOperationFromRegister(TxBuffer* buffer, char* db, char* ns) {
	__int64 size = buffer->readLong();

	MemoryStream* stream = new MemoryStream(buffer->readChars(), size);

	stream->seek(0);

	OPERATION_STATUS status = (OPERATION_STATUS)stream->readChar();
	char* rdb = stream->readChars();
	char* rns = stream->readChars();

	TransactionOperation* result = NULL;
	__int32 length = stream->readInt();
	if (!(status & TXOS_NORMAL)) {
		goto jumpoperation;
	};
	if ((db == NULL) || (ns == NULL) || ((strcmp(rdb, db) == 0) && (strcmp(rns, ns) == 0))) {
		char* cstream = stream->readChars();
		MemoryStream ms(cstream, length);
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
		stream->seek(stream->currentPos() + length);
	}

	delete stream;
	return result;
}

std::list<TransactionOperation*>* BaseTransaction::findOperations(char* db, char* ns) {
	std::list<TransactionOperation*>* result = new std::list<TransactionOperation*>();
	std::vector<TxBuffer*> buffers = _bufferManager->getActiveBuffers();
	for (std::vector<TxBuffer*>::iterator i = buffers.begin(); i != buffers.end(); i++) {
		TxBuffer* buffer = *i;
		buffer->seek(0);
		while (!buffer->eof()) {
			TransactionOperation* operation = readOperationFromRegister(buffer, db, ns);
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

void BaseTransaction::flushBuffer() {
	if (_bufferManager->buffersCount() > 1) {
		TxBuffer* buffer = _bufferManager->pop();

		TransactionOperation* operation = NULL;
		while ((operation = readOperationFromRegister(buffer)) != NULL) {
			std::string* db = operation->db;
			std::string* ns = operation->ns;
			switch (operation->code) {
				case TXO_INSERT: 
					{
						BsonOper* insert = (BsonOper*)operation->operation;
						BSONObj* bson = insert->bson;
						_controller->insert(const_cast<char*>(db->c_str()), const_cast<char*>(ns->c_str()), bson);
						delete bson;
					};
					break;
				case TXO_UPDATE: 
					{
						BsonOper* update = (BsonOper*)operation->operation;
						BSONObj* bson = update->bson;
						_controller->update(const_cast<char*>(db->c_str()), const_cast<char*>(ns->c_str()), bson);
						delete bson;
					};
					break;
				case TXO_REMOVE: 
					{
						RemoveOper* remove = (RemoveOper*)operation->operation;
						const std::string id = remove->key;
						const std::string revision = remove->revision;
						_controller->remove(const_cast<char*>(db->c_str()), const_cast<char*>(ns->c_str()), id.c_str(), revision.c_str());
					};
					break;
				case TXO_DROPNAMESPACE:
					{
						_controller->dropNamespace(const_cast<char*>(db->c_str()), const_cast<char*>(ns->c_str()));
					};
					break;
			}
			delete operation;
		}
	}
}
