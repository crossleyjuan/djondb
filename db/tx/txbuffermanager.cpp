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
#include <stdlib.h>


TxBufferManager::TxBufferManager(const char* file) {
	_stream = NULL;
	_buffersSize = 64*1024*1024;
	_buffersCount = 0;
	_dataDir = getSetting("DATA_DIR");
	initialize(file);
}

void TxBufferManager::initialize(const char* file) {
	std::string controlFileName = _dataDir + FILESEPARATOR + std::string(file) + ".trc";
	std::string bufferFile = _dataDir + FILESEPARATOR + std::string(file) + ".log";

	bool existControl = existFile(controlFileName.c_str());

	char* flags;
	if (existControl) {
		flags = "rb+";
	} else {
		flags = "wb+";
	}
	_controlFile = (InputOutputStream*)new FileInputOutputStream(controlFileName.c_str(), flags); 
	_controlFile->seek(0);

	bool existLogFile = existFile(bufferFile.c_str());
	if (existLogFile) {
		flags = "rb+";
	} else {
		flags = "wb+";
	}

	if (existControl) {
		_buffersSize = _controlFile->readInt();
		
		loadBuffers(bufferFile.c_str());
	} else {
		_controlFile->writeLong(_buffersSize);
		__int64 pos = _controlFile->currentPos();
		_controlFile->writeInt(0);
		_controlFile->seek(pos);
		loadBuffers(bufferFile.c_str());
	}
}

void TxBufferManager::loadBuffers(const char* logFilePath) {
	openLogFile(logFilePath);

	__int32 buffers = _controlFile->readInt();

	for (__int32 x = 0; x < buffers; x++) {
		char flag = _controlFile->readChar();
		__int64 startOffset = _controlFile->readLong();
		__int64 bufferLen = _controlFile->readLong();
		TxBuffer* buffer = new TxBuffer(this, _stream, startOffset, bufferLen);

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
	_stream->close();
	delete _stream;
}

TxBuffer* TxBufferManager::createNewBuffer() {
	TxBuffer* result = new TxBuffer(this, _stream, _buffersCount * _buffersSize, (__int64)0);
	return result;
}

TxBuffer* TxBufferManager::getBuffer(__int32 minimumSize) {
	TxBuffer* result = NULL;
	if (!_activeBuffers.empty()) {
		result = _activeBuffers.back();
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
			addBuffer(result);
		} else {
			result = _reusableBuffers.front();
			_reusableBuffers.pop();
			_controlFile->seek(result->controlPosition());
		}
		_controlFile->writeChar((char)0x01);
		_controlFile->writeLong(result->startOffset());
		_controlFile->writeLong(0);
		_buffersCount++;
	}
	return result;
}

void TxBufferManager::openLogFile(const char* fileName) {
	bool existLogFile = existFile(fileName);
	char* flags = NULL;
	if (existLogFile) {
		flags = "rb+";
	} else {
		flags = "wb+";
	}
	_stream = new FileInputOutputStream(fileName, flags); 
}

void TxBufferManager::addBuffer(TxBuffer* buffer) {
	_activeBuffers.push(buffer);
	_vactiveBuffers.push_back(buffer);
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
