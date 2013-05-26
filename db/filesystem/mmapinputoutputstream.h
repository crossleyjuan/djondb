#ifndef MMAPINPUTOUTPUTSTREAM_H
#define MMAPINPUTOUTPUTSTREAM_H

#include "inputoutputstream.h"
#include <istream>
#include <iostream>
#include <stdio.h>

#ifdef WINDOWS
#include <windows.h>
#endif

class MMapInputOutputStream: public InputOutputStream
{
public:
    MMapInputOutputStream(const char* fileName, __int64 offset, __int32 pages);
    virtual ~MMapInputOutputStream();

    virtual unsigned char readChar();
		/* Reads 1 bytes in the input (little endian order) */
		virtual bool readBoolean ();
    /* Reads 2 bytes in the input (little endian order) */
    virtual __int16 readShortInt ();
    /* Reads 4 bytes in the input (little endian order) */
    virtual __int32 readInt ();
    /* Reads 4 bytes in the input (little endian order) */
    virtual __int64 readLong ();
    /* Reads a 16 byte long in the input */
    virtual __int64 readLong64();
    /* Reads a 4 byte float in the input */
    virtual float readFloatIEEE ();
    /* Reads a 8 byte double in the input */
    virtual double readDoubleIEEE ();
    /* Read a chars */
    virtual char* readChars();
    virtual char* readChars(__int32 length);
    virtual void readChars(__int32 length, char* res);
    virtual const char* readFull();

	 virtual __int64 currentPos() const;
	 virtual void seek(__int64 pos, SEEK_DIRECTION direction = FROMSTART_SEEK);
	 virtual __int64 crc32();
	 virtual __int64 length() const;
	 virtual std::string* readString();
	 virtual const std::string fileName() const;
	 virtual bool eof();

	 virtual void close();

	 virtual bool isClosed();

	 virtual void writeChar (unsigned char v);
	 /* Write 1 bytes in the output (little endian order) */
	 virtual void writeBoolean (bool v);
	 /* Write 2 bytes in the output (little endian order) */
	 virtual void writeShortInt (__int16 v);
	 /* Write 4 bytes in the output (little endian order) */
	 virtual void writeInt (__int32 v);
	 /* Write 4 bytes in the output (little endian order) */
	 virtual void writeLong (__int64 v);
	 /* Write a 4 byte float in the output */
	 virtual void writeFloatIEEE (float v);
	 /* Write a 8 byte double in the output */
	 virtual void writeDoubleIEEE (double v);
	 /* Write a char */
	 virtual void writeChars(const char* text, __int32 len);
	 virtual void writeString(const std::string& text);


	 virtual void flush();

	 char* pointer();
private:
	 void read(char* dest, int len);
	 void checkAvailableSpace();
	 void resize(__int32 len);


private:
#ifndef WINDOWS
	 int _pFile;
#else
	 HANDLE _pFile;
	 HANDLE _hMapFile;
#endif
	 char* _initaddr;
	 char* _addr;
	 __int64 _pos;
	 __int64 _len;
	 std::string _fileName;
	 bool _open;
};

#endif // MMAPINPUTOUTPUTSTREAM_H
