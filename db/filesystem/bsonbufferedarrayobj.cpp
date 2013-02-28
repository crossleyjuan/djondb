// =====================================================================================
//  Filename:  bsonarrayobj.cpp
//
//  Description:  BSONBufferedArrayObj implementation
//
//  Version:  1.0
//  Created:  02/15/2012 05:50:03 PM
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

#include "bsonbufferedarrayobj.h"
#include "bsonbufferedobj.h"
#include <sstream>
#include <string>
#include <stdlib.h>
#include "bsonobj.h"

BSONBufferedArrayObj::BSONBufferedArrayObj(char* buffer, __int64 len) {
	this->_buffer = buffer;
	this->_bufferMaxLen = len;
	this->_bufferArrayLen = 0;

	initialize();
}

void BSONBufferedArrayObj::initialize() {
	char* pCurrentElement = _buffer;

	 __int64 elements = readData<__int64>(pCurrentElement);
	 _bufferArrayLen += sizeof(__int64);

	 for (__int64 x = 0; x < elements; x++) {
		 BSONBufferedObj* obj = new BSONBufferedObj(pCurrentElement, _bufferMaxLen - _bufferArrayLen);
		 _elements.push_back(obj);
		 pCurrentElement += obj->bufferLength();
		 _bufferArrayLen += obj->bufferLength();
	 }
}

BSONBufferedArrayObj::~BSONBufferedArrayObj() {
	for (std::vector<BSONObj*>::iterator i = _elements.begin(); i != _elements.end(); i++) {
		BSONBufferedObj* element = (BSONBufferedObj*)*i;
	   delete element;	
	}
	_elements.clear();
}

BSONBufferedArrayObj::BSONBufferedArrayObj(const BSONBufferedArrayObj& orig) {
	for (std::vector<BSONObj*>::const_iterator i = orig._elements.begin(); i != orig._elements.end(); i++) {
		BSONBufferedObj* element = (BSONBufferedObj*)*i;
		this->_elements.push_back(element);
	}
}

__int32 BSONBufferedArrayObj::length() const {
	return _elements.size();
}

__int64 BSONBufferedArrayObj::bufferLength() const {
	return _bufferArrayLen;;
}

BSONObj* BSONBufferedArrayObj::get(__int32 index) const {
	return _elements.at(index);
}

char* BSONBufferedArrayObj::toChar() const {
	std::stringstream ss;
	ss << "[";
	bool first = true;
	for (std::vector<BSONObj*>::const_iterator i = _elements.begin(); i != _elements.end(); i++) {
		if (!first) {
			ss << ", ";
		}
		first = false;
		BSONBufferedObj* element = (BSONBufferedObj*)*i;
		ss << element->toChar();
	}
	ss << "]";
	std::string sres = ss.str();
	char* result = (char*)malloc(sres.length() + 1);
	memset(result, 0, sres.length() + 1);
	strcpy(result, sres.c_str());

	return result;
}

BSONArrayObj::iterator BSONBufferedArrayObj::begin() {
	return _elements.begin();
}

BSONArrayObj::iterator BSONBufferedArrayObj::end() {
	return _elements.end();
}

BSONArrayObj* BSONBufferedArrayObj::select(const char* select) const {
	bool include_all = (strcmp(select, "*") == 0);
	BSONArrayObj* result = new BSONArrayObj();
	for (std::vector<BSONObj*>::const_iterator i = _elements.begin(); i != _elements.end(); i++) {
		BSONBufferedObj* element = (BSONBufferedObj*)*i;
		BSONObj* sub = element->select(select);
		result->add(*sub);
		delete sub;
	}
	return result;
}
