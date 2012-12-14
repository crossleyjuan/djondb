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
#include <assert.h>

FileInputStream::FileInputStream(const char* fileName, const char* flags)
{
#ifndef WINDOWS
	_pFile = fopen(fileName, flags);
	if (_pFile) {
		setvbuf (_pFile, NULL , _IOFBF , 1024*4 ); // large buffer
	}
#else
	if (existFile(fileName)) {
		_pFile = CreateFile(fileName,                // name of the write
			GENERIC_READ,          // open for writing
			FILE_SHARE_READ,        // do not share
			NULL,                   // default security
			OPEN_EXISTING,             // create new file only
			FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS,  // normal file
			NULL);                  // no attr. template
	} else {
		_pFile = CreateFile(fileName,                // name of the write
			GENERIC_READ | GENERIC_WRITE,          // open for writing
			FILE_SHARE_READ,        // do not share
			NULL,                   // default security
			CREATE_NEW,             // create new file only
			FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS,  // normal file
                       NULL);                  // no attr. template
	}
    if (_pFile == INVALID_HANDLE_VALUE) 
    { 
		assert(false);
    }
#endif
	_fileName = fileName;
	_open = true;
}

FileInputStream::~FileInputStream() {
	close();
}

__int64 FileInputStream::read(char* buffer, __int32 len) {
	int readed = 0;
#ifndef WINDOWS
	readed = fread(buffer, 1, len, _pFile);
#else
	DWORD dwreaded = 0;
	bool res = ReadFile(_pFile, buffer, len, &dwreaded, NULL);
	if (!res) {
		cout << GetLastError() << endl;
		exit(1);
	}
	if ((dwreaded == 0) && res) {
		_eof = true;
	} else {
		_eof = false;
	}
	readed = (__int64)dwreaded;
#endif
	return readed;
}

unsigned char FileInputStream::readChar() {
	unsigned char v;
	read((char*)&v, 1);
	return v;
}

/* Reads 2 bytes in the input (little endian order) */
short int FileInputStream::readShortInt () {
	short int result = readData<short int>();
	return result;
}

/* Reads 4 bytes in the input (little endian order) */
__int32 FileInputStream::readInt () {
	__int32 result = readData<__int32>();
	return result;
}

/* Reads 4 bytes in the input (little endian order) */
__int64 FileInputStream::readLong () {
	return readData<__int64>();
}

/* Reads a 4 byte float in the input */
float FileInputStream::readFloatIEEE () {
	float f;
	read((char*)&f, sizeof(f));
	return f;
}

/* Reads a 8 byte double in the input */
double FileInputStream::readDoubleIEEE () {
	double d;
	read((char*)&d, sizeof(d));
	return d;
}

/* Read a chars */
char* FileInputStream::readChars() {
	__int32 len = readInt();
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

void FileInputStream::readChars(__int32 length, char* res) {
	memset(res, 0, length+1);
	read(res, length);
}

char* FileInputStream::readChars(__int32 length) {
	char* res = (char*)malloc(length+1);
	memset(res, 0, length+1);
	read(res, length);
	return res;
}

const char* FileInputStream::readFull() {
	seek(0);
	std::stringstream ss;
	char buffer[1024];
	__int32 readed = 0;
	while (!eof()) {
		memset(buffer, 0, 1024);
		readed = read(buffer, 1023);
		ss << buffer;
	}
	std::string str = ss.str();
	return strdup(str.c_str());
}

bool FileInputStream::eof() {
	if (_pFile == NULL) {
		return true;
	}
	__int64 pos = currentPos();
	// Force reading the last char to check the feof flag
	readChar();
#ifndef WINDOWS
	bool res = feof(_pFile);
#else
	bool res = _eof;
#endif
	// Back to the original position
	seek(pos);
	return res;
}

__int64 FileInputStream::currentPos() const {
#ifndef WINDOWS
	return ftell(_pFile);
#else
	LARGE_INTEGER liOffset = {0};
	LARGE_INTEGER liPos = {0};

	bool res = SetFilePointerEx(_pFile, liOffset, &liPos, FILE_CURRENT);

	if (res == 0) {
		cout << GetLastError() << endl;
		exit(1);
	}
	return (__int64)liPos.QuadPart;
#endif
}

void FileInputStream::seek(__int64 i) {
#ifndef WINDOWS
	fseek(_pFile, i, SEEK_SET);
#else
	LARGE_INTEGER liOffset = {0};
	liOffset.QuadPart = i;
	LARGE_INTEGER liPos = {0};

	bool res = SetFilePointerEx(_pFile, liOffset, NULL, FILE_BEGIN);

	if (res == 0) {
		cout << GetLastError() << endl;
		exit(1);
	}
#endif
}

__int64 FileInputStream::crc32() {
	__int64 pos = currentPos();
#ifndef WINDOWS
	fseek(_pFile, 0, SEEK_END);
#else
	SetFilePointer(_pFile, 0, NULL, FILE_END);
#endif
	__int64 bufferSize = currentPos();
	bufferSize -= pos;
	seek(pos);

	char* buffer = (char*)malloc(bufferSize+1);
	memset(buffer, 0, bufferSize + 1);
	read(buffer, bufferSize);

	boost::crc_32_type crc;
	crc.process_bytes(buffer, bufferSize);
	__int64 result = crc.checksum();

	// back to the original position
	seek(pos);

	free(buffer);
	return result;
}

void FileInputStream::close() {
	if (_open) {
#ifndef WINDOWS
		fclose(_pFile);
		_pFile = 0;
#else
		CloseHandle(_pFile);
#endif
		_open = false;
	}
}

bool FileInputStream::isClosed() {
	return !_open;
}
