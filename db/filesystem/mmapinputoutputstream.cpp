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

#include "mmapinputoutputstream.h"

#include "util.h" 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sstream>
#include <iostream>
#include <boost/crc.hpp>
#include <limits.h>
#include <errno.h>
#include <assert.h>

#ifndef WINDOWS
#include <sys/mman.h>
#include <sys/uio.h>
#include <unistd.h>
#include <fcntl.h>
#else
#include <strsafe.h>
#include <windows.h>
#endif

char* getLastErrorMessage() {
#ifndef WINDOWS
	char* error = strerror(errno);
#else
	LPTSTR lpMsgBuf;
	DWORD dw = GetLastError();
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, dw, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR) &lpMsgBuf, 0, NULL );
	char* error = strcpy((char*)lpMsgBuf, strlen((char*)lpMsgBuf));
	LocalFree(lpMsgBuf);
#endif
	return error;
}


MMapInputOutputStream::MMapInputOutputStream(const char* fileName, __int64 offset, __int32 pages)
{
	Logger* log = getLogger(NULL);
	_fileName = fileName;
	_open = false;
	_pos = 0;
	_len = 0;
	 _initaddr = NULL;
	 _addr = NULL;
#ifndef WINDOWS
	 if (existFile(fileName)) {
		 _pFile = ::open(fileName, O_RDWR);
	 } else {
		 _pFile = ::open(fileName, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
	 }
	 if (_pFile > -1) {
		 _len = fileSize(fileName);
		 long psize = pageSize();
		 __int64 maxLen = (pages * psize) + offset;
		 if (_len < maxLen) {
			 _len = maxLen;
		 }
		 lseek(_pFile, _len, SEEK_SET);
		 write(_pFile, "", 1);
		 lseek(_pFile, 0, SEEK_SET);

		 bool shared = true;
		 bool advise = true;
#ifdef LINUX
		 _addr = reinterpret_cast<char *>(mmap(NULL, _len, PROT_READ | PROT_WRITE, MAP_FILE | (shared?MAP_SHARED:MAP_PRIVATE) | MAP_POPULATE , _pFile, offset));
#else
		 _addr = reinterpret_cast<char *>(mmap(NULL, _len, PROT_READ | PROT_WRITE, MAP_FILE | (shared?MAP_SHARED:MAP_PRIVATE), _pFile, offset));
#endif
		 _initaddr = _addr;    
		 if (_addr == MAP_FAILED) {
			 log->error("Error mapping the file: %s. errno: %d", fileName, errno);
			 exit(1);
		 }
		 if(advise)
			 if(madvise(_addr,_len,MADV_SEQUENTIAL|MADV_WILLNEED)!=0)  {
				 log->error("Error mapping the file: %s, couldn't set hints, errno: %d", fileName, errno, errno);
				 exit(1);
			 }
		 ::close(_pFile);
		 _pFile = 0;
	 } else {
		 char* error = strerror(errno);
		 log->error("An error ocurred opening the file: %s, error: %s", error);
		 exit(1);
	 }
#else
	 // Create the test file. Open it "Create Always" to overwrite any
	 // existing file. The data is re-created below
	if (!existFile(fileName)) {
		 _pFile = CreateFile(fileName,
				 GENERIC_WRITE,
				 FILE_SHARE_READ | FILE_SHARE_WRITE,
				 NULL,
				 CREATE_NEW,
				 FILE_ATTRIBUTE_NORMAL,
				 NULL);

		long psize = pageSize();
		__int64 maxLen = (pages * psize) + offset;
		 SetFilePointer(_pFile, maxLen, NULL, FILE_BEGIN);
		 int newPos;
		 DWORD dwWritten;
		 WriteFile(_pFile, "a", 1, &dwWritten, NULL);

		 CloseHandle(_pFile);
	 }

	_pFile = CreateFile(fileName,
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL);

	_len = fileSize(fileName);
	long psize = pageSize();
	__int64 maxLen = (pages * psize) + offset;
	if (_len < maxLen) {
		 SetFilePointer(_pFile, maxLen, NULL, FILE_BEGIN);
		 DWORD dwWritten;
		 WriteFile(_pFile, "a", 1, &dwWritten, NULL);

		_len = maxLen;
	}

    DWORD offsetHigh = ((offset >> 32) & 0xFFFFFFFF);
    DWORD offsetLow = (offset & 0xFFFFFFFF);

    DWORD lenHigh = ((_len >> 32) & 0xFFFFFFFF);
    DWORD lenLow = (_len & 0xFFFFFFFF);

	if (_pFile == INVALID_HANDLE_VALUE)
	 {
		char* error = getLastErrorMessage();
		log->error(error);
		free(error);
		exit(1);
	 }

	 // Create a file mapping object for the file
	 // Note that it is a good idea to ensure the file size is not zero
	 _hMapFile = CreateFileMapping( _pFile,          // current file handle
			 NULL,           // default security
			 PAGE_READWRITE, // read/write permission
			 lenHigh,              // size of mapping object, high
			 lenLow,  // size of mapping object, low
			 NULL);          // name of mapping object

	 if (_hMapFile == NULL)
	 {
		char* error = getLastErrorMessage();
		log->error(error);
		free(error);
		exit(1);
	 }

	 // Map the view and test the results.
	 LPVOID lpMapAddress = MapViewOfFile(_hMapFile,            // handle to
			 // mapping object
			 FILE_MAP_ALL_ACCESS, //FILE_MAP_READ, // read/write
			 offsetHigh,                   // high-order 32 bits of file offset
			 offsetLow ,      // low-order 32 bits of file offset
			 (pages * psize));      // number of bytes
	 // to map
	 if (lpMapAddress == NULL)
	 {
		char* error = getLastErrorMessage();
		log->error(error);
		free(error);
		exit(1);
	 }

	 // Calculate the pointer to the data.
	 _addr = (char*)lpMapAddress;
	 _initaddr = _addr;
	
#endif
	 _open = true;
}

MMapInputOutputStream::~MMapInputOutputStream() {
	close();
}

unsigned char MMapInputOutputStream::readChar() {
	unsigned char v;
	v = *_addr;
	_addr++;
	_pos++;
	return v;
}

/* Reads 2 bytes in the input (little endian order) */
__int16 MMapInputOutputStream::readShortInt () {
	__int16 result = readData<__int16>();
	return result;
}

/* Reads 4 bytes in the input (little endian order) */
__int32 MMapInputOutputStream::readInt () {
	__int32 result = readData<__int32>();
	return result;
}

/* Reads 4 bytes in the input (little endian order) */
__int64 MMapInputOutputStream::readLong () {
	return readData<__int64>();
}

/* Reads 16 bytes in the input (little endian order) */
__int64 MMapInputOutputStream::readLong64() {
	return readData<__int64>();
}

/* Reads a 4 byte float in the input */
float MMapInputOutputStream::readFloatIEEE () {
	float f;
	f = *_addr;
	_addr += sizeof(float);
	_pos += sizeof(float);

	return f;
}

/* Reads a 8 byte double in the input */
double MMapInputOutputStream::readDoubleIEEE () {
	double d;
	d = *_addr;
	_addr += sizeof(double);
	_pos += sizeof(double);

	return d;
}

/* Read a chars */
char* MMapInputOutputStream::readChars() {
	__int32 len = readInt();
	char* res = readChars(len);
	return res;
}

std::string* MMapInputOutputStream::readString() {
	char* c = readChars();
	std::string* res = new std::string(c);
	free(c);
	return res;
}

const std::string MMapInputOutputStream::fileName() const {
	return _fileName;
}

void MMapInputOutputStream::readChars(__int32 length, char* res) {
	memset(res, 0, length+1);
	read(res, length);
}

char* MMapInputOutputStream::readChars(__int32 length) {
	char* res = (char*)malloc(length+1);
	memset(res, 0, length+1);
	read(res, length);
	return res;
}

const char* MMapInputOutputStream::readFull() {
	_addr = _initaddr;
	std::stringstream ss;
	char buffer[1024];
	__int32 readed = 0;
	throw "unsupported yet";
	return NULL;
}

void MMapInputOutputStream::read(char* dest, int len) {
	memcpy(dest, _addr, len);
	_addr += len;
	_pos += len;
}

bool MMapInputOutputStream::eof() {
	if (_pos >= _len) {
		return true;
	}
	return false;
}

__int64 MMapInputOutputStream::currentPos() const {
	return _pos;
}

void MMapInputOutputStream::seek(__int64 i, SEEK_DIRECTION direction) {
	Logger* log = getLogger(NULL);
	if (direction == FROMSTART_SEEK) {
		_addr = _initaddr + i;
		_pos = i;
	} else {
		log->error(std::string("Unsupported direction in Memory Mapped Files")); 
		assert(false);
	}
}

__int64 MMapInputOutputStream::crc32() {
	return 0;
}

void MMapInputOutputStream::close() {
	if (_open) {
#ifndef WINDOWS
		munmap(_initaddr, _len);
		_pFile = 0;
#else
		if (!UnmapViewOfFile((LPVOID)_initaddr)) {
			Logger* log = getLogger(NULL);
			log->error("An error ocurred unmapping the file: %d", GetLastError());
		}
		CloseHandle(_hMapFile);
		CloseHandle(_pFile);
#endif
		_open = false;
	}
}

bool MMapInputOutputStream::isClosed() {
	return !_open;
}

char* MMapInputOutputStream::pointer() {
	return _addr;
}

__int64 MMapInputOutputStream::length() const {
	return _len;
}

void MMapInputOutputStream::writeChar (unsigned char v) {
	memcpy(_addr, (char*)&v, 1);
	_addr++;
	_pos++;
	_len++;
}

void MMapInputOutputStream::checkAvailableSpace() {
	if ((_len - _pos) < 64*1024) {
		long psize = pageSize();
		__int32 newLength = _len + (psize*10*1024*1024);
		resize(newLength);
	}
}

void MMapInputOutputStream::resize(__int32 len) {
#ifndef WINDOWS
	void* p = mremap (_addr, _len, len, MREMAP_MAYMOVE);
	if (p == MAP_FAILED)
		_addr = NULL;
	else
		_addr = (char*)p;
	_len = len;
#else
	assert(false);
#endif
}

/* Write 2 bytes in the output (little endian order) */
void MMapInputOutputStream::writeShortInt (__int16 v) {
	writeData<__int16>(v);
}

/* Write 4 bytes in the output (little endian order) */
void MMapInputOutputStream::writeInt (__int32 v) {
	writeData<__int32>(v);
}

/* Write 4 bytes in the output (little endian order) */
void MMapInputOutputStream::writeLong (__int64 v) {
	writeData<__int64>(v);
}

/* Write a 4 byte float in the output */
void MMapInputOutputStream::writeFloatIEEE (float v) {
	checkAvailableSpace();
	memcpy(_addr, (char*)&v, sizeof(float));
	_addr += sizeof(float);
	_pos += sizeof(float);
	_len += sizeof(float);
}

/* Write a 8 byte double in the output */
void MMapInputOutputStream::writeDoubleIEEE (double v) {
	checkAvailableSpace();
	memcpy(_addr, (char*)&v, sizeof(double));
	_addr += sizeof(double);
	_pos += sizeof(double);
	_len += sizeof(double);
}

/* Write a char */
void MMapInputOutputStream::writeChars(const char* text, __int32 len) {
	writeData<__int32>(len);
	memcpy(_addr, text, len);
	_addr += len;
	_len += len;
	_pos += len;
}

void MMapInputOutputStream::writeString(const std::string& text) {
	writeChars(text.c_str(), text.length());
}

void MMapInputOutputStream::flush() {
}
