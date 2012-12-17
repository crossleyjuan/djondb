#ifndef NETWORKINPUTSTREAM_H
#define NETWORKINPUTSTREAM_H

#include "inputstream.h"
#include "util.h"
#include <string>
#include <assert.h>

class NetworkInputStream : public InputStream
{
    public:
        /** Default constructor */
        NetworkInputStream(int socket);
        /** Copy constructor **/
        NetworkInputStream(const NetworkInputStream& orig);
        /** Default destructor */
        virtual ~NetworkInputStream();

    public:
        virtual unsigned char readChar();
        /* Reads 2 bytes in the input (little endian order) */
        virtual short int readShortInt ();
        /* Reads 4 bytes in the input (little endian order) */
        virtual __int32 readInt ();
        /* Reads 8 bytes in the input (little endian order) */
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

        virtual void closeStream();
        bool eof();
        int available();
        int waitAvailable(int timeout = 10);
        bool isClosed();

		  virtual void seek(__int64) {
			  // Unsupported methods in network interfaces
			  assert(false);
		  };

		  virtual __int64 currentPos() const {
			  // Unsupported methods in network interfaces
			  assert(false);
			  return 0;
		  };

		  virtual std::string* readString();
		  int setNonblocking();

		  char* _buffer;
		  int _bufferPos;
		  int _bufferSize;
	 protected:
	 private:
		  int _socket;
		  bool _open;
		  Logger* _logger;

	 private:
		  int checkStatus();
		  int readBufferData(void *buffer, __int32 len);
		  int fillBuffer(int timeout);
};

#endif // NETWORKINPUTSTREAM_H
