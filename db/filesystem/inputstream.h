#ifndef INPUTSTREAM_H
#define INPUTSTREAM_H

#include "util.h"
#include <istream>
#include <iostream>
#include <stdio.h>
#include <limits.h>

class InputStream
{
public:
    InputStream() {};

    virtual unsigned char readChar() = 0;
    /* Reads 2 bytes in the input (little endian order) */
    virtual int readInt () = 0;
    /* Reads 4 bytes in the input (little endian order) */
    virtual long readLong () = 0;
    /* Reads 8 bytes in the input (little endian order) */
    virtual __LONG64 readLong64 () = 0;
    /* Reads a 4 byte float in the input */
    virtual float readFloatIEEE () = 0;
    /* Reads a 8 byte double in the input */
    virtual double readDoubleIEEE () = 0;
    /* Read a chars */
    virtual char* readChars() = 0;
    virtual char* readChars(int length) = 0;
    bool eof();
    virtual bool isClosed() = 0;

	 virtual std::string* readString() = 0;

	 template <typename T>
	 T readData() {
		 T result = 0;
		 unsigned char* v = (unsigned char*)&result;
		 int size = sizeof(T);
		 for (int i = 0; i < size; i++) {
			 v[i] = readChar() & UCHAR_MAX;
			 //printf("v[%d]: %x ", i, v[i]);
		 }
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
};

#endif // INPUTSTREAM_H
