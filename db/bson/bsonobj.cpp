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

#include "bsonobj.h"
#include "util.h"
#include "bsonutil.h"

#include "bsonparser.h"

#include <iostream>
#include <sstream>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <set>
#include <assert.h>

using namespace std;

#define MAX_BSONOBJ_BUFFER 8000
BSONObj::BSONObj()
{
	_cBSON = NULL;
}

BSONObj::~BSONObj()
{
	for (std::map<std::string, BSONContent*>::const_iterator i = _elements.begin(); i != _elements.end(); i++) {
		BSONContent* cont = i->second;
		delete(cont);
	}
	_elements.clear();
	if (_cBSON != NULL) {
		free(_cBSON);
		_cBSON = 0;
	}
}

/*
	void BSONObj::add(std::string key, void* val) {
	fillContent(key, PTR, val);
	}
	*/

void BSONObj::add(std::string key, __int32 val) {
	remove(key);
	BSONContentInt* content = new BSONContentInt(val); 
	_elements.insert(pair<std::string, BSONContent* >(key, content));
}

void BSONObj::add(std::string key, double val) {
	remove(key);
	BSONContentDouble* content = new BSONContentDouble(val); 
	_elements.insert(pair<std::string, BSONContent* >(key, content));
}

void BSONObj::add(std::string key, __int64 val) {
	remove(key);
	BSONContentLong* content = new BSONContentLong(val); 
	_elements.insert(pair<std::string, BSONContent* >(key, content));
}

void BSONObj::add(std::string key, char* val) {
	add(key, val, strlen(val));
}

void BSONObj::add(std::string key, char* val, __int32 length) {
	remove(key);
	BSONContentString* content = new BSONContentString(strcpy(val, length), length); 
	_elements.insert(pair<std::string, BSONContent* >(key, content));
}

void BSONObj::add(std::string key, const BSONObj& val) {
	remove(key);
	BSONContentBSON* content = new BSONContentBSON(new BSONObj(val)); 
	_elements.insert(pair<std::string, BSONContent* >(key, content));
}

void BSONObj::add(std::string key, const BSONArrayObj& val) {
	remove(key);
	BSONContentBSONArray* content = new BSONContentBSONArray(new BSONArrayObj(val)); 
	_elements.insert(pair<std::string, BSONContent* >(key, content));
}

char* BSONObj::toChar() {
	// if the toChar was already calculated before use it
	if (_cBSON != NULL) {
		return strcpy(_cBSON);
	}

	Logger* log = getLogger(NULL);
	
	char* result = (char*)malloc(MAX_BSONOBJ_BUFFER);
	memset(result, 0, MAX_BSONOBJ_BUFFER);

	__int32 pos = 0;
	result[0] = '{';
	pos += 1;

	bool first = true;
	
	for (std::map<std::string, BSONContent* >::const_iterator i = _elements.begin(); i != _elements.end(); i++) {
		if (!first) {
			result[pos] = ',';
			pos++;
		}
		first = false;
		BSONContent* content = i->second;
		std::string key = i->first;
		sprintf(result + pos, " \"%s\" : ", key.c_str());
		pos += key.length() + 6;
		//ss << "\"" << key << "\" :";
		char* chr;
		const char* cstr;
		switch (content->type())  {
			case BSON_TYPE: {
									 BSONContentBSON* bbson = (BSONContentBSON*)content;
									 BSONObj* bson = (BSONObj*)*bbson;
									 char* chrbson = bson->toChar();
									 sprintf(result + pos, "%s", chrbson);
									 free(chrbson);
									 break;
								 }
			case BSONARRAY_TYPE: {
											BSONContentBSONArray* bbsonarray = (BSONContentBSONArray*)content;
											BSONArrayObj* bsonarray = (BSONArrayObj*)*bbsonarray;
											char* chrbsonarray = bsonarray->toChar();
											sprintf(result + pos, "%s", chrbsonarray);
											free(chrbsonarray);
											break;
										}
			case INT_TYPE:  {
									 BSONContentInt* bint = (BSONContentInt*)content;
									 sprintf(result + pos, "%d", (__int32)*bint);
									 break;
								 }
			case LONG_TYPE: {
									 BSONContentLong* blong = (BSONContentLong*)content;
									 sprintf(result + pos, "%ld", (__int64)*blong);
									 break;
								 }
			case DOUBLE_TYPE: {
									 BSONContentDouble* bdouble = (BSONContentDouble*)content;
									 sprintf(result + pos, "%f", (double)*bdouble);
									 break;
								 }
			case STRING_TYPE:
			case PTRCHAR_TYPE: { 
										 BSONContentString* bstring = (BSONContentString*)content;

										 djondb::string s = *bstring;
										 sprintf(result + pos, "\"%.*s\"", s.length(), s.c_str());
										 break;
									 }
		}
		pos = strlen(result);
		assert(pos < MAX_BSONOBJ_BUFFER);
	}
	result[pos] = '}';
	result[pos+1] = 0;
	pos++;

	__int32 len = strlen(result);

	// Saves the value to cache the calculated value
	_cBSON = result;

	char* cresult = strcpy(result);

	return cresult;
}

