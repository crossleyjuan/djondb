// *********************************************************************************************************************
// file:
// author: Juan Pablo Crossley (crossleyjuan@gmail.com)
// created:
// updated:
// license:
// 
// This file is part of the djondb project, for license information please refer to the LICENSE file,
// the application and libraries are provided as-is and free of use under the terms explained in the file LICENSE
// Its authors create this application in order to make the world a better place to live, but you should use it on
// your own risks.
// 
// Also, be adviced that, the GPL license force the committers to ensure this application will be free of use, thus
// if you do any modification you will be required to provide it for free unless you use it for personal use (you may 
// charge yourself if you want), bare in mind that you will be required to provide a copy of the license terms that ensures
// this program will be open sourced and all its derivated work will be too.
// *********************************************************************************************************************

#include "fileinputstreamw32.h"

#include "util.h" 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sstream>
#include <iostream>
#include <boost/crc.hpp>
#include <limits.h>

FileInputStreamW32::FileInputStreamW32(const char* fileName, const char* flags)
{
    _pFile = fopen(fileName, flags);
    _fileName = fileName;
    _open = true;
}

FileInputStreamW32::~FileInputStreamW32() {
    close();
}

unsigned char FileInputStreamW32::readChar() {
    unsigned char v;
    fread(&v, 1, 1, _pFile);
    return v;
}

/* Reads 1 bytes in the input (little endian order) */
bool FileInputStreamW32::readBoolean () {
	bool result = (bool)readDataTmp<char>();
	return result;
}

/* Reads 2 bytes in the input (little endian order) */
__int16 FileInputStreamW32::readShortInt () {
	__int16 result = readDataTmp<__int16>();
	return result;
}

/* Reads 4 bytes in the input (little endian order) */
__int32 FileInputStreamW32::readInt () {
	int result = readDataTmp<int>();
	return result;
}

/* Reads 4 bytes in the input (little endian order) */
__int64 FileInputStreamW32::readLong () {
	// This is the only required change to migrate the databases in windows
	// version 0.120121125 or older
	return readDataTmp<long>();
}

/* Reads 16 bytes in the input (little endian order) */
__int64 FileInputStreamW32::readLong64() {
	return readDataTmp<__int64>();
}

/* Reads a 4 byte float in the input */
float FileInputStreamW32::readFloatIEEE () {
	float f;
	fread(&f, 1, sizeof(f), _pFile);
	return f;
}

/* Reads a 8 byte double in the input */
double FileInputStreamW32::readDoubleIEEE () {
	double d;
	fread(&d, 1, sizeof(d), _pFile);
	return d;
}

/* Read a chars */
char* FileInputStreamW32::readChars() {
	__int32 len = readInt();
	char* res = readChars(len);
	return res;
}

std::string* FileInputStreamW32::readString() {
	char* c = readChars();
	std::string* res = new std::string(c);
	free(c);
	return res;
}

const std::string FileInputStreamW32::fileName() const {
	return _fileName;
}

char* FileInputStreamW32::readChars(__int32 length) {
	char* res = (char*)malloc(length+1);
	memset(res, 0, length+1);
	fread(res, 1, length, _pFile);
	return res;
}

const char* FileInputStreamW32::readFull() {
	fseek(_pFile, 0, SEEK_SET);
	std::stringstream ss;
	char buffer[1024];
	__int32 readed = 0;
	while (!feof(_pFile)) {
		memset(buffer, 0, 1024);
		readed = fread(buffer, 1, 1023, _pFile);
		ss << buffer;
	}
	std::string str = ss.str();
	return strdup(str.c_str());
}

bool FileInputStreamW32::eof() {
	if (_pFile == NULL) {
		return true;
	}
	__int64 pos = currentPos();
	// Force reading the last char to check the feof flag
	readChar();
	bool res = feof(_pFile);
	// Back to the original position
	seek(pos);
	return res;
}

__int64 FileInputStreamW32::currentPos() const {
	return ftell(_pFile);
}

void FileInputStreamW32::seek(__int64 i, SEEK_DIRECTION direction) {
	if (direction == FROMSTART_SEEK) {
		fseek(_pFile, i, SEEK_SET);
	} else {
		fseek(_pFile, i, SEEK_END);
	}
}

__int64 FileInputStreamW32::crc32() {
	__int64 pos = currentPos();
	fseek(_pFile, 0, SEEK_END);
	__int64 bufferSize = currentPos();
	bufferSize -= pos;
	seek(pos);

	char* buffer = (char*)malloc(bufferSize+1);
	memset(buffer, 0, bufferSize + 1);
	fread(buffer, 1, bufferSize, _pFile);

	boost::crc_32_type crc;
	crc.process_bytes(buffer, bufferSize);
	__int64 result = crc.checksum();

	// back to the original position
	seek(pos);

	free(buffer);
	return result;
}

void FileInputStreamW32::close() {
	if (_pFile) {
		fclose(_pFile);
		_pFile = 0;
		_open = false;
	}
}

bool FileInputStreamW32::isClosed() {
	return !_open;
}
