/*
 * =====================================================================================
 *
 *       Filename:  TXBUFFER.h
 *
 *    Description: This class work as a front controller and bridge for any controller
 *                 operation that needs a transaction 
 *
 *        Version:  1.0
 *        Created:  09/26/2012 08:26:29 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Juan Pablo Crossley (Cross), crossleyjuan@gmail.com
 *   Organization:  djondb
 *
 * This file is part of the djondb project, for license information please refer to the LICENSE file,
 * the application and libraries are provided as-is and free of use under the terms explained in the file LICENSE
 * Its authors create this application in order to make the world a better place to live, but you should use it on
 * your own risks.
 * 
 * Also, be adviced that, the GPL license force the committers to ensure this application will be free of use, thus
 * if you do any modification you will be required to provide it for free unless you use it for personal use (you may 
 * charge yourself if you want), bare in mind that you will be required to provide a copy of the license terms that ensures
 * this program will be open sourced and all its derivated work will be too.
 * =====================================================================================
 */

#ifndef TXBUFFER_INCLUDED_H
#define TXBUFFER_INCLUDED_H

#include <queue>
#include "inputoutputstream.h"

class TxBufferManager;

class TxBuffer: public InputOutputStream {
    public:
        TxBuffer(const TxBufferManager* manager, InputOutputStream* stream, __int64 offset, __int64 bufferLen);
        TxBuffer(const TxBuffer& other);
        virtual ~TxBuffer();

        virtual unsigned char readChar();
        /* Reads 2 bytes in the input (little endian order) */
        virtual __int16 readShortInt ();
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
		  virtual void writeChars(const char* text, __int32 len);
		  virtual void writeString(const std::string& text);

		  virtual void seek(__int64 pos, SEEK_DIRECTION direction = FROMSTART_SEEK);
		  virtual __int64 currentPos() const;

		  virtual const std::string fileName() const;
		  virtual bool eof();
		  virtual void close();

		  virtual void flush();

		  virtual bool isClosed();

		  __int32 controlPosition() const;
		  void setControlPosition(__int32 pos);
		  __int64 startOffset() const;
		  __int64 bufferLength() const;
		  void reset();

	 private:
		  InputOutputStream* _stream;
		  __int64 _startOffset;
		  __int64 _bufferLength;
		  __int64 _currentPos;
		  __int32 _controlPosition;
		  TxBufferManager* _manager;
};

#endif // TXBUFFER_INCLUDED_H
