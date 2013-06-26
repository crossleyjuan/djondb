/*
 * =====================================================================================
 *
 *       Filename:  txbuffermanager.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2/6/2013 08:26:29 PM *       Revision:  none
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
#include <errno.h>

__int64 TX_DEFAULT_BUFFER_SIZE = pageSize() * 20;

TxBufferManager::TxBufferManager(Controller* controller, const char* file, bool mainLog) {
	_buffersSize = TX_DEFAULT_BUFFER_SIZE;
	_buffersCount = 0;
	_dataDir = getSetting("DATA_DIR");
	_controller = controller;
	_lockActiveBuffers = new Lock();
	_lockWait = new Lock();
	_flushingBuffers = false;
	_runningMonitor = false;
	_monitorThread = NULL;
	_mainLog = mainLog;
	_activeBuffers = new std::queue<TxBuffer*>();
	_vactiveBuffers = new std::vector<TxBuffer*>();
	_reusableBuffers = new std::queue<TxBuffer*>();
	_buffersByLog = new std::map<std::string, int*>();
	_log = getLogger(NULL);

	initialize(file);
}

void TxBufferManager::initialize(const char* file) {
	std::string controlFileName = std::string(file) + ".trc";
	char* fullcontrolFileName = combinePath(_dataDir.c_str(), controlFileName.c_str());
	std::string fileName = std::string(file) + ".log";
	char* fullLogFileName = combinePath(_dataDir.c_str(), fileName.c_str());
	_logFileName = strcpy(const_cast<char*>(fileName.c_str()), fileName.length());

	bool existControl = existFile(fullcontrolFileName);

	char* flags;
	if (existControl) {
		flags = "rb+";
	} else {
		flags = "wb+";
	}
	_controlFile = (InputOutputStream*)new FileInputOutputStream(fullcontrolFileName, flags); 
	if (_log->isDebug()) _log->debug(3, "_controlFile->acquireLock();");
	_controlFile->acquireLock();
	_controlFile->seek(0);

	bool existLogFile = existFile(fullLogFileName);
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
	if (_log->isDebug()) _log->debug(3, "_controlFile->releaseLock();");
	_controlFile->releaseLock();

	free(fullcontrolFileName);
	free(fullLogFileName);
}

void TxBufferManager::loadBuffers() {
	openLogFile();

	__int32 buffers = _controlFile->readInt();

	// The buffers are circular, this means that some
	// buffers are in the front of the control file but
	// they are at the tail of the queue
	std::vector<TxBuffer*> front;
	std::vector<TxBuffer*> tail;
	bool addToTail = true;
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
			if (addToTail) {
				tail.push_back(buffer);
			} else {
				front.push_back(buffer);
			}
		} else {
			addToTail = false;
			addReusable(buffer);
		}
		free(logFileName);
	}
	for (std::vector<TxBuffer*>::iterator iter = front.begin(); iter != front.end(); iter++) {
		TxBuffer* buffer = *iter;
		addBuffer(buffer);
	}
	for (std::vector<TxBuffer*>::iterator iter = tail.begin(); iter != tail.end(); iter++) {
		TxBuffer* buffer = *iter;
		addBuffer(buffer);
	}
}

TxBufferManager::~TxBufferManager() {
	_log->debug(2, "TxBufferManager::~TxBufferManager()");
	if (runningMonitor()) {
		stopMonitor();
	}

	/*
		while (!_activeBuffers->empty()) {
		TxBuffer* buffer =  _activeBuffers->front();
		_activeBuffers->pop();
		buffer->close();
		delete buffer;
		}
		while (!_reusableBuffers->empty()) {
		TxBuffer* buffer = _reusableBuffers->front();
		_reusableBuffers->pop();
		buffer->close();
		delete buffer;
		}
		*/
	_controlFile->close();
	delete _controlFile;
	delete _lockActiveBuffers;
	// Releases the waits
	_lockWait->lock();
	_lockWait->notify();
	_lockWait->unlock();
	delete _lockWait;
	delete _activeBuffers;
	delete _vactiveBuffers;
	delete _reusableBuffers;
	for (std::map<std::string, int*>::iterator i = _buffersByLog->begin(); i != _buffersByLog->end(); i++) {
		int* count = i->second;
		delete count;
	}
	delete _buffersByLog;
	if (_monitorThread) {
		delete _monitorThread;
		_monitorThread = 0;
	}
	free(_logFileName);
}

TxBuffer* TxBufferManager::createNewBuffer() {
	TxBuffer* result = new TxBuffer(this, _logFileName, _buffersCount * _buffersSize, (__int64)0, _buffersSize / pageSize(), _mainLog);
	return result;
}

