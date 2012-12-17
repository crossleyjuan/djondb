#ifndef MEMORYSTREAM_H
#define MEMORYSTREAM_H

#include "inputstream.h"
#include "outputstream.h"
#include <string>

using namespace std;

class MemoryStream: public InputStream, public OutputStream
{
    public:
        MemoryStream(const MemoryStream& other);
        MemoryStream();
        MemoryStream(long bufferSize);
        virtual ~MemoryStream();

        virtual unsigned char readChar();
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
        virtual char* readChars(int length);
        virtual const char* readFull();

        virtual std::string* readString();

        virtual void writeChar (unsigned char v);
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
        virtual void writeChars(const char* text, int len);
        virtual void writeString(const std::string& text);

		virtual void seek(__int64 pos, SEEK_DIRECTION direction = FROMSTART_SEEK);
        virtual __int64 currentPos() const;

        virtual bool eof();
        virtual void close();

        virtual void flush();

        virtual bool isClosed();

		  char* toChars();

	 private:
		  void write(const char * ptr, size_t count);
		  size_t read(char * ptr, size_t count);
		  void allocate(const size_t size);
		  void nextBuffer();

    private:
        bool _open;
		  long _length;
		  long _pos;
		  int _currentIndex;
		  int _currentBufferPos;
		  char* _currentBuffer;

		  char** _buffer;
		  long _bufferSize;
		  int _maxIndexes;
};

#endif // MEMORYSTREAM_H
