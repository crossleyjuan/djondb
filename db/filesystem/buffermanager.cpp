/*
 * =====================================================================================
 *
 *       Filename:  buffermanager.cpp
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
#include "buffermanager.h"
#include "fileinputoutputstream.h"
#include "memorystream.h"
#include "lock.h"
#include "util.h"
#include <string.h>
#include <stdlib.h>
#include <errno.h>

__int64 DEFAULT_BUFFER_SIZE = pageSize() * 20;

BufferManager::BufferManager(const char* file) {
	_buffersSize = DEFAULT_BUFFER_SIZE;
	_buffersCount = 0;
	_dataDir = getSetting("DATA_DIR");
	_lockActiveBuffers = new Lock();
	_lockWait = new Lock();
	_flushingBuffers = false;
	_activeBuffers = new std::queue<Buffer*>();
	_vactiveBuffers = new std::vector<Buffer*>();
	_reusableBuffers = new std::queue<Buffer*>();
	_buffersByLog = new std::map<std::string, int*>();
	_log = getLogger(NULL);

	initialize(file);
}

void BufferManager::initialize(const char* file) {
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

void BufferManager::loadBuffers() {
	openLogFile();

	__int32 buffers = _controlFile->readInt();

	// The buffers are circular, this means that some
	// buffers are in the front of the control file but
	// they are at the tail of the queue
	std::vector<Buffer*> front;
	std::vector<Buffer*> tail;
	bool addToTail = true;
	for (__int32 x = 0; x < buffers; x++) {
		__int32 controlPosition = _controlFile->currentPos();
		char flag = _controlFile->readChar();
		__int64 startOffset = _controlFile->readLong();
		__int64 bufferLen = _controlFile->readLong();
		char* logFileName = _controlFile->readChars();
		Buffer* buffer = new Buffer(this, logFileName, startOffset, _buffersSize, _buffersSize / pageSize());
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
	for (std::vector<Buffer*>::iterator iter = front.begin(); iter != front.end(); iter++) {
		Buffer* buffer = *iter;
		addBuffer(buffer);
	}
	for (std::vector<Buffer*>::iterator iter = tail.begin(); iter != tail.end(); iter++) {
		Buffer* buffer = *iter;
		addBuffer(buffer);
	}
}

BufferManager::~BufferManager() {
	_log->debug(2, "BufferManager::~BufferManager()");

	/*
		while (!_activeBuffers->empty()) {
		Buffer* buffer =  _activeBuffers->front();
		_activeBuffers->pop();
		buffer->close();
		delete buffer;
		}
		while (!_reusableBuffers->empty()) {
		Buffer* buffer = _reusableBuffers->front();
		_reusableBuffers->pop();
		buffer->close();
		delete buffer;
		}
		*/
	_controlFile->close();
	delete _controlFile;
	delete _lockActiveBuffers;
	// Releases the waits
	_lockWait->notify();
	delete _lockWait;
	delete _activeBuffers;
	delete _vactiveBuffers;
	delete _reusableBuffers;
	for (std::map<std::string, int*>::iterator i = _buffersByLog->begin(); i != _buffersByLog->end(); i++) {
		int* count = i->second;
		delete count;
	}
	delete _buffersByLog;
	free(_logFileName);
}

Buffer* BufferManager::createNewBuffer() {
	Buffer* result = new Buffer(this, _logFileName, _buffersCount * _buffersSize, (__int64)0, _buffersSize / pageSize());
	return result;
}

void BufferManager::registerBufferControlFile(Buffer* buffer, bool newBuffer, int flag) {
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
	if (_log->isDebug()) _log->debug(3, "_controlFile->releaseLock();");
	_controlFile->releaseLock();
}

Buffer* BufferManager::getBuffer(__int32 index) {
	if ((index < 0) || (index >= buffersCount())) {
		return NULL;
	} else {
		return _vactiveBuffers->at(index);
	}
}

Buffer* BufferManager::getCurrentBuffer(__int32 minimumSize) {
	return getCurrentBuffer(minimumSize, false);
}

