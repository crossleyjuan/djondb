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

TxBuffer::TxBuffer(const TxBufferManager* manager, InputOutputStream* stream, __int64 offset, __int64 bufferLen) {
	_stream = stream;
	_startOffset = offset;
	_bufferLength = bufferLen;
};

TxBuffer::TxBuffer(const TxBuffer& other) {
	this->_stream = other._stream;
	this->_startOffset = other._startOffset;
	this->_manager = other._manager;
};

TxBuffer::~TxBuffer() {
};


unsigned char TxBuffer::readChar() {
	return _stream->readChar();
};

/* Reads 2 bytes in the input (little endian order) */
__int16 TxBuffer::readShortInt() {
	return _stream->readShortInt();
};

/* Reads 4 bytes in the input (little endian order) */
__int32 TxBuffer::readInt() {
	return _stream->readInt();
};

/* Reads 8 bytes in the input (little endian order) */
__int64 TxBuffer::readLong() {
	return _stream->readLong();
};

/* Reads a 16 byte long in the input */
__int64 TxBuffer::readLong64() {
	return _stream->readLong64();
};

/* Reads a 4 byte float in the input */
float TxBuffer::readFloatIEEE() {
	return _stream->readFloatIEEE();
};

/* Reads a 8 byte double in the input */
double TxBuffer::readDoubleIEEE() {
	return _stream->readDoubleIEEE();
};

/* Read a chars */
char* TxBuffer::readChars() {
	return _stream->readChars();
};

char* TxBuffer::readChars(__int32 length) {
	return _stream->readChars();
};

const char* TxBuffer::readFull() {
	return _stream->readFull();
};


std::string* TxBuffer::readString() {
	return _stream->readString();
};

void TxBuffer::writeChar(unsigned char v) {
	_stream->writeChar(v);
	_bufferLength += sizeof(unsigned char);
};

/* Write 2 bytes in the output (little endian order) */
void TxBuffer::writeShortInt(__int16 v) {
	_stream->writeShortInt(v);
	_bufferLength += sizeof(__int16);
};

/* Write 4 bytes in the output (little endian order) */
void TxBuffer::writeInt(__int32 v) {
	_stream->writeInt(v);
	_bufferLength += sizeof(__int32);
};

/* Write 4 bytes in the output (little endian order) */
void TxBuffer::writeLong(__int64 v) {
	_stream->writeLong(v);
	_bufferLength += sizeof(__int64);
};

/* Write a 4 byte float in the output */
void TxBuffer::writeFloatIEEE(float v) {
	_stream->writeFloatIEEE(v);
	_bufferLength += sizeof(float);
};

/* Write a 8 byte double in the output */
void TxBuffer::writeDoubleIEEE(double v) {
	_stream->writeDoubleIEEE(v);
	_bufferLength += sizeof(double);
};

/* Write a char */
void TxBuffer::writeChars(const char* text, __int32 len) {
	_stream->writeChars(text, len);
	_bufferLength = _stream->currentPos() - _startOffset;
};

void TxBuffer::writeString(const std::string& text) {
	_stream->writeString(text);
	_bufferLength = _stream->currentPos() - _startOffset;
};

void TxBuffer::seek(__int64 pos, SEEK_DIRECTION direction) {
	if (direction == FROMSTART_SEEK) {
		_stream->seek(_startOffset + pos, direction);
	} else {
		_stream->seek(pos, direction);
	}
};

__int64 TxBuffer::currentPos() const {
	__int64 pos = _stream->currentPos();
	return pos - _startOffset;
};

const std::string TxBuffer::fileName() const {
	return _stream->fileName();
};

bool TxBuffer::eof() {
	return (_bufferLength - currentPos()) == 0; 
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
