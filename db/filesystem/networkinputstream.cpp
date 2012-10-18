// *********************************************************************************************************************
// file:
// author: Juan Pablo Crossley (crossleyjuan@gmail.com)
// created:
// updated:
// license:
// 
// This file is part of the djondb project, for license information please refer to the LICENSE file,
// the application and libraries are provided as-is and free of use under the terms explained in the file LICENSE
// Its authors create this application in order to make the world a better place to live, but you should use it on
// your own risks.
// 
// Also, be adviced that, the GPL license force the committers to ensure this application will be free of use, thus
// if you do any modification you will be required to provide it for free unless you use it for personal use (you may 
// charge yourself if you want), bare in mind that you will be required to provide a copy of the license terms that ensures
// this program will be open sourced and all its derivated work will be too.
// *********************************************************************************************************************

#include "networkinputstream.h"

#include "defs.h"
#include "util.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sstream>
#include <iostream>
#include <limits.h>
#ifndef _WIN32
  #include <sys/types.h>
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <sys/ioctl.h>
#else
  #include <winsock.h>         // For socket(), connect(), send(), and recv()
  typedef int socklen_t;
  typedef char raw_type;       // Type used for raw data on this platform
#endif
#include <fcntl.h>
#include <assert.h>
#include <errno.h>

const int STREAM_BUFFER_SIZE = 100000;


NetworkInputStream::NetworkInputStream(int clientSocket)
{
	_socket = clientSocket;
	_buffer = (char*) malloc(STREAM_BUFFER_SIZE);
	_bufferPos = 0;
	_bufferSize = 0;
	_open = true;
	_logger = getLogger(NULL);
}

NetworkInputStream::NetworkInputStream(const NetworkInputStream& orig) {
	_buffer = orig._buffer;
	this->_bufferPos = orig._bufferPos;
	this->_bufferSize = orig._bufferSize;
	this->_socket = orig._socket;
	_logger = getLogger(NULL);
}

NetworkInputStream::~NetworkInputStream() {
	free (_buffer);
	//    closeStream();
	delete _logger;
}

unsigned char NetworkInputStream::readChar() {
	if (_logger->isDebug()) _logger->debug(3, "NetworkInputStream::readChar");
	unsigned char v;
	readBufferData(&v, 1);

	if (_logger->isDebug()) _logger->debug(3, "char: %d", v);
	if (_logger->isDebug()) _logger->debug(3, "~NetworkInputStream::readChar");
	return v;
}

/* Reads 2 bytes in the input (little endian order) */
short int NetworkInputStream::readShortInt () {
	if (_logger->isDebug()) _logger->debug(3, "NetworkInputStream::readShortInt");
	short int result = readData<short int>();
	if (_logger->isDebug()) _logger->debug(3, "~NetworkInputStream::readInt");
	return result;
}

/* Reads 4 bytes in the input (little endian order) */
int NetworkInputStream::readInt () {
	if (_logger->isDebug()) _logger->debug(3, "NetworkInputStream::readInt");
	int v = readData<int>(); 

	if (_logger->isDebug()) _logger->debug(3, "int: %d", v);
	if (_logger->isDebug()) _logger->debug(3, "~NetworkInputStream::readLong");
	return v;
}

/* Reads 4 bytes in the input (little endian order) */
long NetworkInputStream::readLong () {
#ifdef _64BITS
	return (long)readLong64();
#else
	return (long)readInt();
#endif
}

/* Reads 4 bytes in the input (little endian order) */
__LONG64 NetworkInputStream::readLong64 () {
	return readData<__LONG64>();
}

/* Reads a 4 byte float in the input */
float NetworkInputStream::readFloatIEEE () {
	float f;
   readBufferData(&f, sizeof(f));
	return f;
}

/* Reads a 8 byte double in the input */
double NetworkInputStream::readDoubleIEEE () {
	double d;
	readBufferData(&d, sizeof(d));
	return d;
}

/* Read a chars */
char* NetworkInputStream::readChars() {
	if (_logger->isDebug()) _logger->debug(3, "NetworkInputStream::readChars");
	int len = readLong();
	char* res;
	if (len > 0) {
		res = readChars(len);
		if (_logger->isDebug()) _logger->debug(3, "chars: ", res);
	} else {
		res = (char*)malloc(1);
		memset(res, 0, 1);
	}
	if (_logger->isDebug()) _logger->debug(3, "~NetworkInputStream::readChars");
	return res;
}

