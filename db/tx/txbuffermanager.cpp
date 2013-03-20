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

__int64 TX_DEFAULT_BUFFER_SIZE = pageSize() * 20;

TxBufferManager::TxBufferManager(Controller* controller, const char* file, bool mainLog) {
	_buffersSize = TX_DEFAULT_BUFFER_SIZE;
	_buffersCount = 0;
	_dataDir = getSetting("DATA_DIR");
	_controller = controller;
	_lockActiveBuffers = new Lock();
	_flushingBuffers = false;
	_runningMonitor = false;
	_monitorThread = NULL;
	_mainLog = mainLog;

	initialize(file);
}

void TxBufferManager::initialize(const char* file) {
	std::string controlFileName = std::string(file) + ".trc";
	std::string fullcontrolFileName = combinePath(_dataDir.c_str(), controlFileName.c_str());
	std::string fileName = std::string(file) + ".log";
	std::string fullLogFileName = combinePath(_dataDir.c_str(), fileName.c_str());
	_logFileName = strcpy(const_cast<char*>(fileName.c_str()), fileName.length());

	bool existControl = existFile(fullcontrolFileName.c_str());

	char* flags;
	if (existControl) {
		flags = "rb+";
	} else {
		flags = "wb+";
	}
	_controlFile = (InputOutputStream*)new FileInputOutputStream(fullcontrolFileName, flags); 
	_controlFile->seek(0);

	bool existLogFile = existFile(fullLogFileName.c_str());
	if (existLogFile) {
		flags = "rb+";
	} else {
		flags = "wb+";
	}

	if (existControl) {
		_buffersSize = _controlFile->readLong();
		
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
		__int32 controlPosition = _controlFile->currentPos();
		char flag = _controlFile->readChar();
		__int64 startOffset = _controlFile->readLong();
		__int64 bufferLen = _controlFile->readLong();
		char* logFileName = _controlFile->readChars();
		bool mainLog = (bool)_controlFile->readInt();
		TxBuffer* buffer = new TxBuffer(this, logFileName, startOffset, bufferLen, _buffersSize / pageSize(), mainLog);
		buffer->setControlPosition(controlPosition);

		if (flag & 0x01) {
			addBuffer(buffer);
		} else {
			addReusable(buffer);
		}
	}
}

TxBufferManager::~TxBufferManager() {
	if (runningMonitor()) {
		stopMonitor();
	}

	// Wait until flushBuffers finished its work
	while (_flushingBuffers) {
		Thread::sleep(30);
	}
	while (!_activeBuffers.empty()) {
		TxBuffer* buffer =  _activeBuffers.front();
		_activeBuffers.pop();
		buffer->close();
		delete buffer;
	}
	while (!_reusableBuffers.empty()) {
		TxBuffer* buffer = _reusableBuffers.front();
		_reusableBuffers.pop();
		buffer->close();
		delete buffer;
	}
	_controlFile->close();
	delete _controlFile;
	delete _lockActiveBuffers;
	if (_monitorThread) delete _monitorThread;
	free(_logFileName);
}

TxBuffer* TxBufferManager::createNewBuffer() {
	TxBuffer* result = new TxBuffer(this, _logFileName, _buffersCount * _buffersSize, (__int64)0, _buffersSize / pageSize(), _mainLog);
	return result;
}

void registerBufferControlFile(InputOutputStream* controlFile, std::map<std::string, int> buffersByLog, TxBuffer* buffer, bool newBuffer, int flag, int buffersCount) {
	controlFile->acquireLock();
	controlFile->seek(sizeof(__int64)); 
	controlFile->writeInt(buffersCount);
	// Active buffer
	if (newBuffer) {
		controlFile->seek(0, FROMEND_SEEK);
		__int32 controlPos = controlFile->currentPos();
		buffer->setControlPosition(controlPos);
	} else {
		controlFile->seek(buffer->controlPosition());
		buffer->reset();
	}
	controlFile->writeChar((char)0x01);
	controlFile->writeLong(buffer->startOffset());
	controlFile->writeLong(0);
	const std::string fileName = buffer->fileName();
	std::map<std::string, int>::iterator itBuffersByLog = buffersByLog.find(fileName);
	if (itBuffersByLog == buffersByLog.end()) {
		buffersByLog.insert(pair<std::string, int>(fileName, 1));
	} else {
		int* count = &itBuffersByLog->second;
		*count = *count + 1;
	}
	controlFile->writeChars(fileName.c_str(), fileName.length());
	controlFile->writeInt((int)buffer->mainLog());
	controlFile->releaseLock();
}

TxBuffer* TxBufferManager::getBuffer(__int32 minimumSize) {
	return getBuffer(minimumSize, false);
}