__int32 BSONObj::getInt(std::string key) const throw(BSONException) {
	BSONContent* content = getContent(key);
	if ((content != NULL) && (content->type() == INT_TYPE)) {
		BSONContentInt* bint = (BSONContentInt*)content;
		return *bint;
	} else {
		throw BSONException(format("key not found %s", key.c_str()).c_str());
	}
}

double BSONObj::getDouble(std::string key) const throw(BSONException) {
	BSONContent* content = getContent(key);
	if ((content != NULL) && (content->type() == DOUBLE_TYPE)) {
		BSONContentDouble* bdouble = (BSONContentDouble*)content;
		return *bdouble;
	} else {
		throw BSONException(format("key not found %s", key.c_str()).c_str());
	}
}

__int64 BSONObj::getLong(std::string key) const throw(BSONException) {
	BSONContent* content = getContent(key);
	if ((content != NULL) && (content->type() == LONG_TYPE)) {
		BSONContentLong* blong = (BSONContentLong*)content;
		return *blong;
	} else {
		throw BSONException(format("key not found %s", key.c_str()).c_str());
	}
}

char* BSONObj::getString(std::string key) const throw(BSONException) {
	BSONContent* content = getContent(key);
	if ((content != NULL) && (content->type() == PTRCHAR_TYPE)) {
		BSONContentString* bstring = (BSONContentString*)content;
		djondb::string s = *bstring;
		return s.c_str();
	} else {
		throw BSONException(format("key not found %s", key.c_str()).c_str());
	}
}

BSONObj* BSONObj::getBSON(std::string key) const throw(BSONException) {
	BSONContent* content = getContent(key);
	if ((content != NULL) && (content->type() == BSON_TYPE)) {
		BSONContentBSON* bbson = (BSONContentBSON*)content;
		return *bbson;
	} else {
		throw BSONException(format("key not found %s", key.c_str()).c_str());
	}
}

BSONArrayObj* BSONObj::getBSONArray(std::string key) const throw(BSONException) {
	BSONContent* content = getContent(key);
	if ((content != NULL) && (content->type() == BSONARRAY_TYPE)) {
		BSONContentBSONArray* bbson = (BSONContentBSONArray*)content;
		return *bbson;
	} else {
		throw BSONException(format("key not found %s", key.c_str()).c_str());
	}
}

BSONContent* BSONObj::getContent(std::string key) const {
	BSONContent* content = NULL;
	for (std::map<std::string, BSONContent* >::const_iterator it = _elements.begin(); it != _elements.end(); it++) {
		std::string itKey = it->first;
		if (itKey.compare(key) == 0) {
			content = it->second;
			break;
		}
	}
	return content;
}

