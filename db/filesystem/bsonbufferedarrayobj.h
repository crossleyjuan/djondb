// =====================================================================================
//  Filename:  bsonarrayobj.h
//
//  Description:  This file contains the definition of the class BSONBufferedArrayObj which is an array of BSONBufferedObj
//
//  Version:  1.0
//  Created:  02/15/2012 09:07:11 AM
//  Revision:  none
//  Compiler:  gcc
//
//  Author:  Juan Pablo Crossley (crossleyjuan@gmail.com),
//
// License:
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
// =====================================================================================

#ifndef BSONBUFFEREDARRAYOBJ_H_INCLUDED
#define BSONBUFFEREDARRAYOBJ_H_INCLUDED

#include <vector>
#include "bsonarrayobj.h"
#include <limits.h>

class BSONBufferedObj;

class BSONBufferedArrayObj: public BSONArrayObj {
public:
	BSONBufferedArrayObj(char* buffer, __int64 len);
	~BSONBufferedArrayObj();
	BSONBufferedArrayObj(const BSONBufferedArrayObj& orig);

	__int32 length() const;
	BSONObj* get(__int32 index) const;
   char* toChar() const;
	BSONArrayObj* select(const char* select) const;
	
	iterator begin();
	iterator end();
	virtual __int64 bufferLength() const;

private:
	void initialize();
	char* _buffer;
	int   _bufferMaxLen;

	int   _bufferArrayLen;

	std::vector<BSONObj*> _elements;

private:
	char readChar(char*& buffer)  const {
		char c = buffer[0];
		buffer += 1;
		return c;
	}

	template <typename T>
		T readData(char*& buffer) {
			T result = 0;
			unsigned char* v = (unsigned char*)&result;
			int size = sizeof(T);
			for (int i = 0; i < size; i++) {
				v[i] = readChar(buffer) & UCHAR_MAX;
			}
			T clear = 0;
			for (int i = 0; i < size; i++) {
				clear = clear << 8;
				clear = clear | 0xFF;
			}
			//printf("\nresult before add: %x\n", result);
			result = result & clear;
			//printf("result after add: %x\n", result);
			return result;
		}
};
#endif // BSONBUFFEREDARRAYOBJ_H_INCLUDED