Buffer* BufferManager::getCurrentBuffer(__int32 minimumSize, bool forceNewBuffer) {
	Buffer* result = NULL;
	if (!_activeBuffers->empty()) {
		result = _activeBuffers->back();
		// because some other action could change the currentPos
		// we should ensure its in the right place before returning the buffer
		result->seek(result->currentPos());
	}
	if ((forceNewBuffer) || (result == NULL) 
			|| (result->spaceLeft() < minimumSize)) {

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

void BufferManager::openLogFile() {
	bool existLogFile = existFile(_logFileName);
	char* flags = NULL;
	if (existLogFile) {
		flags = "rb+";
	} else {
		flags = "wb+";
	}
}

void BufferManager::addBuffer(Buffer* buffer) {
	if (_log->isDebug()) _log->debug(2, "BufferManager::addBuffer()");

	if (_log->isDebug()) _log->debug(2, "_lockActiveBuffers->lock() %d", (long)_lockActiveBuffers);
	_lockActiveBuffers->lock();
	_activeBuffers->push(buffer);
	_vactiveBuffers->push_back(buffer);
	buffer->setBufferIndex(_buffersCount);
	_buffersCount++;
	if (_log->isDebug()) _log->debug(2, "_lockActiveBuffers->unlock() %d", (long)_lockActiveBuffers);
	_lockWait->notify();
	_lockActiveBuffers->unlock();
}

#ifdef DEBUG
void BufferManager::debugFlushBuffers() {
	if (_log->isDebug()) {
		_log->debug(2, "Buffers to be flushed");
		for (std::vector<Buffer*>::iterator i = _vactiveBuffers->begin(); i != _vactiveBuffers->end(); i++) {
			Buffer* buffer = *i;
			if (_log->isDebug()) _log->debug(2, "   Buffer fileName: %s", buffer->fileName().c_str());
		}
	}
	if (_log->isDebug()) _log->debug(2, "debugFlushBuffers buffersCount: %d", buffersCount());
	while (buffersCount() > 1) {
		flushBuffer();
	}
}
#endif

void BufferManager::addBuffers(std::vector<Buffer*> buffers) {
	if (_log->isDebug()) _log->debug(2, "BufferManager::addBuffers()");

	if (_log->isDebug()) _log->debug(3, "_lockActiveBuffers->lock() %d", (long)_lockActiveBuffers);
	_lockActiveBuffers->lock();
	for (std::vector<Buffer*>::iterator it = buffers.begin(); it != buffers.end(); it++) {
		Buffer* buffer = *it;
		if (_log->isDebug()) _log->debug(2, "Adding buffer. fileName: %s", buffer->fileName().c_str());
		_buffersCount++;
		registerBufferControlFile(buffer, true, 0x01);
		_activeBuffers->push(buffer);
		_vactiveBuffers->push_back(buffer);
	}
	if (_log->isDebug()) _log->debug(3, "_lockActiveBuffers->unlock() %d", (long)_lockActiveBuffers);
	_lockActiveBuffers->notify();
	_lockActiveBuffers->unlock();
	// this will force a new buffer to be put
	getCurrentBuffer(0, true);
	if (_log->isDebug()) _log->debug("buffersCount: %d", buffersCount());
}

void BufferManager::addReusable(Buffer* buffer) {
	_reusableBuffers->push(buffer);
}

const std::vector<Buffer*>* BufferManager::getActiveBuffers() const {
	return _vactiveBuffers;
}

std::vector<Buffer*> BufferManager::popAll() {
	if (_log->isDebug()) _log->debug(2, "BufferManager::popAll()");
	std::vector<Buffer*> result;
	while (buffersCount() > 0) {
		result.push_back(pop());
	}
	if (_log->isDebug()) _log->debug(2, "~BufferManager::popAll()");
	return result;
}

Buffer* BufferManager::pop() {
	if (_log->isDebug()) _log->debug(2, "Buffer* BufferManager::pop()");

	if (_log->isDebug()) _log->debug(3, "_lockActiveBuffers->lock() %d", (long)_lockActiveBuffers);
	_lockActiveBuffers->lock();
	Buffer* buffer = _activeBuffers->front();
	_activeBuffers->pop();
	_vactiveBuffers->erase(_vactiveBuffers->begin());
	if (_log->isDebug()) _log->debug(3, "_lockActiveBuffers->unlock() %d", (long)_lockActiveBuffers);

	// removes the buffer from the bufferByLog
	std::map<std::string, int*>::iterator itBuffersByLog = _buffersByLog->find(buffer->fileName());
	if (itBuffersByLog != _buffersByLog->end()) {
		int* count = itBuffersByLog->second;
		(*count)--;
	}
	_lockActiveBuffers->unlock();

	__int64 controlPos = buffer->controlPosition();
	if (_log->isDebug()) _log->debug(3, "_controlFile->acquireLock();");
	_controlFile->acquireLock();
	_controlFile->seek(controlPos);
	_controlFile->writeChar((char)0x02); // reusable
	_buffersCount--;
	if (_log->isDebug()) _log->debug(3, "_controlFile->releaseLock();");
	_controlFile->releaseLock();

	if (_log->isDebug()) _log->debug(2, "~Buffer* BufferManager::pop()");
	return buffer;
}

__int32 BufferManager::buffersCount() const {
	return _buffersCount;
}

void BufferManager::dropAllBuffers() {
	std::vector<Buffer*> buffers = popAll();
	for (std::vector<Buffer*>::iterator it = buffers.begin(); it != buffers.end(); it++) {
		Buffer* buffer = *it;
		dropBuffer(buffer);
	}
}

void BufferManager::dropControlFile() {
	std::string fileName = _controlFile->fileName();

	_controlFile->close();

	if (_log->isDebug()) _log->debug(2, "Removing the control file: %s", fileName.c_str());

	if (!removeFile(fileName.c_str())) {
		_log->error("The file %s could not be dropped", fileName.c_str());
	}
}

void BufferManager::dropBuffer(Buffer* buffer) {
	if (_log->isDebug()) _log->debug(2, "dropBuffer(buffer:  fileName %s)", buffer->fileName().c_str());

	// Removes the file if the buffermanager does not have more buffers of the same file
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
	delete buffer;
}