BSONContent* BSONObj::getContent(std::string key, BSONTYPE ttype) const {
	BSONContent* content = getContent(key);
	if (content != NULL) {
		if (content->type() != ttype) {
			throw "type does not match";
		}
	}
	return content;
}

BSONObj::const_iterator BSONObj::begin() const {
	return _elements.begin();
}

BSONObj::const_iterator BSONObj::end() const {
	return _elements.end();
}

__int32 BSONObj::length() const {
	return _elements.size();
}

bool BSONObj::has(std::string key) const {
	for (std::map<std::string, BSONContent* >::const_iterator it = _elements.begin(); it != _elements.end(); it++) {
		std::string itKey = it->first;
		if (itKey.compare(key) == 0) {
			return true;
		}
	}
	return false;
}

BSONObj::BSONObj(const BSONObj& orig) {
	for (std::map<std::string, BSONContent* >::const_iterator i = orig._elements.begin(); i != orig._elements.end(); i++) {
		std::string key = i->first;
		BSONContent* origContent = i->second;
		BSONContent* content;
		switch (origContent->type()) {
			case INT_TYPE: {
									content = new BSONContentInt(*(BSONContentInt*)origContent);
									break;
								}
			case LONG_TYPE: {
									content = new BSONContentLong(*(BSONContentLong*)origContent);
									break;
								}
			case DOUBLE_TYPE: {
									content = new BSONContentDouble(*(BSONContentDouble*)origContent);
									break;
								}
			case PTRCHAR_TYPE: {
									content = new BSONContentString(*(BSONContentString*)origContent);
									break;
								}
			case BSON_TYPE: {
									content = new BSONContentBSON(*(BSONContentBSON*)origContent);
									break;
								}
			case BSONARRAY_TYPE: {
									content = new BSONContentBSONArray(*(BSONContentBSONArray*)origContent);
									break;
								}
		}

		this->_elements.insert(pair<std::string, BSONContent* >(i->first, content));
	}
	if (orig._cBSON != NULL) {
		this->_cBSON = strcpy(orig._cBSON);
	} else {
		this->_cBSON = NULL;
	}
}

BSONTYPE BSONObj::type(std::string key) const {
	BSONContent* content = NULL;
	for (std::map<std::string, BSONContent* >::const_iterator it = _elements.begin(); it != _elements.end(); it++) {
		std::string itKey = it->first;
		if (itKey.compare(key) == 0) {
			content = it->second;
			break;
		}
	}
	if (content != NULL) {
		return content->type();
	} else {
		return UNKNOWN_TYPE;
	}
}

BSONContent* BSONObj::get(std::string key) const throw(BSONException) {
	BSONContent* content = NULL;
	for (std::map<std::string, BSONContent* >::const_iterator it = _elements.begin(); it != _elements.end(); it++) {
		std::string itKey = it->first;
		if (itKey.compare(key) == 0) {
			content = it->second;
			break;
		}
	}
	if (content != NULL) {
		return content;
	} else {
		throw BSONException(format("key not found %s", key.c_str()).c_str());
	}
}

BSONContent* BSONObj::getXpath(const std::string& xpath) const {
	__int32 posDot = xpath.find('.');
	BSONContent* result = NULL;
	if (posDot == string::npos) {
		result = getContent(xpath);
	} else {
		std::string path = xpath.substr(0, posDot);
		result = getContent(path);
		if ((result != NULL) && (result->type() == BSON_TYPE)) {
			BSONContentBSON* bcontent = (BSONContentBSON*)result;
			BSONObj* inner = (BSONObj*)*bcontent;
			result = inner->getXpath(xpath.substr(posDot + 1));
		}
	}

	if (result != NULL) {
		return result->clone();
	} else {
		return NULL;
	}
}

