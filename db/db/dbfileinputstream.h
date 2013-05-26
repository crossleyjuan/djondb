#ifndef DBFileInputStream_H
#define DBFileInputStream_H

#include "inputoutputstream.h"
#include "util.h"
#include <string>

using namespace std;

class DBFileInputStream: public InputStream
{
    public:
        DBFileInputStream(const DBFileInputStream& other);
        DBFileInputStream(InputStream* stream, char* fileName);
        DBFileInputStream(InputStream* stream);
        virtual ~DBFileInputStream();

        virtual unsigned char readChar();
        /* Reads 1 bytes in the input (little endian order) */
        virtual bool readBoolean ();
        /* Reads 2 bytes in the input (little endian order) */
        virtual short int readShortInt ();
        /* Reads 4 bytes in the input (little endian order) */
        virtual __int32 readInt ();
        /* Reads 16 bytes in the input (little endian order) */
        virtual __int64 readLong ();
        /* Reads 16 bytes in the input (little endian order) */
        virtual __int64 readLong64();
        /* Reads a 4 byte float in the input */
        virtual float readFloatIEEE ();
        /* Reads a 8 byte double in the input */
        virtual double readDoubleIEEE ();
        /* Read a chars */
        virtual char* readChars();
        virtual char* readChars(__int32 length);

        virtual std::string* readString();

		virtual void seek(__int64 pos, SEEK_DIRECTION direction = FROMSTART_SEEK);
        virtual __int64 currentPos() const;

        virtual const std::string fileName() const;
        virtual bool eof();
        virtual void close();

        virtual bool isClosed();

		Version* version() const;

	 private:
		  InputStream* _stream;
		  __int32 _versionOffset;
		  Version* _dbVersion;
		  char* _fileName;
};

#endif // DBFileInputStream_H
