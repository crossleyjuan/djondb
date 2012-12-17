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

#include "mmapinputstream.h"

#include "util.h" 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sstream>
#include <iostream>
#include <boost/crc.hpp>
#include <limits.h>
#include <errno.h>

#ifndef WINDOWS
#include <sys/mman.h>
#include <sys/uio.h>
#include <unistd.h>
#include <fcntl.h>
#else
#endif

MMapInputStream::MMapInputStream(const char* fileName, const char* flags)
{
	Logger* log = getLogger(NULL);
	_fileName = fileName;
	_open = false;
	_pos = 0;
	_len = 0;
	 _initaddr = NULL;
	 _addr = NULL;
#ifndef WINDOWS
	_pFile = ::open(fileName, O_RDONLY);
	if (_pFile > -1) {
		_len = fileSize(fileName);
		bool shared = false;
		bool advise = true;
#ifdef LINUX
		_addr = reinterpret_cast<char *>(mmap(NULL, _len, PROT_READ, MAP_FILE | (shared?MAP_SHARED:MAP_PRIVATE) | MAP_POPULATE , _pFile, 0));
#else
		_addr = reinterpret_cast<char *>(mmap(NULL, _len, PROT_READ, MAP_FILE | (shared?MAP_SHARED:MAP_PRIVATE), _pFile, 0));
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
		close();
	}
#else
		// Create the test file. Open it "Create Always" to overwrite any
		// existing file. The data is re-created below
		_pFile = CreateFile(fileName,
				GENERIC_READ,
				FILE_SHARE_READ | FILE_SHARE_WRITE,
				NULL,
				OPEN_EXISTING,
				FILE_ATTRIBUTE_NORMAL,
				NULL);

		if (_pFile == INVALID_HANDLE_VALUE)
		{
			DWORD error = GetLastError();
		}

		SYSTEM_INFO SysInfo;  // system information; used to get granularity
		// Get the system allocation granularity.
		GetSystemInfo(&SysInfo);
		DWORD dwSysGran = SysInfo.dwAllocationGranularity;

		// Now calculate a few variables. Calculate the file offsets as
		// 64-bit values, and then get the low-order 32 bits for the
		// function calls.

		// To calculate where to start the file mapping, round down the
		// offset of the data into the file to the nearest multiple of the
		// system allocation granularity.
		DWORD dwFileMapStart; // where to start the file map view
		dwFileMapStart = (0 / dwSysGran) * dwSysGran;

		_len = GetFileSize(_pFile,  NULL);

		// Create a file mapping object for the file
		// Note that it is a good idea to ensure the file size is not zero
		HANDLE hMapFile = CreateFileMapping( _pFile,          // current file handle
				NULL,           // default security
				PAGE_READONLY, // read/write permission
				0,              // size of mapping object, high
				0,  // size of mapping object, low
				NULL);          // name of mapping object

		if (hMapFile == NULL)
		{
			log->error("Error mapping the file: %s. errno: %d", fileName, GetLastError());
			exit(1);
		}

		// Map the view and test the results.
		LPVOID lpMapAddress = MapViewOfFile(hMapFile,            // handle to
				// mapping object
				FILE_MAP_READ, // read/write
				0,                   // high-order 32
				// bits of file
				// offset
				0,      // low-order 32
				// bits of file
				// offset
				_len);      // number of bytes
		// to map
		if (lpMapAddress == NULL)
		{
			log->error("Error mapping the file: %s. errno: %d", fileName, GetLastError());
			exit(1);
		}

		// Calculate the pointer to the data.
		_addr = (char*)lpMapAddress;
		_initaddr = _addr;
#endif
		_open = true;
}

MMapInputStream::~MMapInputStream() {
}

unsigned char MMapInputStream::readChar() {
	unsigned char v;
	v = *_addr;
	_addr++;
	_pos++;
	return v;
}

/* Reads 2 bytes in the input (little endian order) */
short int MMapInputStream::readShortInt () {
	short int result = readData<short int>();
	return result;
}

/* Reads 4 bytes in the input (little endian order) */
__int32 MMapInputStream::readInt () {
	__int32 result = readData<__int32>();
	return result;
}

/* Reads 4 bytes in the input (little endian order) */
__int64 MMapInputStream::readLong () {
	return readData<__int64>();
}

/* Reads 16 bytes in the input (little endian order) */
__int64 MMapInputStream::readLong64() {
	return readData<__int64>();
}

/* Reads a 4 byte float in the input */
float MMapInputStream::readFloatIEEE () {
	float f;
	read((char*)&f, sizeof(f));
	return f;
}

/* Reads a 8 byte double in the input */
double MMapInputStream::readDoubleIEEE () {
	double d;
	read((char*)&d, sizeof(d));
	return d;
}

/* Read a chars */
char* MMapInputStream::readChars() {
	__int32 len = readInt();
	char* res = readChars(len);
	return res;
}

std::string* MMapInputStream::readString() {
	char* c = readChars();
	std::string* res = new std::string(c);
	free(c);
	return res;
}

const std::string MMapInputStream::fileName() const {
	return _fileName;
}

void MMapInputStream::readChars(__int32 length, char* res) {
	memset(res, 0, length+1);
	read(res, length);
}

char* MMapInputStream::readChars(__int32 length) {
	char* res = (char*)malloc(length+1);
	memset(res, 0, length+1);
	read(res, length);
	return res;
}

const char* MMapInputStream::readFull() {
	_addr = _initaddr;
	std::stringstream ss;
	char buffer[1024];
	__int32 readed = 0;
	throw "unsupported yet";
	return NULL;
}

void MMapInputStream::read(char* dest, int len) {
	memcpy(dest, _addr, len);
	_addr += len;
	_pos += len;
}

bool MMapInputStream::eof() {
	if (_pos >= _len) {
		return true;
	}
	return false;
}

__int64 MMapInputStream::currentPos() const {
	return _pos;
}

void MMapInputStream::seek(__int64 i) {
	_addr = _initaddr + i;
	_pos = i;
}

__int64 MMapInputStream::crc32() {
	return 0;
}

void MMapInputStream::close() {
	if (_pFile > 0) {
#ifndef WINDOWS
		::close(_pFile);
		_pFile = 0;
#else
		CloseHandle(_pFile);
#endif
		_open = false;
	}
}

bool MMapInputStream::isClosed() {
	return !_open;
}
