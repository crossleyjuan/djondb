#ifndef FILEINPUTSTREAMW32_H
#define FILEINPUTSTREAMW32_H

#include "inputstream.h"
#include <istream>
#include <iostream>
#include <stdio.h>

class FileInputStreamW32: public InputStream
{
public:
    FileInputStreamW32(const char* fileName, const char* flags);
    virtual ~FileInputStreamW32();

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
    FILE* _pFile;
    std::string _fileName;
    bool _open;
};

#endif // FILEINPUTSTREAMw32_H
