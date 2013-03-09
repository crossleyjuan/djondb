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

#include "bsonbufferedobj.h"
#include "util.h"
#include "bsonutil.h"
#include "bsonbufferedcontent.hpp"

#include "bsonparser.h"
#include "bsonbufferedarrayobj.h"

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
BSONBufferedObj::BSONBufferedObj(char* buffer, __int64 len)
{
	_cBSON = NULL;
	_buffer = buffer;
	_bufferMaxLen = len;
	_keys = NULL;
	_keySize = NULL;
	_types = NULL;
	_values = NULL;
	_elements = 0;
	_bufferBSONLen = 0;

	initialize();
}

BSONBufferedObj::BSONBufferedObj(const BSONBufferedObj& orig) {
	cout << "BSONBufferedObj copying" << endl;
}

void BSONBufferedObj::reset(char* buffer, int len) {
	_cBSON = NULL;
	_buffer = buffer;
	_bufferMaxLen = len;
	if (_keys != NULL) {
		free(_keys);
	}
	_keys = NULL;
	if (_keySize != NULL) {
		free(_keySize);
	}
	_keySize = NULL;
	if (_types != NULL) {
		free(_types);
	}
	_types = NULL;
	if (_values != NULL) {
		free(_values);
	}
	_values = NULL;
	_elements = 0;
	_bufferBSONLen = 0;

	initialize();
}

BSONBufferedObj::~BSONBufferedObj()
{
	if (_keys != NULL) free(_keys);
   if (_keySize != NULL) free(_keySize);
	if (_types != NULL) { 
		for (int x = 0; x < _elements; x++) {
			BSONTYPE type = (BSONTYPE)*_types[x];
			if (type == BSON_TYPE) {
				BSONBufferedObj* o = (BSONBufferedObj*)_values[x];
				if (o != NULL) {
					delete o;
				}
				_values[x] = NULL;
			} else if (type == BSONARRAY_TYPE) {
				BSONBufferedArrayObj* array = (BSONBufferedArrayObj*)_values[x];
				if (array != NULL) {
					delete array;
				}
				_values[x] = NULL;
			}
		}
		free(_types);
	}
	if (_values != NULL) free(_values);
}

char* BSONBufferedObj::getValue(char* key) const {
	char* testKey;
	__int32 keyLen = strlen(key);
	for (int i = 0; i < _elements; i++) {
		__int32 testkeyLen = *(__int32*)_keySize[i];
		// this avoids problems when one key fits into the other
		if (keyLen == testkeyLen) {
			testKey = _keys[i];
			if (strncmp(testKey, key, testkeyLen) == 0) {
				char* value = _values[i];
				return value;
			}
		}
	}
	return NULL;
}

BSONTYPE BSONBufferedObj::getType(char* key) const {
	char* testKey;
	for (int i = 0; i < _elements; i++) {
		__int32 keyLen = *(__int32*)_keySize[i];
		testKey = _keys[i];
		if (strncmp(testKey, key, keyLen) == 0) {
			__int32 type = *_types[i];
			BSONTYPE btype = (BSONTYPE)type;
			if (btype == STRING_TYPE) {
				btype = PTRCHAR_TYPE;
			}
			return btype;
		}
	}
	throw BSONException(format("key not found %s", key).c_str());
}

void BSONBufferedObj::initialize() {
	Logger* log = getLogger(NULL);
	if (_keys != NULL) {
		// Cleans the previous allocated elements
		free(_keys);
		free(_keySize);
		free(_values);
		free(_types);
	}
	char* pCurrentElement = _buffer;
	this->_elements = readData<__int64>(pCurrentElement);

	//pCurrentElement += sizeof(__int64);
	_bufferBSONLen += sizeof(__int64);

	_keys = (char**)malloc(sizeof(char*) * _elements);
	_keySize = (__int32**)malloc(sizeof(__int32*) * _elements);
	_values = (char**)malloc(sizeof(char*) * _elements);
	_types = (__int64**)malloc(sizeof(__int64*) * _elements);

	for (__int32 x = 0; x < _elements; x++) {
		_keySize[x] = (__int32*)pCurrentElement;
		pCurrentElement += sizeof(__int32);
		_bufferBSONLen += sizeof(__int32);
		_keys[x] = pCurrentElement;
		pCurrentElement += *_keySize[x];
		_bufferBSONLen += *_keySize[x];

		_types[x] = (__int64*)pCurrentElement;
		pCurrentElement += sizeof(__int64);
		_bufferBSONLen += sizeof(__int64);

		void* data = NULL;
		BSONObj* inner;
		bool include = false;
		_values[x] = pCurrentElement;
		switch (*_types[x]) {
			case INT_TYPE: {
									pCurrentElement += sizeof(__int32);
									_bufferBSONLen += sizeof(__int32);
									break;
								};
			case LONG_TYPE: 
			case LONG64_TYPE: {
										pCurrentElement += sizeof(__int64);
										_bufferBSONLen += sizeof(__int64);
										break;
									}
			case DOUBLE_TYPE: {
										pCurrentElement += sizeof(double);
										_bufferBSONLen += sizeof(double);
										break;
									}
			case PTRCHAR_TYPE: 
			case STRING_TYPE: {
										__int32 len = *(__int32*)pCurrentElement;
										pCurrentElement += sizeof(__int32);
										_bufferBSONLen += sizeof(__int32);
										pCurrentElement += len;
										_bufferBSONLen += len;
										break;
									}
			case BSON_TYPE: {
									 BSONBufferedObj* inner = new BSONBufferedObj(pCurrentElement, _bufferMaxLen - _bufferBSONLen);
									 pCurrentElement += inner->bufferLength();
									 _bufferBSONLen += inner->bufferLength();
									 _values[x] = (char*)inner;
									 break;
								 }
			case BSONARRAY_TYPE: {
											BSONBufferedArrayObj* innerArray = new BSONBufferedArrayObj(pCurrentElement, _bufferMaxLen - _bufferBSONLen);
											pCurrentElement += innerArray->bufferLength();
											_bufferBSONLen += innerArray->bufferLength();
											_values[x] = (char*)innerArray;
											break;
										}
		}
	}
}

