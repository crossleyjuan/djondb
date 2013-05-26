/*
 * =====================================================================================
 *
 *       Filename:  buffermanager.cpp
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
#include "buffermanager.h"
#include <stdlib.h>
#include "buffermanager.h"
#include "mmapinputoutputstream.h"
#include "lock.h"
#include <assert.h>

Buffer::Buffer(const BufferManager* manager, const char* file, __int64 offset, __int64 bufferLen, __int64 maxLen) {
	std::string path = getSetting("DATA_DIR");
	char* fileName = combinePath(path.c_str(), file);
	_stream = new MMapInputOutputStream(fileName, offset, maxLen);
	_startOffset = offset;
	_bufferLength = bufferLen;
	_maxLength = maxLen;
	_currentPos = 0;
	_stream->seek(0);
	_lock = new Lock();
	_fileName = new std::string(file);
	free(fileName);
};

Buffer::Buffer(const Buffer& other) {
	this->_stream = other._stream;
	this->_startOffset = other._startOffset;
	this->_manager = other._manager;
	this->_bufferLength = other._bufferLength;
	this->_currentPos = other._currentPos;
	this->_lock = other._lock;
	seek(_currentPos);
};

Buffer::~Buffer() {
	_stream->close();
	delete _stream;
	delete _lock;
	delete _fileName;
};

void Buffer::reset() {
	_bufferLength = 0;
	seek(0);
}

unsigned char Buffer::readChar() {
	char c = _stream->readChar();
	_currentPos += sizeof(char);
	return c;
};

/* Reads 1 bytes in the input (little endian order) */
bool Buffer::readBoolean() {
	bool r = (char)_stream->readChar();
	_currentPos += sizeof(char);
	return r;
};

/* Reads 2 bytes in the input (little endian order) */
__int16 Buffer::readShortInt() {
	__int16 r = _stream->readShortInt();
	_currentPos += sizeof(__int16);
	return r;
};

/* Reads 4 bytes in the input (little endian order) */
__int32 Buffer::readInt() {
	__int32 r = _stream->readInt();
	_currentPos += sizeof(__int32);
	return r;
};

/* Reads 8 bytes in the input (little endian order) */
__int64 Buffer::readLong() {
	__int64 r =  _stream->readLong();
	_currentPos += sizeof(__int64);
	return r;
};

/* Reads a 16 byte long in the input */
__int64 Buffer::readLong64() {
	__int64 r = _stream->readLong64();
	_currentPos += sizeof(__int64);
	return r;
};

/* Reads a 4 byte float in the input */
float Buffer::readFloatIEEE() {
	float r = _stream->readFloatIEEE();
	_currentPos += sizeof(float);
	return r;
};

/* Reads a 8 byte double in the input */
double Buffer::readDoubleIEEE() {
	double r = _stream->readDoubleIEEE();
	_currentPos += sizeof(double);
	return r;
};

/* Read a chars */
char* Buffer::readChars() {
	char* r = _stream->readChars();
	_currentPos = _stream->currentPos();
	return r;
};

__int32 Buffer::spaceLeft() const {
	return _maxLength - _bufferLength;
};

char* Buffer::readChars(__int32 length) {
	char* r = _stream->readChars(length);
	_currentPos = _stream->currentPos();
	return r;
};

const char* Buffer::readFull() {
	const char* r = _stream->readFull();
	_currentPos = _stream->currentPos();
	return r;
};


std::string* Buffer::readString() {
	std::string* r = _stream->readString();
	_currentPos = _stream->currentPos();
	return r;
};

void Buffer::writeChar(unsigned char v) {
	_stream->writeChar(v);
	_bufferLength += sizeof(unsigned char);
	_currentPos =+ sizeof(unsigned char);
};

/* Write 2 bytes in the output (little endian order) */
void Buffer::writeBoolean(bool v) {
	_stream->writeChar((char)v);
	_bufferLength += sizeof(char);
	_currentPos += sizeof(char);
};

/* Write 2 bytes in the output (little endian order) */
void Buffer::writeShortInt(__int16 v) {
	_stream->writeShortInt(v);
	_bufferLength += sizeof(__int16);
	_currentPos += sizeof(__int16);
};

/* Write 4 bytes in the output (little endian order) */
void Buffer::writeInt(__int32 v) {
	_stream->writeInt(v);
	_bufferLength += sizeof(__int32);
	_currentPos += sizeof(__int32);
};

/* Write 4 bytes in the output (little endian order) */
void Buffer::writeLong(__int64 v) {
	_stream->writeLong(v);
	_bufferLength += sizeof(__int64);
	_currentPos += sizeof(__int64);
};

/* Write a 4 byte float in the output */
void Buffer::writeFloatIEEE(float v) {
	_stream->writeFloatIEEE(v);
	_bufferLength += sizeof(float);
	_currentPos += sizeof(float);
};

/* Write a 8 byte double in the output */
void Buffer::writeDoubleIEEE(double v) {
	_stream->writeDoubleIEEE(v);
	_bufferLength += sizeof(double);
	_currentPos += sizeof(double);
};

/* Write a char */
void Buffer::writeChars(const char* text, __int32 len) {
	__int64 pos = _stream->currentPos();
	_stream->writeChars(text, len);
	__int32 written = _stream->currentPos();
	_bufferLength += written;
	_currentPos += written;
};

void Buffer::writeString(const std::string& text) {
	__int64 pos = _stream->currentPos();
	_stream->writeString(text);
	__int32 written = _stream->currentPos();
	_bufferLength += written;
	_currentPos += written;
};

void Buffer::seek(__int64 pos, SEEK_DIRECTION direction) {
	if (direction == FROMSTART_SEEK) {
		_stream->seek(pos, direction);
		_currentPos = pos;
	} else {
		if (direction == FROMEND_SEEK) {
			_stream->seek(_bufferLength - pos);
		} else {
			_stream->seek(pos);
		}
		_currentPos = _stream->currentPos();
	}
};

__int64 Buffer::currentPos() const {
	return _currentPos;
};

const std::string Buffer::fileName() const {
	return *_fileName;
};

bool Buffer::eof() {
	return (_bufferLength - _currentPos) <= 0; 
};

void Buffer::close() {
	_stream->close();
};


void Buffer::flush() {
	_stream->flush();
};

bool Buffer::isClosed() {
	return _stream->isClosed();
};

__int32 Buffer::controlPosition() const {
	return _controlPosition;
}

void Buffer::setControlPosition(__int32 pos) {
	_controlPosition = pos;
}

__int64 Buffer::startOffset() const {
	return _startOffset;
}

__int64 Buffer::bufferLength() const {
	return _bufferLength;
}

void Buffer::acquireLock() {
	_lock->lock();
	_stream->seek(_currentPos);
}

void Buffer::releaseLock() {
	_lock->unlock();
}

__int32 Buffer::bufferIndex() const {
	return _bufferIndex;
}

void Buffer::setBufferIndex(__int32 index) {
	_bufferIndex = index;
}
