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

#include "fileinputoutputstream.h"

#include "util.h"
#include <string.h>
#include <boost/crc.hpp>
#include <stdlib.h>
#include <errno.h>
#include <sstream>
#include <limits.h>
#include <assert.h>

FileInputOutputStream::FileInputOutputStream(const std::string& fileName, const char* flags) {
	Logger* log = getLogger(NULL);
#ifndef WINDOWS
    _pFile = fopen(fileName.c_str(), flags);
	 setvbuf (_pFile, NULL , _IOFBF , 1024*4 ); // large buffer
    fseek(_pFile, 0, SEEK_END);
#else
	if (existFile(fileName.c_str())) {
		_pFile = CreateFile(fileName.c_str(),                // name of the write
			GENERIC_WRITE | GENERIC_READ,          // open for writing
			FILE_SHARE_READ | FILE_SHARE_WRITE,        // do not share
			NULL,                   // default security
			OPEN_EXISTING,             // create new file only
			FILE_ATTRIBUTE_NORMAL,  // normal file
			NULL);                  // no attr. template
	} else {
		_pFile = CreateFile(fileName.c_str(),                // name of the write
			GENERIC_WRITE | GENERIC_READ,          // open for writing
			FILE_SHARE_READ | FILE_SHARE_WRITE,        // do not share
			NULL,                   // default security
			CREATE_NEW,             // create new file only
			FILE_ATTRIBUTE_NORMAL,  // normal file
                       NULL);                  // no attr. template
	}
    if (_pFile == INVALID_HANDLE_VALUE) 
    { 
		assert(false);
    }
	LARGE_INTEGER liOffset = {0};
	LARGE_INTEGER liPos = {0};

	bool res = SetFilePointerEx(_pFile, liOffset, NULL, FILE_END);

	if (res == 0) {
		cout << GetLastError() << endl;
		exit(1);
	}
#endif

    _fileName = fileName;
    _open = true;
	 delete log;
}

FileInputOutputStream::~FileInputOutputStream() {
    close();
}

__int64 FileInputOutputStream::read(char* buffer, __int32 len) {
	int readed = 0;
#ifndef WINDOWS
	readed = fread(buffer, 1, len, _pFile);
#else
	DWORD dwreaded = 0;
	bool res = ReadFile(_pFile, buffer, len, &dwreaded, NULL);
	if ((dwreaded == 0) && res) {
		_eof = true;
	} else {
		_eof = false;
	}
	readed = (__int64)dwreaded;
#endif
	return readed;
}

void FileInputOutputStream::write(char* buffer, __int32 len) {
#ifndef WINDOWS
	readed = fwrite(buffer, 1, len, _pFile);
#else
	DWORD numberOfBytesWritten;
	WriteFile(_pFile, buffer, len, &numberOfBytesWritten, NULL);
#endif
}


/* Write 1 byte in the output */
void FileInputOutputStream::writeChar (unsigned char v)
{
	write((char*)&v, 1);
}

/* Write 2 bytes in the output (little endian order) */
void FileInputOutputStream::writeShortInt (short int v)
{
	writeData<short int>(v);
}

/* Write 4 bytes in the output (little endian order) */
void FileInputOutputStream::writeInt (__int32 v)
{
	writeData<__int32>(v);
}

/* Write 4 bytes in the output (little endian order) */
void FileInputOutputStream::writeLong (__int64 v)
{
	writeData<__int64>(v);
}

/* Write a 4 byte float in the output */
void FileInputOutputStream::writeFloatIEEE (float v)
{
	write((char*)&v, sizeof(v));
}

/* Write a 8 byte double in the output */
void FileInputOutputStream::writeDoubleIEEE (double v)
{
	write((char*)&v, sizeof(v));
}

void FileInputOutputStream::writeChars(const char *text, __int32 len) {
    writeInt(len);
	write(const_cast<char*>(text), len);
}

void FileInputOutputStream::writeString(const std::string& text) {
    const char* c = text.c_str();
    __int32 l = strlen(c);
    writeChars(c, l);
}

void FileInputOutputStream::seek(__int64 i) {
#ifndef WINDOWS
    fflush(_pFile);
	fseek(_pFile, i, SEEK_SET);
#else
	flush();
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

__int64 FileInputOutputStream::currentPos() const {
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

void FileInputOutputStream::close() {
    if (_pFile) {
        flush();
#ifndef WINDOWS
        fclose(_pFile);
        _pFile = 0;
#else
		CloseHandle(_pFile);
#endif
        _open = false;
    }
}

void FileInputOutputStream::flush() {
#ifndef WINDOWS
    fflush(_pFile);
#else
	FlushFileBuffers(_pFile);
#endif
}

const std::string FileInputOutputStream::fileName() const {
    return _fileName;
}

unsigned char FileInputOutputStream::readChar() {
    unsigned char v = 0;
	read((char*)&v, 1);
    return v;
}

/* Reads 2 bytes in the input (little endian order) */
short int FileInputOutputStream::readShortInt () {
	__int32 v = readData<short int>();
	return v;
	/*
    int v = readChar() | readChar() << 8;
    return v;
	 */
}

/* Reads 4 bytes in the input (little endian order) */
__int32 FileInputOutputStream::readInt () {
	__int32 v = readData<__int32>();
	return v;
}

/* Reads 4 bytes in the input (little endian order) */
__int64 FileInputOutputStream::readLong () {
	return readData<__int64>();
}

/* Reads a 4 byte float in the input */
float FileInputOutputStream::readFloatIEEE () {
    float f;
	read((char*)&f, sizeof(f));
    return f;
}

/* Reads a 8 byte double in the input */
double FileInputOutputStream::readDoubleIEEE () {
    double d;
	read((char*)&d, sizeof(d));
    return d;
}

/* Read a chars */
char* FileInputOutputStream::readChars() {
    __int32 len = readInt();
    char* res = readChars(len);
    return res;
}

std::string* FileInputOutputStream::readString() {
    char* c = readChars();
    std::string* res = new std::string(c);
    free(c);
    return res;
}

char* FileInputOutputStream::readChars(__int32 length) {
    char* res = (char*)malloc(length+1);
    memset(res, 0, length+1);
	read(res, length);
    return res;
}

const char* FileInputOutputStream::readFull() {
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

bool FileInputOutputStream::eof() {
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

bool FileInputOutputStream::isClosed() {
    return !_open;
}
