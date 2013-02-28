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

#include "dbfilestream.h"

#include "util.h"
#include <string.h>
#include <boost/crc.hpp>
#include <stdlib.h>
#include <errno.h>
#include <sstream>
#include <limits.h>
#include <assert.h>

DBFileStream::DBFileStream(InputOutputStream* stream) {
	_stream = stream;
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
	}
	seek(0, FROMEND_SEEK);
	free(mark);
}

DBFileStream::~DBFileStream() {
	delete _stream;
	delete _dbVersion;
}

/* Write 1 byte in the output */
void DBFileStream::writeChar (unsigned char v)
{
	_stream->writeChar(v);
}

/* Write 2 bytes in the output (little endian order) */
void DBFileStream::writeShortInt (__int16 v)
{
	_stream->writeShortInt(v);
}

/* Write 4 bytes in the output (little endian order) */
void DBFileStream::writeInt (__int32 v)
{
	_stream->writeInt(v);
}

/* Write 4 bytes in the output (little endian order) */
void DBFileStream::writeLong (__int64 v)
{
	_stream->writeLong(v);
}

/* Write a 4 byte float in the output */
void DBFileStream::writeFloatIEEE (float v)
{
	_stream->writeFloatIEEE(v);
}

/* Write a 8 byte double in the output */
void DBFileStream::writeDoubleIEEE (double v)
{
	_stream->writeDoubleIEEE(v);
}

void DBFileStream::writeChars(const char *text, __int32 len) {
	_stream->writeChars(text, len);
}

void DBFileStream::writeString(const std::string& text) {
	_stream->writeString(text);
}

void DBFileStream::seek(__int64 i, SEEK_DIRECTION direction) {
	if (direction == FROMSTART_SEEK) {
		_stream->seek(_versionOffset + i, direction);
	} else {
		_stream->seek(i, direction);
	}
}

__int64 DBFileStream::currentPos() const {
	__int64 pos = _stream->currentPos();
	return pos - _versionOffset;
}

void DBFileStream::close() {
	_stream->close();
}

void DBFileStream::flush() {
	_stream->flush();
}

const std::string DBFileStream::fileName() const {
	return _stream->fileName();
}

unsigned char DBFileStream::readChar() {
	return _stream->readChar();
}

/* Reads 2 bytes in the input (little endian order) */
__int16 DBFileStream::readShortInt () {
	return _stream->readShortInt();
}

/* Reads 4 bytes in the input (little endian order) */
__int32 DBFileStream::readInt () {
	return _stream->readInt();
}

/* Reads 4 bytes in the input (little endian order) */
__int64 DBFileStream::readLong () {
	return _stream->readLong();
}

/* Reads 4 bytes in the input (little endian order) */
__int64 DBFileStream::readLong64() {
	return _stream->readLong64();
}

/* Reads a 4 byte float in the input */
float DBFileStream::readFloatIEEE () {
	return _stream->readFloatIEEE();
}

/* Reads a 8 byte double in the input */
double DBFileStream::readDoubleIEEE () {
	return _stream->readDoubleIEEE();
}

/* Read a chars */
char* DBFileStream::readChars() {
	return _stream->readChars();
}

std::string* DBFileStream::readString() {
	return _stream->readString();
}

char* DBFileStream::readChars(__int32 length) {
	return _stream->readChars(length);
}

const char* DBFileStream::readFull() {
	return _stream->readFull();
}

bool DBFileStream::eof() {
	return _stream->eof();
}

bool DBFileStream::isClosed() {
	return _stream->isClosed();
}

Version* DBFileStream::version() const {
	return _dbVersion;
}