__int64 BSONBufferedObj::bufferLength() const {
	return _bufferBSONLen;
}

char* BSONBufferedObj::toChar() {
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

	char* key = (char*)malloc(100);
	int maxKeySize = 100;
	for (int i = 0; i < _elements; i++) {
		if (!first) {
			result[pos] = ',';
			pos++;
		}
		first = false;
		if (*_keySize[i] > maxKeySize) {
			maxKeySize = *_keySize[i];
			key = (char*)realloc(key, maxKeySize);
		}
		memset(key, 0, maxKeySize);
		key = strncpy(key, _keys[i], *_keySize[i]);
		sprintf(result + pos, " \"%.*s\" : ", *_keySize[i], key);
		pos += *_keySize[i] + 6;
		//ss << "\"" << key << "\" :";
		char* chr;
		const char* cstr;
		__int64 type = *_types[i];
		switch (type)  {
			case BSON_TYPE: {
									 BSONBufferedObj* obj = (BSONBufferedObj*)_values[i];
									 sprintf(result + pos, "%s", obj->toChar());
									 break;
								 }
								 /* 
									 case BSONARRAY_TYPE:
									 chr = ((BSONArrayObj*)content->_element)->toChar();
									 sprintf(result + pos, "%s", chr);
									 free(chr);
									 break;
									 */
			case INT_TYPE: {
									__int32 val = *(__int32*)_values[i];
									sprintf(result + pos, "%d", val);
									break;
								}
			case LONG_TYPE: {
									 __int64 val = *(__int64*)_values[i];
									 sprintf(result + pos, "%ld", val);
									 break;
								 }
			case DOUBLE_TYPE: {
										double val = *(double*)_values[i];
										sprintf(result + pos, "%f", val);
										break;
									}
			case STRING_TYPE:
			case PTRCHAR_TYPE: {
										 char* element = (char*)_values[i];
										 __int32 lenElement = *(__int32*)element;
										 element += sizeof(__int32);

										 sprintf(result + pos, "\"%.*s\"", lenElement, element);
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

__int32 BSONBufferedObj::getInt(std::string key) const throw(BSONException) {
	char* value = getValue(const_cast<char*>(key.c_str()));
	if (value == NULL) {
		throw BSONException(format("key not found %s", key.c_str()).c_str());
	} else {
		return *(__int32*)value;
	}
}

double BSONBufferedObj::getDouble(std::string key) const throw(BSONException) {
	char* value = getValue(const_cast<char*>(key.c_str()));
	if (value == NULL) {
		throw BSONException(format("key not found %s", key.c_str()).c_str());
	} else {
		return *(double*)value;
	}
}

__int64 BSONBufferedObj::getLong(std::string key) const throw(BSONException) {
	char* value = getValue(const_cast<char*>(key.c_str()));
	if (value == NULL) {
		throw BSONException(format("key not found %s", key.c_str()).c_str());
	} else {
		return *(__int64*)value;
	}
}

const djondb::string BSONBufferedObj::getDJString(std::string key) const throw(BSONException) {
	char* value = getValue(const_cast<char*>(key.c_str()));
	if (value == NULL) {
		throw BSONException(format("key not found %s", key.c_str()).c_str());
	} else {
		__int32 len = *(__int32*)value;
		const char* result = value + sizeof(__int32);
		return djondb::string(result, len);
	}
}

BSONBufferedObj* BSONBufferedObj::getBSON(std::string key) const throw(BSONException) {
	char* value = getValue(const_cast<char*>(key.c_str()));
	if (value == NULL) {
		throw BSONException(format("key not found %s", key.c_str()).c_str());
	} else {
		BSONBufferedObj* result = (BSONBufferedObj*)value;
		return result;
	}
}

BSONArrayObj* BSONBufferedObj::getBSONArray(std::string key) const throw(BSONException) {
	char* value = getValue(const_cast<char*>(key.c_str()));
	if (value == NULL) {
		throw BSONException(format("key not found %s", key.c_str()).c_str());
	} else {
		BSONBufferedArrayObj* result = (BSONBufferedArrayObj*)value;
		return result;
	}
}

BSONContent* BSONBufferedObj::getContent(std::string key) const {
	char* value = getValue(const_cast<char*>(key.c_str()));

	if (value != NULL) {
		BSONTYPE type = getType(const_cast<char*>(key.c_str()));
		__int32 len = 0;
		switch (type) {
			case BSON_TYPE:
				{
					len = ((BSONBufferedObj*)value)->bufferLength();
					break;
				}
			case BSONARRAY_TYPE:
			case INT_TYPE:
				len = sizeof(__int32);
				break;
			case LONG_TYPE:
			case LONG64_TYPE:
				len = sizeof(__int64);
				break;
			case DOUBLE_TYPE:
				len = sizeof(double);
				break;
			case PTRCHAR_TYPE:
			case STRING_TYPE: {
										len = *(__int32*)value;
										value += sizeof(__int32);
										break;
									}
		}
		BSONBufferedContent* content = new BSONBufferedContent(value, type, len);
		return content;
	} else {
		return new BSONBufferedContent();
	}
}

BSONContent* BSONBufferedObj::getContent(std::string key, BSONTYPE ttype) const {
	throw BSONException("Unsupported method");
}

BSONBufferedObj::const_iterator BSONBufferedObj::begin() const {
	throw BSONException("Unsupported method");
}

BSONBufferedObj::const_iterator BSONBufferedObj::end() const {
	throw BSONException("Unsupported method");
}

__int32 BSONBufferedObj::length() const {
	return _elements;
}

bool BSONBufferedObj::has(std::string key) const {
	char* value = getValue(const_cast<char*>(key.c_str()));
	return (value != NULL);
}

BSONTYPE BSONBufferedObj::type(std::string key) const {
	return getType(const_cast<char*>(key.c_str()));
}

BSONContent* BSONBufferedObj::get(std::string key) const throw(BSONException) {
	throw BSONException("Unsupported method");
}

BSONContent* BSONBufferedObj::getXpath(const std::string& xpath) const {
	std::string cpyXpath = xpath;
	__int32 posDot = cpyXpath.find('.');
	std::string path;
	if (posDot != string::npos) {
		path = cpyXpath.substr(0, posDot);
		cpyXpath = cpyXpath.substr(posDot + 1);
	} else {
		path = cpyXpath;
		cpyXpath = "";
	}
	BSONBufferedContent* content = (BSONBufferedContent*)getContent(path);
	if ((content->type() == BSON_TYPE) && (xpath.length() > 0)) {
		BSONBufferedObj* obj = (BSONBufferedObj*)content->element();
		BSONBufferedContent* innerContent = (BSONBufferedContent*)obj->getXpath(cpyXpath);
		delete content;
		content = innerContent;
	}
	return content;
}

bool BSONBufferedObj::operator ==(const BSONBufferedObj& obj) const {
	throw BSONException("Unsupported method");
}

bool BSONBufferedObj::operator !=(const BSONBufferedObj& obj) const {
	throw BSONException("Unsupported method");
}

BSONObj* BSONBufferedObj::select(const char* sel) const {
	std::set<std::string> columns = bson_splitSelect(sel);
	bool include_all = (strcmp(sel, "*") == 0);

	BSONObj* result = new BSONObj();

	for (int x = 0; x < _elements; x++) {
		std::string key(_keys[x], *_keySize[x]);
		if (include_all || (columns.find(key) != columns.end())) {
			BSONTYPE type = getType(const_cast<char*>(key.c_str()));

			switch (type) {
				case BSON_TYPE:  
					{
						BSONBufferedObj* inner = getBSON(key);

						char* subselect = "*";
						if (!include_all) {
							subselect = bson_subselect(sel, key.c_str());
						}
						BSONObj* innerSubSelect = inner->select(subselect);
						result->add(key, *innerSubSelect);
						delete innerSubSelect;
						break;
					}
				case BSONARRAY_TYPE: 
					{
						BSONArrayObj* innerArray = getBSONArray(key);
						char* subselect = "*";
						if (!include_all) {
							subselect = bson_subselect(sel, key.c_str());
						}
						BSONArrayObj* innerSubArray = innerArray->select(subselect);
						result->add(key, *innerSubArray);
						delete innerSubArray;
						break;
					}
				case INT_TYPE: 
					{
						__int32 val = getInt(key);
						result->add(key, val);
						break;
					}
				case LONG_TYPE:
					{
						__int64 val = getLong(key);
						result->add(key, val);
						break;
					}
				case LONG64_TYPE:
					{
						__LONG64 val = getLong(key);
						result->add(key, val);
						break;
					}
				case DOUBLE_TYPE:
					{
						double val = getDouble(key);
						result->add(key, val);
						break;
					}
				case STRING_TYPE:
				case PTRCHAR_TYPE:
					{
						djondb::string val = getDJString(key);
						result->add(key, const_cast<char*>(val.c_str()), val.length());
						break;
					}
			}
		}
	}
	return result;
}