void TxBufferManager::registerBufferControlFile(TxBuffer* buffer, bool newBuffer, int flag) {
	if (_log->isDebug()) _log->debug(3, "_controlFile->acquireLock();");
	_controlFile->acquireLock();
	// Jumps the buffersSize (first 8 bytes)
	_controlFile->seek(sizeof(__int64)); 
	_controlFile->writeInt(_buffersCount);
	// Active buffer
	if (newBuffer) {
		_controlFile->seek(0, FROMEND_SEEK);
		__int32 controlPos = _controlFile->currentPos();
		buffer->setControlPosition(controlPos);
	} else {
		_controlFile->seek(buffer->controlPosition());
		buffer->reset();
	}
	_controlFile->writeChar((char)0x01);
	_controlFile->writeLong(buffer->startOffset());
	_controlFile->writeLong(0);
	const std::string fileName = buffer->fileName();
	std::map<std::string, int*>::iterator itBuffersByLog = _buffersByLog->find(fileName);
	if (itBuffersByLog == _buffersByLog->end()) {
		int* count = new int();
		*count = 1;
		_buffersByLog->insert(pair<std::string, int*>(fileName, count));
	} else {
		int* count = itBuffersByLog->second;
		(*count)++;
	}
	_controlFile->writeChars(fileName.c_str(), fileName.length());
	_controlFile->writeInt((int)buffer->mainLog());
	if (_log->isDebug()) _log->debug(3, "_controlFile->releaseLock();");
	_controlFile->releaseLock();
}

TxBuffer* TxBufferManager::getBuffer(__int32 minimumSize) {
	return getBuffer(minimumSize, false);
}

