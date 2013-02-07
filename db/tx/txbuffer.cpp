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

TXBuffer::TXBuffer(const TXBufferManager* manager, const InputOuputStream* stream, __int64 offset) {
	_stream = stream;
	_startOffset = offset;
};

TXBuffer::TXBuffer(const TXBuffer& other) {
	this->_stream = other._stream;
	this->_startOffset = other._startOffset;
	this->_manager = other._manager;
};

TXBuffer::~TXBuffer() {
};


unsigned char TXBuffer::readChar() {
	return _input->readChar();
};

/* Reads 2 bytes in the input (little endian order) */
__int16 TXBuffer::readShortInt() {
	return _input->readShortInt();
};

/* Reads 4 bytes in the input (little endian order) */
__int32 TXBuffer::readInt() {
	return _input->readInt();
};

/* Reads 8 bytes in the input (little endian order) */
__int64 TXBuffer::readLong() {
	return _input->readLong();
};

/* Reads a 16 byte long in the input */
__int64 TXBuffer::readLong64() {
	return _input->readLong64();
};

/* Reads a 4 byte float in the input */
float TXBuffer::readFloatIEEE() {
	return _input->readFloatIEEE();
};

/* Reads a 8 byte double in the input */
double TXBuffer::readDoubleIEEE() {
	return _input->readDoubleIEEE();
};

/* Read a chars */
char* TXBuffer::readChars() {
	return _input->readChars();
};

char* TXBuffer::readChars(__int32 length) {
	return _input->readChars();
};

const char* TXBuffer::readFull() {
	return _input->readFull();
};


std::string* TXBuffer::readString() {
	return _input->readString();
};

void TXBuffer::writeChar(unsigned char v) {
	_input->writeChar(v);
};

/* Write 2 bytes in the output (little endian order) */
void TXBuffer::writeShortInt(__int16 v) {
	_input->writeShortInt(v);
};

/* Write 4 bytes in the output (little endian order) */
void TXBuffer::writeInt(__int32 v) {
	_input->writeInt(v);
};

/* Write 4 bytes in the output (little endian order) */
void TXBuffer::writeLong(__int64 v) {
	_input->writeLong(v);
};

/* Write a 4 byte float in the output */
void TXBuffer::writeFloatIEEE(float v) {
	_input->writeFloatIEEE(v);
};

/* Write a 8 byte double in the output */
void TXBuffer::writeDoubleIEEE(double v) {
	_input->writeDoubleIEEE(v);
};

/* Write a char */
void TXBuffer::writeChars(const char* text, __int32 len) {
	_input->writeChars(v);
};

void TXBuffer::writeString(const std::string& text) {
	_input->writeString(text);
};

void TXBuffer::seek(__int64 pos, SEEK_DIRECTION direction) {
	if (direction == FROMSTART_SEEK) {
		_stream->seek(_startOffset + i, direction);
	} else {
		_stream->seek(i, direction);
	}
};

__int64 TXBuffer::currentPos() const {
	__int64 pos = _stream->currentPos();
	return pos - _versionOffset;
};

const std::string TXBuffer::fileName() const {
	return _input->fileName();
};

bool TXBuffer::eof() {
	return _input->eof();
};

void TXBuffer::close() {
	_input->close();
};


void TXBuffer::flush() {
	_input->flush();
};


bool TXBuffer::isClosed() {
	return _input->isClosed();
};

