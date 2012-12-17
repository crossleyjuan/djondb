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

#include "dbfileinputstream.h"

#include "util.h"
#include <string.h>
#include <boost/crc.hpp>
#include <stdlib.h>
#include <errno.h>
#include <sstream>
#include <limits.h>
#include <assert.h>

DBFileInputStream::DBFileInputStream(InputStream* stream) {
	_stream = stream;
	_fileName = NULL;
	_versionOffset = 0;
	stream->seek(0);
	char* mark = stream->readChars();
	// check if the file is marked as versioned file (0.1 version does not have this mark)
	if (strcmp(mark, "djondb_dat") == 0) {
		char* version = stream->readChars();
		_dbVersion = new Version(version);
		_versionOffset = stream->currentPos();
		free(version);
	} else {
		_dbVersion = new Version("0.1");
		stream->seek(0);
	}
	free(mark);
}

DBFileInputStream::DBFileInputStream(InputStream* stream, char* fileName) {
	_stream = stream;
	_fileName = fileName;
	_versionOffset = 0;
	stream->seek(0);
	char* mark = stream->readChars();
	// check if the file is marked as versioned file (0.1 version does not have this mark)
	if (strcmp(mark, "djondb_dat") == 0) {
		char* version = stream->readChars();
		_dbVersion = new Version(version);
		_versionOffset = stream->currentPos();
	} else {
		_dbVersion = new Version("0.1");
		stream->seek(0);
	}
}

DBFileInputStream::~DBFileInputStream() {
	delete _stream;
	delete _dbVersion;
	if (_fileName = NULL) free(_fileName);
}

void DBFileInputStream::seek(__int64 i, SEEK_DIRECTION direction) {
	if (direction == FROMSTART_SEEK) {
		_stream->seek(_versionOffset + i, direction);
	} else {
		_stream->seek(i, direction);
	}
}

__int64 DBFileInputStream::currentPos() const {
	__int64 pos = _stream->currentPos();
	return pos - _versionOffset;
}

void DBFileInputStream::close() {
	_stream->close();
}

const std::string DBFileInputStream::fileName() const {
	return _fileName;
}

unsigned char DBFileInputStream::readChar() {
	return _stream->readChar();
}

/* Reads 2 bytes in the input (little endian order) */
__int16 DBFileInputStream::readShortInt () {
	return _stream->readShortInt();
}

/* Reads 4 bytes in the input (little endian order) */
__int32 DBFileInputStream::readInt () {
	return _stream->readInt();
}

/* Reads 4 bytes in the input (little endian order) */
__int64 DBFileInputStream::readLong () {
	return _stream->readLong();
}

/* Reads 4 bytes in the input (little endian order) */
__int64 DBFileInputStream::readLong64() {
	return _stream->readLong64();
}

/* Reads a 4 byte float in the input */
float DBFileInputStream::readFloatIEEE () {
	return _stream->readFloatIEEE();
}

/* Reads a 8 byte double in the input */
double DBFileInputStream::readDoubleIEEE () {
	return _stream->readDoubleIEEE();
}

/* Read a chars */
char* DBFileInputStream::readChars() {
	return _stream->readChars();
}

std::string* DBFileInputStream::readString() {
	return _stream->readString();
}

char* DBFileInputStream::readChars(__int32 length) {
	return _stream->readChars(length);
}

bool DBFileInputStream::eof() {
	return _stream->eof();
}

bool DBFileInputStream::isClosed() {
	return _stream->isClosed();
}

Version* DBFileInputStream::version() const {
	return _dbVersion;
}