TxBuffer* TxBufferManager::getBuffer(__int32 minimumSize, bool force) {
	TxBuffer* result = NULL;
	if (!_activeBuffers->empty()) {
		result = _activeBuffers->back();
		// because some other action could change the currentPos
		// we should ensure its in the right place before returning the buffer
		result->seek(result->currentPos());
	}
	if ((force) || (result == NULL) 
			|| ((_buffersSize - result->currentPos()) < minimumSize)) {

		bool newBuffer;
		int flag = 0x01;
		// Active buffer
		if (_reusableBuffers->empty()) {
			result = createNewBuffer();
			newBuffer = true;
		} else {
			result = _reusableBuffers->front();
			_reusableBuffers->pop();
			result->reset();
			newBuffer = false;
		}
		addBuffer(result);
		result->seek(0);

		registerBufferControlFile(result, newBuffer, flag);
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
	if (_log->isDebug()) _log->debug(2, "TxBufferManager::addBuffer()");

	if (_log->isDebug()) _log->debug(2, "_lockActiveBuffers->lock() %d", (long)_lockActiveBuffers);
	_lockActiveBuffers->lock();
	_activeBuffers->push(buffer);
	_vactiveBuffers->push_back(buffer);
	_buffersCount++;
	if (_log->isDebug()) _log->debug(2, "_lockActiveBuffers->unlock() %d", (long)_lockActiveBuffers);
	_lockWait->lock();
	_lockWait->notify();
	_lockWait->unlock();
	_lockActiveBuffers->unlock();
}

#ifdef DEBUG
void TxBufferManager::debugFlushBuffers() {
	if (_log->isDebug()) {
		_log->debug(2, "Buffers to be flushed");
		for (std::vector<TxBuffer*>::iterator i = _vactiveBuffers->begin(); i != _vactiveBuffers->end(); i++) {
			TxBuffer* buffer = *i;
			if (_log->isDebug()) _log->debug(2, "   Buffer fileName: %s, mainLog: %s", buffer->fileName().c_str(), buffer->mainLog() ? "true": "false");
		}
	}
	if (_log->isDebug()) _log->debug(2, "debugFlushBuffers buffersCount: %d", buffersCount());
	while (buffersCount() > 1) {
		flushBuffer();
	}
}
#endif

void TxBufferManager::addBuffers(std::vector<TxBuffer*> buffers) {
	if (_log->isDebug()) _log->debug(2, "TxBufferManager::addBuffers()");

	if (_log->isDebug()) _log->debug(3, "_lockActiveBuffers->lock() %d", (long)_lockActiveBuffers);
	_lockActiveBuffers->lock();
	for (std::vector<TxBuffer*>::iterator it = buffers.begin(); it != buffers.end(); it++) {
		TxBuffer* buffer = *it;
		if (_log->isDebug()) _log->debug(2, "Adding buffer. fileName: %s, mainLog: %s", buffer->fileName().c_str(), buffer->mainLog() ? "true": "false");
		_buffersCount++;
		registerBufferControlFile(buffer, true, 0x01);
		_activeBuffers->push(buffer);
		_vactiveBuffers->push_back(buffer);
	}
	if (_log->isDebug()) _log->debug(3, "_lockActiveBuffers->unlock() %d", (long)_lockActiveBuffers);
	_lockActiveBuffers->notify();
	_lockActiveBuffers->unlock();
	// this will force a new buffer to be put
	getBuffer(0, true);
	if (_log->isDebug()) _log->debug("buffersCount: %d", buffersCount());
}

void TxBufferManager::addReusable(TxBuffer* buffer) {
	_reusableBuffers->push(buffer);
}

const std::vector<TxBuffer*>* TxBufferManager::getActiveBuffers() const {
	return _vactiveBuffers;
}

std::vector<TxBuffer*> TxBufferManager::popAll() {
	if (_log->isDebug()) _log->debug(2, "TxBufferManager::popAll()");
	std::vector<TxBuffer*> result;
	while (buffersCount() > 0) {
		result.push_back(pop());
	}
	if (_log->isDebug()) _log->debug(2, "~TxBufferManager::popAll()");
	return result;
}

TxBuffer* TxBufferManager::pop() {
	if (_log->isDebug()) _log->debug(2, "TxBuffer* TxBufferManager::pop()");

	if (_log->isDebug()) _log->debug(3, "_lockActiveBuffers->lock() %d", (long)_lockActiveBuffers);
	_lockActiveBuffers->lock();
	TxBuffer* buffer = _activeBuffers->front();
	_activeBuffers->pop();
	_vactiveBuffers->erase(_vactiveBuffers->begin());
	if (_log->isDebug()) _log->debug(3, "_lockActiveBuffers->unlock() %d", (long)_lockActiveBuffers);

	// removes the buffer from the bufferByLog
	std::map<std::string, int*>::iterator itBuffersByLog = _buffersByLog->find(buffer->fileName());
	if (itBuffersByLog != _buffersByLog->end()) {
		int* count = itBuffersByLog->second;
		(*count)--;
	}
	_buffersCount--;
	_lockActiveBuffers->unlock();

	if (_log->isDebug()) _log->debug(2, "~TxBuffer* TxBufferManager::pop()");
	return buffer;
}

__int32 TxBufferManager::buffersCount() const {
	return _buffersCount;
}

void TxBufferManager::startMonitor() {
	if (_log->isDebug()) _log->debug(2, "TxBufferManager::startMonitor()");
	_monitorThread = new Thread(&TxBufferManager::monitorBuffers);
	_runningMonitor = true;
	_monitorThread->start(this);
	if (_log->isDebug()) _log->debug(3, "_monitorThread->start(%d)", (long)this);
}

bool TxBufferManager::runningMonitor() const {
	return _runningMonitor;
}

void TxBufferManager::stopMonitor() {
	if (_runningMonitor) {
		if (_log->isDebug()) _log->debug(2, "x  TxBufferManager::stopMonitor()");

		_runningMonitor = false;
		// Sends a notification to release the inner wait
		//#ifdef DEBUG
		//	debugFlushBuffers();
		//#else
		_lockActiveBuffers->lock();
		if (_log->isDebug()) _log->debug(2, "x  TxBufferManager::stopMonitor() before notify");
		_lockActiveBuffers->notify();
		_lockActiveBuffers->unlock();
		// Release any thread that could be waiting for this lock
		_lockWait->lock();
		_lockWait->notify();
		_lockWait->unlock();
		if (_log->isDebug()) _log->debug(2, "x  TxBufferManager::stopMonitor() after notify");
		if (_monitorThread != NULL) _monitorThread->join();
		if (_log->isDebug()) _log->debug(2, "x  TxBufferManager::stopMonitor() after join");
		//#endif
	}
}

void* TxBufferManager::monitorBuffers(void* arg) {
	TxBufferManager* manager = (TxBufferManager*)arg;
	Logger* log = getLogger(NULL);
	if (log->isDebug()) log->debug(3, "monitorBuffers started");

	while (manager->runningMonitor()) {
		// Waiting for notifications on new buffers
		manager->flushBuffer();
	}
	if (log->isDebug()) log->debug(3, "~TxBufferManager::monitorBuffers(void* arg)");

	return NULL;
}

void TxBufferManager::flushBuffer() {
	// This achieves the Producer/Consumer pattern, just waits for elements to flush
	_lockWait->lock();
	_lockWait->wait(1);
	_flushingBuffers = true;
	if (buffersCount() > 1) {
		//if (_log->isDebug()) _log->debug(2, "TxBufferManager::flushBuffer()");
		TxBuffer* buffer = pop();

		//if (_log->isDebug()) _log->debug(2, "Buffer popped up. filename: %s, mainLog: %s", buffer->fileName().c_str(), buffer->mainLog()?"true": "false");

		//if (_log->isDebug()) _log->debug(3, "buffer->acquireLock();");
		buffer->acquireLock();
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
						free(id);
						free(revision);
						delete remove;
					};
					break;
				case TXO_DROPNAMESPACE:
					{
						_controller->dropNamespace(db, ns);
					};
					break;
			}
			free(db);
			free(ns);
			delete operation;
		}
		//if (_log->isDebug()) _log->debug(3, "buffer->releaseLock();");
		buffer->releaseLock();
		if (buffer->mainLog()) {
			buffer->reset();
			addReusable(buffer);
			registerBufferControlFile(buffer, false, 0x02);
		} else {
			dropBuffer(buffer);
		}
	}
	_flushingBuffers = false;
	_lockWait->unlock();
}

