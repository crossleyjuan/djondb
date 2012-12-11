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

FileInputOutputStream::FileInputOutputStream(const std::string& fileName, const char* flags) {
	Logger* log = getLogger(NULL);
    _pFile = fopen(fileName.c_str(), flags);

    // Position the cursor at the end of the file
	 if (_pFile == NULL) {
		 log->error("Error opening the file: %s. Error: %d:%s", fileName.c_str(), errno, strerror(errno));
		 exit(1);
	 }
    fseek(_pFile, 0, SEEK_END);
    _fileName = fileName;
    _open = true;
	 delete log;
}

FileInputOutputStream::~FileInputOutputStream() {
    close();
}

/* Write 1 byte in the output */
void FileInputOutputStream::writeChar (unsigned char v)
{
    fwrite(&v, 1, 1, _pFile);
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
    fwrite(&v, 1, sizeof(v), _pFile);
}

/* Write a 8 byte double in the output */
void FileInputOutputStream::writeDoubleIEEE (double v)
{
    fwrite(&v, 1, sizeof(v), _pFile);
}

void FileInputOutputStream::writeChars(const char *text, __int32 len) {
    writeInt(len);
    fwrite(text, 1, len, _pFile);
}

void FileInputOutputStream::writeString(const std::string& text) {
    const char* c = text.c_str();
    __int32 l = strlen(c);
    writeChars(c, l);
}

void FileInputOutputStream::seek(__int64 i) {
    fflush(_pFile);
    fseek (_pFile, i, SEEK_SET);
}

__int64 FileInputOutputStream::currentPos() const {
    return ftell(_pFile);
}

void FileInputOutputStream::close() {
    if (_pFile) {
        flush();
        fclose(_pFile);
        _pFile = 0;
        _open = false;
    }
}

void FileInputOutputStream::flush() {
    fflush(_pFile);
}

const std::string FileInputOutputStream::fileName() const {
    return _fileName;
}

unsigned char FileInputOutputStream::readChar() {
    unsigned char v = 0;
    fread(&v, 1, 1, _pFile);
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
    fread(&f, 1, sizeof(f), _pFile);
    return f;
}

/* Reads a 8 byte double in the input */
double FileInputOutputStream::readDoubleIEEE () {
    double d;
    fread(&d, 1, sizeof(d), _pFile);
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
    fread(res, 1, length, _pFile);
    return res;
}

const char* FileInputOutputStream::readFull() {
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

bool FileInputOutputStream::eof() {
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

bool FileInputOutputStream::isClosed() {
    return !_open;
}
