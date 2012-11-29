// =====================================================================================
//  Filename:  bsonarrayobj.cpp
//
//  Description:  BSONArrayObj implementation
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

#include "bsonarrayobj.h"
#include <sstream>
#include <string>
#include <stdlib.h>
#include "bsonobj.h"

BSONArrayObj::BSONArrayObj() {
}

BSONArrayObj::~BSONArrayObj() {
	for (std::vector<BSONObj*>::iterator i = _elements.begin(); i != _elements.end(); i++) {
		BSONObj* element = *i;
	   delete element;	
	}
	_elements.clear();
}

BSONArrayObj::BSONArrayObj(const BSONArrayObj& orig) {
	for (std::vector<BSONObj*>::const_iterator i = orig._elements.begin(); i != orig._elements.end(); i++) {
		BSONObj* element = *i;
		BSONObj* copy = new BSONObj(*element);
		this->_elements.push_back(copy);
	}
}

BSONArrayObj::BSONArrayObj(const std::vector<BSONObj*>& orig) {
	for (std::vector<BSONObj*>::const_iterator i = orig.begin(); i != orig.end(); i++) {
		BSONObj* element = *i;
		BSONObj* copy = new BSONObj(*element);
		this->_elements.push_back(copy);
	}
}

int BSONArrayObj::length() const {
	return _elements.size();
}

void BSONArrayObj::add(const BSONObj& obj) {
	_elements.push_back(new BSONObj(obj));
}

BSONObj* BSONArrayObj::get(int index) const {
	return _elements.at(index);
}

char* BSONArrayObj::toChar() const {
	std::stringstream ss;
	ss << "[";
	bool first = true;
	for (std::vector<BSONObj*>::const_iterator i = _elements.begin(); i != _elements.end(); i++) {
		if (!first) {
			ss << ", ";
		}
		first = false;
		BSONObj* element = *i;
		ss << element->toChar();
	}
	ss << "]";
	std::string sres = ss.str();
	char* result = (char*)malloc(sres.length() + 1);
	memset(result, 0, sres.length() + 1);
	strcpy(result, sres.c_str());

	return result;
}

BSONArrayObj::iterator BSONArrayObj::begin() {
	return _elements.begin();
}

BSONArrayObj::iterator BSONArrayObj::end() {
	return _elements.end();
}

BSONArrayObj* BSONArrayObj::select(const char* select) const {
	bool include_all = (strcmp(select, "*") == 0);
	BSONArrayObj* result = new BSONArrayObj();
	for (std::vector<BSONObj*>::const_iterator i = _elements.begin(); i != _elements.end(); i++) {
		BSONObj* element = *i;
		if (include_all) {
			result->add(*element);
		} else {
			BSONObj* sub = element->select(select);
			result->add(*sub);
			delete sub;
		}
	}
	return result;
}