std::string* NetworkInputStream::readString() {
	if (_logger->isDebug()) _logger->debug(3, "NetworkInputStream::readChars");
	char* c = readChars();
	std::string* res = new std::string(c);
	if (_logger->isDebug()) _logger->debug(3, "string: %s", c);
	free(c);
	if (_logger->isDebug()) _logger->debug(3, "~NetworkInputStream::readChars");
	return res;
}

char* NetworkInputStream::readChars(int length) {
	char* res = (char*)malloc((length+1) * sizeof(char));
	memset(res, 0, length+1);
	int readed = readBufferData(res, length);
	assert(readed <= length);
	return res;
}

bool NetworkInputStream::eof() {
	return false;
}

void NetworkInputStream::closeStream() {
#ifdef _WIN32
	::closesocket(_socket);
#else
	close(_socket);
#endif
	_open = false;
}


bool NetworkInputStream::isClosed() {
	checkStatus();
	return !_open;
}

int NetworkInputStream::checkStatus() {
	if (_open) {
		int res = waitAvailable(2000);
		if (res < 0) {
			closeStream();
		}
		return res;
	}
	return 0;
}

int NetworkInputStream::available() {
	int available = (_bufferSize - _bufferPos);
	return available;
	//    size_t nbytes = 0;
	//    if ( ioctl(_socket, FIONREAD, (char*)&nbytes) < 0 )  {
	//            return -1;
	//    }
	//    return nbytes;
}

int NetworkInputStream::readBufferData(void* data, int len) {
	if (checkStatus() < 0) {
		return -1;
	}
	// wait until a data is available to be readed
	int readed = 0;
	while (readed < len) {
#ifdef DEBUG
		if (waitAvailable(60) < 0) {
#else 
		if (waitAvailable(10) < 0) {
#endif
			assert(false);
			return -1;
		}
		int read = (len - readed);
		if ((_bufferSize - _bufferPos) < read) {
			read = _bufferSize - _bufferPos;
		}
		memcpy((char*)data + readed, _buffer + _bufferPos, read);
		_bufferPos += read;
		//        int read = recv(_socket, data, len, 0);
		//        // the connection could be closed
		//        assert(read != 0);
		readed += read;
	}

	return readed;
}

int NetworkInputStream::waitAvailable(int timeout) {
	if (_bufferPos >= _bufferSize) {
		int result = fillBuffer(timeout);
		return result;
	}
	return 0;
}

int NetworkInputStream::setNonblocking() {
	int flags;

	/* If they have O_NONBLOCK, use the Posix way to do it */
#if defined(O_NONBLOCK)
	/* Fixme: O_NONBLOCK is defined but broken on SunOS 4.1.x and AIX 3.2.5. */
	if (-1 == (flags = fcntl(_socket, F_GETFL, 0)))
		flags = 0;
	return fcntl(_socket, F_SETFL, flags | O_NONBLOCK);
#else
#ifndef WINDOWS
	/* Otherwise, use the old way of doing it */
	flags = 1;
	return ioctl(_socket, FIOBIO, &flags);
#else
	u_long f = 1;
	return ioctlsocket(_socket, FIONBIO, &f);
#endif
#endif
}

int NetworkInputStream::fillBuffer(int timeout) {
	Logger* log = getLogger(NULL);
	memset(_buffer, 0, STREAM_BUFFER_SIZE);

	fd_set read;
	FD_ZERO(&read);
	FD_SET(_socket, &read);
	timeval val;
	val.tv_sec = timeout;
	val.tv_usec = 0;
	int result = select(_socket + 1, &read, NULL, NULL, &val);

	bool error = false;
	if (result < 0) {
		if (errno != EINTR) {
			log->error("An error ocurred with select(). Number: %d, socket: %d, error description: %s", errno, _socket, strerror(errno));
			error = true;
		}
	} else if (result == 0) {
		// Timeout
		error = true;
		log->error(std::string("fillBuffer: timeout"));
	}
	if (!error) {
		int readed = recv(_socket, _buffer, STREAM_BUFFER_SIZE, 0);
		if (readed < 0) {
			log->error("An error ocurred with recv(). Number: %d, socket: %d, error description: %s", errno, _socket, strerror(errno));
			error = true;
		}
		if (readed == 0) {
			// Nothing readed
			error = true;
		}
		_bufferPos = 0;
		_bufferSize = readed;
	}
	delete log;
	if (error) {
		closeStream();
		return -1;
	}
	return 0;
}
