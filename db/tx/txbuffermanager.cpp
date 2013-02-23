/*
 * =====================================================================================
 *
 *       Filename:  txbuffermanager.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2/6/2013 08:26:29 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Juan Pablo Crossley (Cross), cross@djondb.com
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
#include "txbuffermanager.h"
#include "fileinputoutputstream.h"
#include "transactiondefs.h"
#include "controller.h"
#include "memorystream.h"
#include "bsoninputstream.h"
#include "bsonoutputstream.h"
#include "lock.h"
#include "bson.h"
#include "util.h"
#include <string.h>
#include <stdlib.h>

__int64 TX_DEFAULT_BUFFER_SIZE = pageSize() * 10 * 1024;

TxBufferManager::TxBufferManager(Controller* controller, const char* file) {
	_buffersSize = TX_DEFAULT_BUFFER_SIZE;
	_buffersCount = 0;
	_dataDir = getSetting("DATA_DIR");
	_controller = controller;
	_lockActiveBuffers = new Lock();
	_monitorThread = new Thread(&TxBufferManager::monitorBuffers);
	_monitorThread->start(this);

	initialize(file);
}

void TxBufferManager::initialize(const char* file) {
	std::string controlFileName = _dataDir + FILESEPARATOR + std::string(file) + ".trc";
	std::string fileName = _dataDir + FILESEPARATOR + std::string(file) + ".log";
	_logFileName = strcpy(const_cast<char*>(fileName.c_str()), fileName.length());

	bool existControl = existFile(controlFileName.c_str());

	char* flags;
	if (existControl) {
		flags = "rb+";
	} else {
		flags = "wb+";
	}
	_controlFile = (InputOutputStream*)new FileInputOutputStream(controlFileName.c_str(), flags); 
	_controlFile->seek(0);

	bool existLogFile = existFile(_logFileName);
	if (existLogFile) {
		flags = "rb+";
	} else {
		flags = "wb+";
	}

	if (existControl) {
		_buffersSize = _controlFile->readInt();
		
		loadBuffers();
	} else {
		_controlFile->writeLong(_buffersSize);
		__int64 pos = _controlFile->currentPos();
		_controlFile->writeInt(0);
		_controlFile->seek(pos);
		loadBuffers();
	}
}

void TxBufferManager::loadBuffers() {
	openLogFile();

	__int32 buffers = _controlFile->readInt();

	for (__int32 x = 0; x < buffers; x++) {
		char flag = _controlFile->readChar();
		__int64 startOffset = _controlFile->readLong();
		__int64 bufferLen = _controlFile->readLong();
		TxBuffer* buffer = new TxBuffer(this, _logFileName, startOffset, bufferLen, _buffersSize / pageSize());

		if (flag & 0x01) {
			addBuffer(buffer);
		} else {
			addReusable(buffer);
		}
	}
}

TxBufferManager::~TxBufferManager() {
	while (!_activeBuffers.empty()) {
		TxBuffer* buffer =  _activeBuffers.front();
		_activeBuffers.pop();
		delete buffer;
	}
	while (!_reusableBuffers.empty()) {
		TxBuffer* buffer = _reusableBuffers.front();
		_reusableBuffers.pop();
		delete buffer;
	}
	_controlFile->close();
	delete _controlFile;
	delete _lockActiveBuffers;
	_monitorThread->stop();
	delete _monitorThread;
	free(_logFileName);
}

TxBuffer* TxBufferManager::createNewBuffer() {
	TxBuffer* result = new TxBuffer(this, _logFileName, _buffersCount * _buffersSize, (__int64)0, _buffersSize / pageSize());
	return result;
}

TxBuffer* TxBufferManager::getBuffer(__int32 minimumSize) {
	TxBuffer* result = NULL;
	if (!_activeBuffers.empty()) {
		result = _activeBuffers.back();
		// because some other action could change the currentPos
		// we should ensure its in the right place before returning the buffer
		result->seek(result->currentPos());
	}
	if ((result == NULL) 
			|| ((_buffersSize - result->currentPos()) < minimumSize)) {

		_controlFile->seek(sizeof(__int64)); 
		_controlFile->writeInt(_buffersCount + 1);
		// Active buffer
		if (_reusableBuffers.empty()) {
			_controlFile->seek(0, FROMEND_SEEK);
			__int32 controlPos = _controlFile->currentPos();
			result = createNewBuffer();
			result->setControlPosition(controlPos);
		} else {
			result = _reusableBuffers.front();
			_reusableBuffers.pop();
			_controlFile->seek(result->controlPosition());
			result->reset();
		}
		addBuffer(result);
		result->seek(0);
		_controlFile->writeChar((char)0x01);
		_controlFile->writeLong(result->startOffset());
		_controlFile->writeLong(0);
		_buffersCount++;
	}
	return result;
}

void TxBufferManager::openLogFile() {
	bool existLogFile = existFile(_logFileName);
	char* flags = NULL;
	if (existLogFile) {
		flags = "rb+";
	} else {
		flags = "wb+";
	}
}

void TxBufferManager::addBuffer(TxBuffer* buffer) {
	_activeBuffers.push(buffer);
	_vactiveBuffers.push_back(buffer);
	// Notify that a new buffer is available
	_lockActiveBuffers->notify();
}

void TxBufferManager::addReusable(TxBuffer* buffer) {
	_reusableBuffers.push(buffer);
}

std::vector<TxBuffer*> TxBufferManager::getActiveBuffers() const {
	return _vactiveBuffers;
}

TxBuffer* TxBufferManager::pop() {
	TxBuffer* buffer = _activeBuffers.front();
	_activeBuffers.pop();

	__int64 controlPos = buffer->controlPosition();
	_controlFile->seek(controlPos);
	_controlFile->writeChar((char)0x02); // reusable
	_buffersCount--;

	_reusableBuffers.push(buffer);
	return buffer;
}

__int32 TxBufferManager::buffersCount() const {
	return _buffersCount;
}

void* TxBufferManager::monitorBuffers(void* arg) {
	TxBufferManager* manager = (TxBufferManager*)arg;
	while (true) {
		// Waiting for notifications on new buffers
		manager->flushBuffer();
	}
}

void TxBufferManager::flushBuffer() {
	_lockActiveBuffers->wait(3);
	if (buffersCount() > 1) {
		TxBuffer* buffer = pop();

		TransactionOperation* operation = NULL;
		buffer->seek(0);
		while (true) {
			operation = readOperationFromRegister(buffer);
			if (operation == NULL) {
				break;
			}
			char* db = operation->db;
			char* ns = operation->ns;
			switch (operation->code) {
				case TXO_INSERT: 
					{
						BsonOper* insert = (BsonOper*)operation->operation;
						BSONObj* bson = insert->bson;
						_controller->insert(db, ns, bson);
						delete bson;
					};
					break;
				case TXO_UPDATE: 
					{
						BsonOper* update = (BsonOper*)operation->operation;
						BSONObj* bson = update->bson;
						_controller->update(db, ns, bson);
						delete bson;
					};
					break;
				case TXO_REMOVE: 
					{
						RemoveOper* remove = (RemoveOper*)operation->operation;
						char* id = remove->key;
						char* revision = remove->revision;
						_controller->remove(db, ns, id, revision);
					};
					break;
				case TXO_DROPNAMESPACE:
					{
						_controller->dropNamespace(db, ns);
					};
					break;
			}
			delete operation;
		}
	}
}

void TxBufferManager::writeOperationToRegister(char* db, char* ns, const TransactionOperation& operation) {
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

	TxBuffer* txBuffer = getBuffer(bufferSize + sizeof(__int64));	
	txBuffer->acquireLock();
	chrs = buffer.toChars();
	txBuffer->writeLong(bufferSize);
	txBuffer->writeChars(chrs, bufferSize);

	txBuffer->flush();
	txBuffer->releaseLock();

	free(chrs);
}

TransactionOperation* TxBufferManager::readOperationFromRegister(TxBuffer* buffer) {
	return readOperationFromRegister(buffer, NULL, NULL);
}

TransactionOperation* TxBufferManager::readOperationFromRegister(TxBuffer* buffer, char* db, char* ns) {
	if (buffer->eof()) {
		return NULL;
	}
	buffer->acquireLock();
	__int64 size = buffer->readLong();

	MemoryStream* stream = new MemoryStream(buffer->readChars(), size);
	buffer->releaseLock();

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
		result->db = ms.readChars();
		result->ns = ms.readChars();
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
									  char* key = ms.readChars();
									  removeOper->key = key;
									  free(key);
									  char* revision = ms.readChars();
									  removeOper->revision = revision;
									  free(revision);
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