bool BSONObj::operator ==(const BSONObj& obj) const {
	if (this->has("_id") && obj.has("_id")) {
		BSONContent* idThis = this->getContent("_id");
		BSONContent* idOther = obj.getContent("_id");

		return (*idThis == *idOther);
	}
	// Element count
	if (this->length() != obj.length()) {
		return false;
	}
	for (BSONObj::const_iterator it = this->begin(); it != this->end(); it++) {
		std::string key = it->first;
		BSONContent* content = it->second;

		BSONContent* other = obj.getContent(key);
		if (other == NULL) {
			return false;
		}
		if (*content != *other) {
			return false;
		}
	}
	return true;
}

bool BSONObj::operator !=(const BSONObj& obj) const {
	if (this->has("_id") && obj.has("_id")) {
		BSONContent* idThis = this->getContent("_id");
		BSONContent* idOther = obj.getContent("_id");

		return (*idThis != *idOther);
	}

	// Element count
	if (this->length() != obj.length()) {
		return true;
	}
	for (BSONObj::const_iterator it = this->begin(); it != this->end(); it++) {
		std::string key = it->first;
		BSONContent* content = it->second;

		BSONContent* other = obj.getContent(key);
		if (other == NULL) {
			return true;
		}
		if (*content != *other) {
			return true;
		}
	}
	return false;
}

BSONObj* BSONObj::select(const char* sel) const {
	std::set<std::string> columns = bson_splitSelect(sel);
	bool include_all = (strcmp(sel, "*") == 0);

	BSONObj* result = new BSONObj();

	for (std::map<std::string, BSONContent* >::const_iterator i = this->_elements.begin(); i != this->_elements.end(); i++) {
		std::string key = i->first;
		if (include_all || (columns.find(key) != columns.end())) {
			BSONContent* origContent = i->second;

			switch (origContent->type()) {
				case BSON_TYPE:  
					{
						BSONContentBSON* bbson = (BSONContentBSON*)origContent;
						BSONObj* inner = (BSONObj*)*bbson;

						if (!include_all) {
							char* subselect = bson_subselect(sel, key.c_str());
							BSONObj* innerSubSelect = inner->select(subselect);
							result->add(key, *innerSubSelect);
							delete innerSubSelect;
						} else {
							result->add(key, *inner);
						}
						break;
					}
				case BSONARRAY_TYPE: 
					{
						BSONContentBSONArray* bbsonarray = (BSONContentBSONArray*)origContent;
						BSONArrayObj* innerArray = (BSONArrayObj*)*bbsonarray;
						if (!include_all) {
							char* subselect = bson_subselect(sel, key.c_str());
							BSONArrayObj* innerSubArray = innerArray->select(subselect);
							result->add(key, *innerSubArray);
							delete innerSubArray;
						} else {
							result->add(key, *innerArray);
						}
						break;
					}
				case INT_TYPE: 
					{
						BSONContentInt* bint = (BSONContentInt*)origContent;
						__int32 val = *bint;
						result->add(key, val);
						break;
					}
				case LONG_TYPE:
					{
						BSONContentLong* blong = (BSONContentLong*)origContent;
						__int64 val = *blong;
						result->add(key, val);
						break;
					}
				case DOUBLE_TYPE:
					{
						BSONContentDouble* bdouble = (BSONContentDouble*)origContent;
						double val = *bdouble;
						result->add(key, val);
						break;
					}
				case PTRCHAR_TYPE:
				case STRING_TYPE:
					{
						BSONContentString* bstring = (BSONContentString*)origContent;
						djondb::string str = *bstring;
						char* val = str.c_str();
						__int32 len = str.length();
						result->add(key, val, len);
						break;
					}
			}
		}
	}
	return result;
}

void BSONObj::remove(std::string kkey) {
	std::map<std::string, BSONContent* >::iterator i = _elements.find(kkey);
	if (i != _elements.end()) {
		// Removes the previous element
		BSONContent* current = i->second;
		delete current;
		_elements.erase(i);
	}

	// cleans up the cached toChar value
	if (_cBSON != NULL) {
		free(_cBSON);
		_cBSON = 0;
	}
}
