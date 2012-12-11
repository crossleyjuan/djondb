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


const int MEMORY_BUFFER_SIZE = 1024;

MemoryStream::MemoryStream() {
    _open = true;
	 _currentBuffer = NULL;
	 _currentIndex = -1;
	 _buffer = NULL;
	 _length = 0;
	 _bufferSize = MEMORY_BUFFER_SIZE;
	 allocate(_bufferSize);
}

MemoryStream::MemoryStream(long bufferSize) {
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
	 for (int x = 0; x < _maxIndexes; x++) {
		 char* r = _buffer[x];
		 free(r);
	 }

}

void MemoryStream::allocate(size_t size) {
	if (_buffer == NULL) {
		_buffer = (char**)malloc(sizeof(char*) * 10);
	}
	_currentBuffer = (char*)malloc(_bufferSize + 1);
	memset(_currentBuffer, 0, _bufferSize + 1);

	_currentIndex++;
	if (_currentIndex > 9) {
		// Throw an exception
		assert(false);
	}
	_buffer[_currentIndex] = _currentBuffer;
	_currentBufferPos = 0;
	_maxIndexes = _currentIndex + 1;
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
	} else {
		int space = _bufferSize - _currentBufferPos;
		memcpy(_currentBuffer + _currentBufferPos, ptr, space);
		_length += space;
		int offset = space;
		space = count - space;
		allocate(_bufferSize);
		write(ptr + offset, space);
	}
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
		int space = _bufferSize - _currentBufferPos;
		memcpy(ptr, _currentBuffer + _currentBufferPos, space);
		readed += space;
		int offset = space;
		space = count - space;
		nextBuffer();
		readed += read(ptr + offset, space);
	}
	return readed;
}

/* Write 1 byte in the output */
void MemoryStream::writeChar (unsigned char v)
{
	write((char*)&v, 1);
}

/* Write 2 bytes in the output (little endian order) */
void MemoryStream::writeShortInt (short int v)
{
	writeData<short int>(v);
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

void MemoryStream::writeChars(const char *text, int len) {
	writeInt(len);
	write(text, len);
}

void MemoryStream::writeString(const std::string& text) {
	const char* c = text.c_str();
	int l = strlen(c);
	writeChars(c, l);
}

void MemoryStream::seek(long i) {
	if (i > _length) {
		i = _length;
	}
	_currentIndex = i % _bufferSize;
	_currentBuffer = _buffer[_currentIndex];
	_currentBufferPos = i - (_currentIndex * _bufferSize);
}

long MemoryStream::currentPos() const {
	return (_currentIndex * _bufferSize) + _currentBufferPos;
}

void MemoryStream::close() {
	_open = false;
}

void MemoryStream::flush() {
}

unsigned char MemoryStream::readChar() {
	unsigned char v = 0;
	read((char*)&v,  1);
	return v;
}

/* Reads 2 bytes in the input (little endian order) */
short int MemoryStream::readShortInt () {
	short int v = readData<short int>();
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

char* MemoryStream::readChars(int length) {
	char* res = (char*)malloc(length+1);
	memset(res, 0, length+1);
	read(res, length);
	return res;
}

const char* MemoryStream::readFull() {
	seek(0);
	std::stringstream ss;
	char buffer[1024];
	int readed = 0;
	while (!eof()) {
		memset(buffer, 0, 1024);
		readed = read(buffer, 1023);
		ss << buffer;
	}
	std::string str = ss.str();
	return strdup(str.c_str());
}

bool MemoryStream::eof() {
	if (_length < ((_currentIndex * _bufferSize) + _currentBufferPos)) {
		return false;
	} else {
		return true;
	}
}

bool MemoryStream::isClosed() {
	return !_open;
}

char* MemoryStream::toChars() {
	char* result = (char*)malloc(_length);

	long offset = 0;
	for (int x = 0; x < _maxIndexes - 1; x++) {
		char* b = _buffer[x];
		memcpy(result + offset, b, _bufferSize); 
		offset += _bufferSize;
	}
	memcpy(result + offset, _buffer[_maxIndexes - 1], _length - (_bufferSize * _maxIndexes));

	return result;
}
