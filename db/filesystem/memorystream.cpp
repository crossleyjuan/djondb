// *********************************************************************************************************************
// file:
// author: Juan Pablo Crossley (crossleyjuan@gmail.com)
// created:
// updated:
// license:
// // This file is part of the djondb project, for license information please refer to the LICENSE file,
// the application and libraries are provided as-is and free of use under the terms explained in the file LICENSE
// Its authors create this application in order to make the world a better place to live, but you should use it on
// your own risks.
// 
// Also, be adviced that, the GPL license force the committers to ensure this application will be free of use, thus
// if you do any modification you will be required to provide it for free unless you use it for personal use (you may 
// charge yourself if you want), bare in mind that you will be required to provide a copy of the license terms that ensures
// this program will be open sourced and all its derivated work will be too.
// *********************************************************************************************************************

#include "memorystream.h"

#include "util.h"
#include <assert.h>
#include <string.h>
#include <boost/crc.hpp>
#include <stdlib.h>
#include <errno.h>
#include <sstream>
#include <limits.h>
#include <cmath>

const int MEMORY_BUFFER_SIZE = 1024;

MemoryStream::MemoryStream() {
    _open = true;
	 _currentBuffer = NULL;
	 _currentBufferPos = 0;
	 _currentIndex = -1;
	 _buffer = NULL;
	 _length = 0;
	 _bufferSize = MEMORY_BUFFER_SIZE;
	 _pos = 0;
	 _maxIndexes = 0;
	 allocate(_bufferSize);
}

MemoryStream::MemoryStream(char* buffer, __int32 len) {
    _open = true;
	 _currentBuffer = NULL;
	 _currentBufferPos = 0;
	 _currentIndex = -1;
	 _buffer = NULL;
	 _length = 0;
	 _bufferSize = MEMORY_BUFFER_SIZE;
	 _pos = 0;
	 _maxIndexes = 0;
	 allocate(_bufferSize);

	 write(buffer, len);
}

MemoryStream::MemoryStream(__int64 bufferSize) {
    _open = true;
	 _currentBuffer = NULL;
	 _currentIndex = -1;
	 _buffer = NULL;
	 _length = 0;
	 _bufferSize = bufferSize;
	 allocate(_bufferSize);
}

MemoryStream::~MemoryStream() {
    close();
	 for (__int32 x = 0; x < _maxIndexes; x++) {
		 char* r = _buffer[x];
		 free(r);
	 }
	 free(_buffer);
}

void MemoryStream::allocate(size_t size) {
	if (_buffer == NULL) {
		_buffer = (char**)malloc(sizeof(char*) * 100);
		for (int x = 0; x < 100; x++) {
			_buffer[x] = NULL;
		}
	}
	_currentIndex++;
	if (_currentIndex >= 100) {
		// Throw an exception
		assert(false);
	}
	_currentBuffer = _buffer[_currentIndex];
	if (_buffer[_currentIndex] == NULL) {
		_currentBuffer = (char*)malloc(_bufferSize + 1);
		memset(_currentBuffer, 0, _bufferSize + 1);
		_buffer[_currentIndex] = _currentBuffer;
		_maxIndexes = _currentIndex + 1;
	}

	_currentBufferPos = 0;
}

void MemoryStream::nextBuffer() {
	_currentIndex++;
	_currentBufferPos = 0;
	_currentBuffer = _buffer[_currentIndex];
}

void MemoryStream::write(const char *ptr, size_t count) {
	if ((_bufferSize - (_currentBufferPos + (int)count)) > 0) {
		memcpy(_currentBuffer + _currentBufferPos, ptr, count);
		_currentBufferPos += count;
		_length += count;
		count = 0;
	} else {
		__int64 space = _bufferSize - _currentBufferPos;
		memcpy(_currentBuffer + _currentBufferPos, ptr, space);
		_length += space;
		__int64 offset = space;
		space = count - space;
		allocate(_bufferSize);
		write(ptr + offset, space);
	}
}

void MemoryStream::writeRaw(const char *ptr, size_t count) {
	write(ptr, count);
}

size_t MemoryStream::read( char* ptr, size_t count) {
	if (count > (_length - _currentBufferPos)) {
		count = _length - _currentBufferPos;
	}
	size_t readed = 0;
	if ((_currentBufferPos + count) < _bufferSize) {
		memcpy(ptr, _currentBuffer + _currentBufferPos, count);
		_currentBufferPos += count;
		readed += count;
	} else {
		__int64 space = _bufferSize - _currentBufferPos;
		memcpy(ptr, _currentBuffer + _currentBufferPos, space);
		readed += space;
		__int64 offset = space;
		space = count - space;
		nextBuffer();
		readed += read(ptr + offset, space);
	}
	return readed;
}

const size_t MemoryStream::readRaw(char* ptr, size_t count) {
	return read(ptr, count);
}

/* Write 1 byte in the output */
void MemoryStream::writeChar (unsigned char v)
{
	write((char*)&v, 1);
}

