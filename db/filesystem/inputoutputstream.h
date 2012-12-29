/*
 * =====================================================================================
 *
 *       Filename:  inputoutputstream.h
 *
 *    Description:  This is an extended interface of InputStream and OutputStream
 *
 *        Version:  1.0
 *        Created:  12/15/2012 11:11:37 AM
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
#ifndef INPUTOUTPUTSTREAM_INCLUDED_H
#define INPUTOUTPUTSTREAM_INCLUDED_H 

#include "inputstream.h"
#include "outputstream.h"
#include "filesystemdefs.h"
#include <string>

using namespace std;

class InputOutputStream: public InputStream, public OutputStream
{
    public:
        virtual unsigned char readChar() = 0;
        /* Reads 2 bytes in the input (little endian order) */
        virtual __int16 readShortInt () = 0;
        /* Reads 4 bytes in the input (little endian order) */
        virtual __int32 readInt () = 0;
        /* Reads 8 bytes in the input (little endian order) */
        virtual __int64 readLong () = 0;
		/* Reads a 16 byte long in the input */
		virtual __int64 readLong64() = 0;
        /* Reads a 4 byte float in the input */
        virtual float readFloatIEEE () = 0;
        /* Reads a 8 byte double in the input */
        virtual double readDoubleIEEE () = 0;
        /* Read a chars */
        virtual char* readChars() = 0;
        virtual char* readChars(__int32 length) = 0;
        virtual const char* readFull() = 0;

        virtual std::string* readString() = 0;

        virtual void writeChar (unsigned char v) = 0;
        /* Write 2 bytes in the output (little endian order) */
        virtual void writeShortInt (__int16 v) = 0;
        /* Write 4 bytes in the output (little endian order) */
        virtual void writeInt (__int32 v) = 0;
        /* Write 4 bytes in the output (little endian order) */
        virtual void writeLong (__int64 v) = 0;
        /* Write a 4 byte float in the output */
        virtual void writeFloatIEEE (float v) = 0;
        /* Write a 8 byte double in the output */
        virtual void writeDoubleIEEE (double v) = 0;
        /* Write a char */
        virtual void writeChars(const char* text, __int32 len) = 0;
        virtual void writeString(const std::string& text) = 0;

		virtual void seek(__int64 pos, SEEK_DIRECTION direction = FROMSTART_SEEK) = 0;
        virtual __int64 currentPos() const = 0;

        virtual const std::string fileName() const = 0;
        virtual bool eof() = 0;
        virtual void close() = 0;

        virtual void flush() = 0;

        virtual bool isClosed() = 0;

};

#endif /* INPUTOUTPUTSTREAM_INCLUDED_H */
