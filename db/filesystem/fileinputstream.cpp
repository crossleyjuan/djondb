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

#include "fileinputstream.h"

#include "util.h" 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sstream>
#include <iostream>
#include <boost/crc.hpp>
#include <limits.h>

FileInputStream::FileInputStream(const char* fileName, const char* flags)
{
    _pFile = fopen(fileName, flags);
    _fileName = fileName;
    _open = true;
}

FileInputStream::~FileInputStream() {
    close();
}

unsigned char FileInputStream::readChar() {
    unsigned char v;
    fread(&v, 1, 1, _pFile);
    return v;
}

/* Reads 2 bytes in the input (little endian order) */
short int FileInputStream::readShortInt () {
	short int result = readData<short int>();
	return result;
}

/* Reads 4 bytes in the input (little endian order) */
int FileInputStream::readInt () {
	int result = readData<int>();
	return result;
}

/* Reads 4 bytes in the input (little endian order) */
long FileInputStream::readLong () {
	return readData<long>();
}

/* Reads 4 bytes in the input (little endian order) */
__LONG64 FileInputStream::readLong64 () {
	__LONG64 result = readData<__LONG64>();
	return result;
}

/* Reads a 4 byte float in the input */
float FileInputStream::readFloatIEEE () {
	float f;
	fread(&f, 1, sizeof(f), _pFile);
	return f;
}

/* Reads a 8 byte double in the input */
double FileInputStream::readDoubleIEEE () {
	double d;
	fread(&d, 1, sizeof(d), _pFile);
	return d;
}

/* Read a chars */
char* FileInputStream::readChars() {
	int len = readInt();
	char* res = readChars(len);
	return res;
}

std::string* FileInputStream::readString() {
	char* c = readChars();
	std::string* res = new std::string(c);
	free(c);
	return res;
}

const std::string FileInputStream::fileName() const {
	return _fileName;
}

char* FileInputStream::readChars(int length) {
	char* res = (char*)malloc(length+1);
	memset(res, 0, length+1);
	fread(res, 1, length, _pFile);
	return res;
}

const char* FileInputStream::readFull() {
	fseek(_pFile, 0, SEEK_SET);
	std::stringstream ss;
	char buffer[1024];
	int readed = 0;
	while (!feof(_pFile)) {
		memset(buffer, 0, 1024);
		readed = fread(buffer, 1, 1023, _pFile);
		ss << buffer;
	}
	std::string str = ss.str();
	return strdup(str.c_str());
}

bool FileInputStream::eof() {
	if (_pFile == NULL) {
		return true;
	}
	long pos = currentPos();
	// Force reading the last char to check the feof flag
	readChar();
	bool res = feof(_pFile);
	// Back to the original position
	seek(pos);
	return res;
}

long FileInputStream::currentPos() const {
	return ftell(_pFile);
}

void FileInputStream::seek(long i) {
	fseek(_pFile, i, SEEK_SET);
}

long FileInputStream::crc32() {
	long pos = currentPos();
	fseek(_pFile, 0, SEEK_END);
	int bufferSize = currentPos();
	bufferSize -= pos;
	seek(pos);

	char* buffer = (char*)malloc(bufferSize+1);
	memset(buffer, 0, bufferSize + 1);
	fread(buffer, 1, bufferSize, _pFile);

	boost::crc_32_type crc;
	crc.process_bytes(buffer, bufferSize);
	long result = crc.checksum();

	// back to the original position
	seek(pos);

	free(buffer);
	return result;
}

void FileInputStream::close() {
	if (_pFile) {
		fclose(_pFile);
		_pFile = 0;
		_open = false;
	}
}

bool FileInputStream::isClosed() {
	return !_open;
}