/* Write 1 bytes in the output (little endian order) */
void MemoryStream::writeBoolean (bool v)
{
	writeData<char>((char)v);
}

/* Write 2 bytes in the output (little endian order) */
void MemoryStream::writeShortInt (__int16 v)
{
	writeData<__int16>(v);
}

/* Write 4 bytes in the output (little endian order) */
void MemoryStream::writeInt (__int32 v)
{
	writeData<__int32>(v);
}

/* Write 4 bytes in the output (little endian order) */
void MemoryStream::writeLong (__int64 v)
{
	writeData<__int64>(v);
}

/* Write a 4 byte float in the output */
void MemoryStream::writeFloatIEEE (float v)
{
	write((char*)&v, sizeof(v));
}

/* Write a 8 byte double in the output */
void MemoryStream::writeDoubleIEEE (double v)
{
	write((char*)&v, sizeof(v));
}

void MemoryStream::writeChars(const char *text, __int32 len) {
	writeInt(len);
	write(text, len);
}

void MemoryStream::writeString(const std::string& text) {
	const char* c = text.c_str();
	__int32 l = strlen(c);
	writeChars(c, l);
}

void MemoryStream::seek(__int64 i, SEEK_DIRECTION direction) {
	if (direction == FROMSTART_SEEK) {
		if (i > _length) {
			i = _length;
		}
		_currentIndex = (int)i / (int)_bufferSize;
		_currentBuffer = _buffer[_currentIndex];
		_currentBufferPos = i % _bufferSize;
	} else {
		_currentIndex = ceil((double)_length / (double)_bufferSize);
		_currentBuffer = _buffer[_currentIndex];
		_currentBufferPos = _length - (_length % _bufferSize);
	}
}

__int64 MemoryStream::currentPos() const {
	return (_currentIndex * _bufferSize) + _currentBufferPos;
}

void MemoryStream::close() {
	_open = false;
}

void MemoryStream::flush() {
}

unsigned char MemoryStream::readChar() {
	unsigned char v = 0;
	int count = 1;
	if (_length < _currentBufferPos) {
		count = 0;
	}
	if ((_currentBufferPos + count) < _bufferSize) {
		v = *(_currentBuffer + _currentBufferPos);
		_currentBufferPos += 1;
	} else {
		nextBuffer();
		v = readChar();
	}
	return v;
}

/* Reads 2 bytes in the input (little endian order) */
bool MemoryStream::readBoolean () {
	bool v = (bool)readData<char>();
	return v;
}

/* Reads 2 bytes in the input (little endian order) */
__int16 MemoryStream::readShortInt () {
	__int16 v = readData<__int16>();
	return v;
}

/* Reads 4 bytes in the input (little endian order) */
__int32 MemoryStream::readInt () {
	__int32 v = readData<__int32>();

	return v;
}

/* Reads 4 bytes in the input (little endian order) */
__int64 MemoryStream::readLong () {
	return readData<__int64>();
}

/* Reads 16 bytes in the input (little endian order) */
__int64 MemoryStream::readLong64() {
	return readData<__int64>();
}

/* Reads a 4 byte float in the input */
float MemoryStream::readFloatIEEE () {
	float f;
	read((char*)&f, sizeof(f));
	return f;
}

/* Reads a 8 byte double in the input */
double MemoryStream::readDoubleIEEE () {
	double d;
	read((char*)&d, sizeof(d));
	return d;
}

/* Read a chars */
char* MemoryStream::readChars() {
	__int32 len = readInt();
	char* res = readChars(len);
	return res;
}

std::string* MemoryStream::readString() {
	char* c = readChars();
	std::string* res = new std::string(c);
	free(c);
	return res;
}

char* MemoryStream::readChars(__int32 length) {
	char* res = (char*)malloc(length+1);
	memset(res, 0, length+1);
	read(res, length);
	return res;
}

char* MemoryStream::readFull() {
	return toChars();
}

bool MemoryStream::eof() {
	if (_length > ((_currentIndex * _bufferSize) + _currentBufferPos)) {
		return false;
	} else {
		return true;
	}
}

bool MemoryStream::isClosed() {
	return !_open;
}

char* MemoryStream::toChars() {
	char* result = (char*)malloc(_length + 1);
	memset(result, 0, _length + 1);

	__int64 offset = 0;
	int maxCurrentIndexes = ceil((double)_length / (double)_bufferSize);
	for (__int32 x = 0; x < maxCurrentIndexes - 1; x++) {
		char* b = _buffer[x];
		memcpy(result + offset, b, _bufferSize); 
		offset += _bufferSize;
	}
	memcpy(result + offset, _buffer[maxCurrentIndexes - 1], _length - (_bufferSize * (maxCurrentIndexes - 1)));

	return result;
}

__int64 MemoryStream::size() const {
	return _length;
}

void MemoryStream::reset() {
	_length = 0;
	seek(0);
}
