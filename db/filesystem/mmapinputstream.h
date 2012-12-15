#ifndef MMAPINPUTSTREAM_H
#define MMAPINPUTSTREAM_H

#include "inputstream.h"
#include <istream>
#include <iostream>
#include <stdio.h>

#ifdef WINDOWS
#include <windows.h>
#endif

class MMapInputStream: public InputStream
{
public:
    MMapInputStream(const char* fileName, const char* flags);
    virtual ~MMapInputStream();

    virtual unsigned char readChar();
    /* Reads 2 bytes in the input (little endian order) */
    virtual short int readShortInt ();
    /* Reads 4 bytes in the input (little endian order) */
    virtual __int32 readInt ();
    /* Reads 4 bytes in the input (little endian order) */
    virtual __int64 readLong ();
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
    virtual void seek(__int64 i);
    virtual __int64 crc32();

    virtual std::string* readString();
    virtual const std::string fileName() const;
    virtual bool eof();

    virtual void close();

    virtual bool isClosed();
private:
	 void read(char* dest, int len);

private:
#ifndef WINDOWS
    int _pFile;
#else
    HANDLE _pFile;
#endif
	 char* _initaddr;
	 char* _addr;
	 __int64 _pos;
	 __int64 _len;
    std::string _fileName;
    bool _open;
};

#endif // MMAPINPUTSTREAM_H
