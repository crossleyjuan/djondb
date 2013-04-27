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
#include <stdlib.h>
#include "txbuffermanager.h"
#include "mmapinputoutputstream.h"
#include "lock.h"

TxBuffer::TxBuffer(const TxBufferManager* manager, const char* file, __int64 offset, __int64 bufferLen, __int64 maxLen, bool mainLog) {
	std::string path = getSetting("DATA_DIR");
	char* fileName = combinePath(path.c_str(), file);
	_stream = new MMapInputOutputStream(fileName, offset, maxLen);
	_startOffset = offset;
	_bufferLength = bufferLen;
	_maxLength = maxLen;
	_mainLog = mainLog;
	_currentPos = 0;
	_stream->seek(0);
	_lock = new Lock();
	_fileName = new std::string(file);
	free(fileName);
};

TxBuffer::TxBuffer(const TxBuffer& other) {
	this->_stream = other._stream;
	this->_startOffset = other._startOffset;
	this->_manager = other._manager;
	this->_bufferLength = other._bufferLength;
	this->_currentPos = other._currentPos;
	this->_lock = other._lock;
	seek(_currentPos);
};

TxBuffer::~TxBuffer() {
	_stream->close();
	delete _stream;
	delete _lock;
	delete _fileName;
};

void TxBuffer::reset() {
	_bufferLength = 0;
	seek(0);
}

unsigned char TxBuffer::readChar() {
	char c = _stream->readChar();
	_currentPos += sizeof(char);
	return c;
};

/* Reads 2 bytes in the input (little endian order) */
__int16 TxBuffer::readShortInt() {
	__int16 r = _stream->readShortInt();
	_currentPos += sizeof(__int16);
	return r;
};

/* Reads 4 bytes in the input (little endian order) */
__int32 TxBuffer::readInt() {
	__int32 r = _stream->readInt();
	_currentPos += sizeof(__int32);
	return r;
};

/* Reads 8 bytes in the input (little endian order) */
__int64 TxBuffer::readLong() {
	__int64 r =  _stream->readLong();
	_currentPos += sizeof(__int64);
	return r;
};

/* Reads a 16 byte long in the input */
__int64 TxBuffer::readLong64() {
	__int64 r = _stream->readLong64();
	_currentPos += sizeof(__int64);
	return r;
};

/* Reads a 4 byte float in the input */
float TxBuffer::readFloatIEEE() {
	float r = _stream->readFloatIEEE();
	_currentPos += sizeof(float);
	return r;
};

/* Reads a 8 byte double in the input */
double TxBuffer::readDoubleIEEE() {
	double r = _stream->readDoubleIEEE();
	_currentPos += sizeof(double);
	return r;
};

/* Read a chars */
char* TxBuffer::readChars() {
	char* r = _stream->readChars();
	_currentPos = _stream->currentPos();
	return r;
};

char* TxBuffer::readChars(__int32 length) {
	char* r = _stream->readChars(length);
	_currentPos = _stream->currentPos();
	return r;
};

const char* TxBuffer::readFull() {
	const char* r = _stream->readFull();
	_currentPos = _stream->currentPos();
	return r;
};


std::string* TxBuffer::readString() {
	std::string* r = _stream->readString();
	_currentPos = _stream->currentPos();
	return r;
};

void TxBuffer::writeChar(unsigned char v) {
	_stream->writeChar(v);
	_bufferLength += sizeof(unsigned char);
	_currentPos = _bufferLength;
};

/* Write 2 bytes in the output (little endian order) */
void TxBuffer::writeShortInt(__int16 v) {
	_stream->writeShortInt(v);
	_bufferLength += sizeof(__int16);
	_currentPos = _bufferLength;
};

/* Write 4 bytes in the output (little endian order) */
void TxBuffer::writeInt(__int32 v) {
	_stream->writeInt(v);
	_bufferLength += sizeof(__int32);
	_currentPos = _bufferLength;
};

/* Write 4 bytes in the output (little endian order) */
void TxBuffer::writeLong(__int64 v) {
	_stream->writeLong(v);
	_bufferLength += sizeof(__int64);
	_currentPos = _bufferLength;
};

/* Write a 4 byte float in the output */
void TxBuffer::writeFloatIEEE(float v) {
	_stream->writeFloatIEEE(v);
	_bufferLength += sizeof(float);
	_currentPos = _bufferLength;
};

/* Write a 8 byte double in the output */
void TxBuffer::writeDoubleIEEE(double v) {
	_stream->writeDoubleIEEE(v);
	_bufferLength += sizeof(double);
	_currentPos = _bufferLength;
};

/* Write a char */
void TxBuffer::writeChars(const char* text, __int32 len) {
	_stream->writeChars(text, len);
	_bufferLength = _stream->currentPos();
	_currentPos = _bufferLength;
};

void TxBuffer::writeString(const std::string& text) {
	_stream->writeString(text);
	_bufferLength = _stream->currentPos();
	_currentPos = _bufferLength;
};

void TxBuffer::seek(__int64 pos, SEEK_DIRECTION direction) {
	if (direction == FROMSTART_SEEK) {
		_stream->seek(_startOffset + pos, direction);
		_currentPos = pos;
	} else {
		_stream->seek(pos, direction);
		_currentPos = _stream->currentPos();
	}
};

__int64 TxBuffer::currentPos() const {
	return _currentPos;
};

const std::string TxBuffer::fileName() const {
	return *_fileName;
};

bool TxBuffer::eof() {
	return (_bufferLength - _currentPos) == 0; 
};

void TxBuffer::close() {
	_stream->close();
};


void TxBuffer::flush() {
	_stream->flush();
};

bool TxBuffer::isClosed() {
	return _stream->isClosed();
};

__int32 TxBuffer::controlPosition() const {
	return _controlPosition;
}

void TxBuffer::setControlPosition(__int32 pos) {
	_controlPosition = pos;
}

__int64 TxBuffer::startOffset() const {
	return _startOffset;
}

__int64 TxBuffer::bufferLength() const {
	return _bufferLength;
}

void TxBuffer::acquireLock() {
	_lock->lock();
	_stream->seek(_currentPos);
}

void TxBuffer::releaseLock() {
	_lock->unlock();
}

