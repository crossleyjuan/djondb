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
		MemoryStream(__int64 bufferSize);
		MemoryStream(char* buffer, __int32 len);
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
		virtual char* readChars(__int32 length);
		virtual const char* readFull();
		virtual const size_t readRaw(char* ptr, size_t count);

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
		virtual void writeChars(const char* text, __int32 len);
		virtual void writeString(const std::string& text);
		virtual void writeRaw(const char * ptr, size_t count);

		virtual void seek(__int64 pos, SEEK_DIRECTION direction = FROMSTART_SEEK);
		virtual __int64 currentPos() const;

		virtual bool eof();
		virtual void close();

		virtual void flush();

		virtual bool isClosed();

		char* toChars();

		__int64 size() const;

	private:
		void write(const char * ptr, size_t count);
		size_t read(char * ptr, size_t count);
		void allocate(const size_t size);
		void nextBuffer();

	private:
		bool _open;
		__int64 _length;
		__int64 _pos;
		__int32 _currentIndex;
		__int64 _currentBufferPos;
		char* _currentBuffer;

		char** _buffer;
		__int64 _bufferSize;
		__int32 _maxIndexes;
};

#endif // MEMORYSTREAM_H