void TxBufferManager::dropAllBuffers() {
	std::vector<TxBuffer*> buffers = popAll();
	for (std::vector<TxBuffer*>::iterator it = buffers.begin(); it != buffers.end(); it++) {
		TxBuffer* buffer = *it;
		dropBuffer(buffer);
	}
}

void TxBufferManager::dropControlFile() {
	std::string fileName = _controlFile->fileName();

	_controlFile->close();

	if (_log->isDebug()) _log->debug(2, "Removing the control file: %s", fileName.c_str());

	if (!removeFile(fileName.c_str())) {
		_log->error("The file %s could not be dropped", fileName.c_str());
	}
}

void TxBufferManager::dropBuffer(TxBuffer* buffer) {
	if (_log->isDebug()) _log->debug(2, "dropBuffer(buffer:  fileName %s, mainLog %s)", buffer->fileName().c_str(), buffer->mainLog() ? "true": "false");

	if (!buffer->mainLog()) {
		std::map<std::string, int*>::iterator it = _buffersByLog->find(buffer->fileName());

		// If the file counter is zero it means there're no references to it and should be
		// deleted from disk (pe. commit), if the file is not referenced this will mean the
		// buffer comes from other buffermanager and should be removed.
		bool dropFile = false;
		if (it != _buffersByLog->end()) {
			int* count = it->second;
			if (*count <= 0) {
				dropFile = true;
				delete count;
				_buffersByLog->erase(it);
			}
		} else {
			dropFile = true;
		}
		if (dropFile) {
			std::string file = buffer->fileName();
			std::string datadir = getSetting("DATA_DIR");
			char* fullFilePath = combinePath(datadir.c_str(), file.c_str());
			if (_log->isDebug()) _log->debug(2, "removing the log file: %s", fullFilePath);
			if (existFile(fullFilePath)) {
				if (!removeFile(fullFilePath)) {
					_log->error("An error ocurred removing the file: %s. Error Number: %d, Error description: %s", fullFilePath, errno, strerror(errno));
				}
			}
			free(fullFilePath);
		}
	}
	delete buffer;
}

void TxBufferManager::writeOperationToRegister(const char* db, const char* ns, const TransactionOperation& operation) {
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
	if (_log->isDebug()) _log->debug(3, "buffer->acquireLock();");
	txBuffer->acquireLock();
	chrs = buffer.toChars();
	txBuffer->writeLong(bufferSize);
	txBuffer->writeChars(chrs, bufferSize);

	txBuffer->flush();

	// Saving the new buffer length
	__int64 lenPos = txBuffer->controlPosition() + sizeof(char) + sizeof(__int64);
	_controlFile->acquireLock();
	_controlFile->seek(lenPos);
	_controlFile->writeLong(txBuffer->bufferLength());
	_controlFile->releaseLock();
	if (_log->isDebug()) _log->debug(3, "buffer->releaseLock();");
	txBuffer->releaseLock();

	free(chrs);
}

TransactionOperation* TxBufferManager::readOperationFromRegister(TxBuffer* buffer) {
	return readOperationFromRegister(buffer, NULL, NULL);
}

/*
 * the will read an operation based on the parameters, 
 * the buffer indicates where is going to be readed
 * the parameters db and ns are used to filter which operations should be included in the result
 * if db is NULL then all the operations are going to be included
 * if ns is NULL all the operations within the db will be included
 * */
TransactionOperation* TxBufferManager::readOperationFromRegister(TxBuffer* buffer, char* db, char* ns) {
	if (buffer->eof()) {
		return NULL;
	}
	bool check = false;
	if (_log->isDebug()) _log->debug(3, "buffer->acquireLock();");
	buffer->acquireLock();
	__int64 size = buffer->readLong();

	char* fullBuffer = buffer->readChars();
	MemoryStream* stream = new MemoryStream(fullBuffer, size);
	if (_log->isDebug()) _log->debug(3, "buffer->releaseLock();");
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
	if (db == NULL) {
		check = true;
	} else if (strcmp(rdb, db) == 0) {
		if (ns == NULL) {
			check = true;
		} else if (strcmp(rns, ns) == 0) {
			check = true;
		}
	}
	if (check) {
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
									  char* revision = ms.readChars();
									  removeOper->revision = revision;
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
