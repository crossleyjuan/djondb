#ifndef INPUTSTREAM_H
#define INPUTSTREAM_H

#include "util.h"
#include "filesystemdefs.h"
#include "lock.h"
#include <istream>
#include <iostream>
#include <stdio.h>
#include <limits.h>

class InputStream
{
public:
    InputStream() {};

    virtual unsigned char readChar() = 0;
    /* Reads 1 bytes in the input (little endian order) */
    virtual bool readBoolean () = 0;
    /* Reads 2 bytes in the input (little endian order) */
    virtual __int16 readShortInt () = 0;
    /* Reads 2 bytes in the input (little endian order) */
    virtual __int32 readInt () = 0;
    /* Reads 4 bytes in the input (little endian order) */
    virtual __int64 readLong () = 0;
    /* Reads a 16 byte float in the input */
    virtual __int64 readLong64() = 0;
    /* Reads a 4 byte float in the input */
    virtual float readFloatIEEE () = 0;
    /* Reads a 8 byte double in the input */
    virtual double readDoubleIEEE () = 0;
    /* Read a chars */
    virtual char* readChars() = 0;
    virtual char* readChars(int length) = 0;
    virtual bool eof() = 0;
    virtual bool isClosed() = 0;

    virtual void close() = 0;

	virtual void seek(__int64 pos, SEEK_DIRECTION direction = FROMSTART_SEEK) = 0;

	 virtual __int64 currentPos() const = 0;

	 virtual std::string* readString() = 0;

	 void acquireLock() {
		 _lockStream.lock();
	 }

	 void releaseLock() {
		 _lockStream.unlock();
	 }

	 template <typename T>
		 T readData() {
			 T result = 0;
			 unsigned char* v = (unsigned char*)&result;
			 int size = sizeof(T);
			 char* data = readChars(size);
			 for (int i = 0; i < size; i++) {
				 v[i] = data[i] & UCHAR_MAX;
			 }
			 free(data);
			 T clear = 0;
			 for (int i = 0; i < size; i++) {
				 clear = clear << 8;
				 clear = clear | 0xFF;
			 }
			 //printf("\nresult before add: %x\n", result);
			 result = result & clear;
			 //printf("result after add: %x\n", result);
			 return result;
		 }
private:
	 Lock _lockStream;
};

#endif // INPUTSTREAM_H
