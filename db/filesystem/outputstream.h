#ifndef OUTPUTSTREAM_H
#define OUTPUTSTREAM_H

#include "util.h"
#include <string>
#include <stdio.h>

class OutputStream
{
public:
    virtual void writeChar (unsigned char v) = 0;
    /* Write 2 bytes in the output (little endian order) */
    virtual void writeInt (int v) = 0;
    /* Write 4 bytes in the output (little endian order) */
    virtual void writeLong (long v) = 0;
    /* Write 8 bytes in the output (little endian order) */
    virtual void writeLong64 (__LONG64 v) = 0;
    /* Write a 4 byte float in the output */
    virtual void writeFloatIEEE (float v) = 0;
    /* Write a 8 byte double in the output */
    virtual void writeDoubleIEEE (double v) = 0;
    /* Write a char */
    virtual void writeChars(const char* text, int len) = 0;
    virtual void writeString(const std::string& text) = 0;

	 template<typename T>
	 void writeData(T data) {
		 unsigned char* v = (unsigned char*)&data;
		 int size = sizeof(T);
		 //printf("\nwriting: %x\n", data);
		 for (int i = 0; i < size; i++) {
			 unsigned char c = v[i];
			 //printf("c: %x ", (int)c);
			 writeChar(c);
		 }
	 }
	 /*
	 void writeData(unsigned char* v, int size) {
		 for (int i = 0; i < size; i++) {
			 unsigned char c = v[i];
			 writeChar(c);
		 }
	 }
	 */
};

#endif // OUTPUTSTREAM_H