TxBuffer* TxBufferManager::getBuffer(__int32 minimumSize, bool force) {
	TxBuffer* result = NULL;
	if (!_activeBuffers.empty()) {
		result = _activeBuffers.back();
		// because some other action could change the currentPos
		// we should ensure its in the right place before returning the buffer
		result->seek(result->currentPos());
	}
	if ((force) || (result == NULL) 
			|| ((_buffersSize - result->currentPos()) < minimumSize)) {

		bool newBuffer;
		int flag = 0x01;
		// Active buffer
		if (_reusableBuffers.empty()) {
			result = createNewBuffer();
			newBuffer = true;
		} else {
			result = _reusableBuffers.front();
			_reusableBuffers.pop();
			result->reset();
			newBuffer = false;
		}
		addBuffer(result);
		result->seek(0);

		_buffersCount++;
		registerBufferControlFile(_controlFile, _buffersByLog, result, newBuffer, flag, _buffersCount);
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
	_lockActiveBuffers->lock();
	_activeBuffers.push(buffer);
	_vactiveBuffers.push_back(buffer);
	_lockActiveBuffers->unlock();
	// Notify that a new buffer is available
	_lockActiveBuffers->notify();
}

void TxBufferManager::addBuffers(std::vector<TxBuffer*> buffers) {
	_lockActiveBuffers->lock();
	for (std::vector<TxBuffer*>::iterator it = buffers.begin(); it != buffers.end(); it++) {
		TxBuffer* buffer = *it;
		registerBufferControlFile(_controlFile, _buffersByLog, buffer, true, 0x01, _buffersCount);
		_activeBuffers.push(buffer);
		_vactiveBuffers.push_back(buffer);
		_buffersCount++;
	}
	_lockActiveBuffers->unlock();
	// this will force a new buffer to be put
	getBuffer(0, true);
}

void TxBufferManager::addReusable(TxBuffer* buffer) {
	_reusableBuffers.push(buffer);
}

std::vector<TxBuffer*> TxBufferManager::getActiveBuffers() const {
	return _vactiveBuffers;
}

std::vector<TxBuffer*> TxBufferManager::popAll() {
	std::vector<TxBuffer*> result;
	while (buffersCount() > 0) {
		result.push_back(pop());
	}
	return result;
}

TxBuffer* TxBufferManager::pop() {
	TxBuffer* buffer = _activeBuffers.front();
	_activeBuffers.pop();

	__int64 controlPos = buffer->controlPosition();
	_controlFile->seek(controlPos);
	_controlFile->writeChar((char)0x01); // reusable
	_buffersCount--;

	return buffer;
}

__int32 TxBufferManager::buffersCount() const {
	return _buffersCount;
}

void TxBufferManager::startMonitor() {
	_monitorThread = new Thread(&TxBufferManager::monitorBuffers);
	_runningMonitor = true;
	_monitorThread->start(this);
}

bool TxBufferManager::runningMonitor() const {
	return _runningMonitor;
}

void TxBufferManager::stopMonitor() {
	_runningMonitor = false;
	// Sends a notification to release the inner wait
	_lockActiveBuffers->notify();
}

void* TxBufferManager::monitorBuffers(void* arg) {
	Logger* log = getLogger(NULL);
	TxBufferManager* manager = (TxBufferManager*)arg;
	while (manager->runningMonitor()) {
		// Waiting for notifications on new buffers
		manager->flushBuffer();
	}
	if (log->isDebug()) log->debug(2, "TxBufferManager::~monitorBuffers");
}

void TxBufferManager::flushBuffer() {
	_lockActiveBuffers->wait(3);
	_flushingBuffers = true;
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
						delete insert;
					};
					break;
				case TXO_UPDATE: 
					{
						BsonOper* update = (BsonOper*)operation->operation;
						BSONObj* bson = update->bson;
						_controller->update(db, ns, bson);
						delete bson;
						delete update;
					};
					break;
				case TXO_REMOVE: 
					{
						RemoveOper* remove = (RemoveOper*)operation->operation;
						char* id = remove->key;
						char* revision = remove->revision;
						_controller->remove(db, ns, id, revision);
						delete remove;
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
		if (buffer->mainLog()) {
			addReusable(buffer);
		} else {
			dropBuffer(buffer);
		}
	}
	_flushingBuffers = false;
}

void TxBufferManager::dropBuffer(TxBuffer* buffer) {
	if (!buffer->mainLog()) {
		std::map<std::string, int>::iterator it = _buffersByLog.find(buffer->fileName());
		int* count = &it->second;
		*count--;
		if (*count == 0) {
			std::string file = buffer->fileName();
			removeFile(file.c_str());
		}
	}
	delete buffer;
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

	char* fullBuffer = buffer->readChars();
	MemoryStream* stream = new MemoryStream(fullBuffer, size);
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
		free(cstream);
	} else {
jumpoperation:
		stream->seek(stream->currentPos() + length);
	}

	free(rdb);
	free(rns);
	delete stream;
	free(fullBuffer);
	return result;
}

void TxBufferManager::join() {
	if (_monitorThread != NULL) {
		this->_monitorThread->join();
	}
}
