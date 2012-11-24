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

#include "bsoncontent.h"
#include "bsonobj.h"
#include "bsonarrayobj.h"
#include "util.h"
#include <stdlib.h>
#include <assert.h>

using namespace std;

BSONContent::BSONContent() {
	this->_element = NULL;
	this->_type = NULL_TYPE;
}

BSONContent::~BSONContent() {
	if (!_element) {
		return;
	}
	switch (_type) {
		case STRING_TYPE:
			delete ((string*)_element);
			break;
		case INT_TYPE:
			delete ((int*)_element);
			break;
		case LONG_TYPE:
			delete ((long*)_element);
			break;
		case LONG64_TYPE:
			delete ((__LONG64*)_element);
			break;
		case DOUBLE_TYPE:
			delete ((double*)_element);
			break;
		case BSON_TYPE:
			delete ((BSONObj*)_element);
			break;
		case BSONARRAY_TYPE:
			delete ((BSONArrayObj*)_element); 
			break;
		default:
			break;
	}
	_element = 0;
}

BSONContent::BSONContent(const BSONContent& orig) {
	this->_type = orig._type;
	int len = 0;
	int* internalInt;
	long* internalLong;
	double* internalDouble;
	int i;
	long l;
	double d;
	BSONObj* bson;
	BSONObj* internalBson;
	BSONArrayObj* bsonArray;
	BSONArrayObj* internalBsonArray;
	switch (this->_type) {
		case STRING_TYPE:
			this->_element = new std::string(*(std::string*)orig._element);
			break;
		case INT_TYPE:
			i = *((int*)orig._element);
			internalInt = new int();
			*internalInt = i;
			this->_element = internalInt;
			break;
		case LONG_TYPE:
			l = *((long*)orig._element);
			internalLong = new long();
			*internalLong = l;
			this->_element = internalLong;
			break;
		case LONG64_TYPE:
			{
				__LONG64 ll = *((__LONG64*)orig._element);
				__LONG64* longlong = new __LONG64();
				*longlong = ll;
				this->_element = longlong;
				break;
			}
		case DOUBLE_TYPE:
			d = *((double*)orig._element);
			internalDouble = new double();
			*internalDouble = d;
			this->_element = internalDouble;
			break;
		case BSON_TYPE:
			bson = (BSONObj*)orig._element;
			internalBson = new BSONObj(*bson);
			this->_element = internalBson;
			break;
		case BSONARRAY_TYPE:
			bsonArray = (BSONArrayObj*)orig._element;
			internalBsonArray = new BSONArrayObj(*bsonArray);
			this->_element = internalBsonArray;
			break;
		default:
			break;
	}
}


bool BSONContent::operator ==(const BSONContent& content) {
	if (this->_type != content._type) {
		return false;
	} else {
		void* cont1 = this->_element;
		void* cont2 = content._element;
		// if the pointers are equal it's pointless to compare the contents
		if (cont1 == cont2) {
			return true;
		} else {
			bool result;
			switch (this->_type) {
				case STRING_TYPE:
					result = ((std::string*)cont1)->compare(*(std::string*)cont2) == 0;
					break;
				case INT_TYPE: 
					{
						int i1 = *(int*)cont1;
						int i2 = *(int*)cont2;
						result = (i1 == i2);
					}
					break;
				case LONG_TYPE:
					{
						long l1 = *(long*)cont1;
						long l2 = *(long*)cont2;
						result = (l1 == l2);
					}
					break;
				case LONG64_TYPE:
					{
						__LONG64 l1 = *(__LONG64*)cont1;
						__LONG64 l2 = *(__LONG64*)cont2;
						result = (l1 == l2);
					}
					break;
				case DOUBLE_TYPE:
					{
						long l1 = *(long*)cont1;
						long l2 = *(long*)cont2;
						result = (l1 == l2);
					}
					break;
				case BSON_TYPE:
					{
						BSONObj* obj1 = (BSONObj*)cont1;
						BSONObj* obj2 = (BSONObj*)cont2;
						result = (*obj1 == *obj2);
					}
				case BSONARRAY_TYPE:
					result = false;
					break;
				default:
					break;
			}
			return result;
		}	
	}
}

bool BSONContent::operator !=(const BSONContent& content) {
	if (this->_type != content._type) {
		return true;
	} else {
		void* cont1 = this->_element;
		void* cont2 = content._element;
		// if the pointers are equal it's pointless to compare the contents
		if (cont1 == cont2) {
			return false;
		} else {
			bool result;
			switch (this->_type) {
				case STRING_TYPE:
					result = ((std::string*)cont1)->compare(*(std::string*)cont2) != 0;
					break;
				case INT_TYPE: 
					{
						int i1 = *(int*)cont1;
						int i2 = *(int*)cont2;
						result = (i1 != i2);
					}
					break;
				case LONG_TYPE:
					{
						long l1 = *(long*)cont1;
						long l2 = *(long*)cont2;
						result = (l1 != l2);
					}
					break;
				case LONG64_TYPE:
					{
						__LONG64 l1 = *(__LONG64*)cont1;
						__LONG64 l2 = *(__LONG64*)cont2;
						result = (l1 != l2);
					}
					break;
				case DOUBLE_TYPE:
					{
						long l1 = *(long*)cont1;
						long l2 = *(long*)cont2;
						result = (l1 != l2);
					}
					break;
				case BSON_TYPE:
					{
						BSONObj* obj1 = (BSONObj*)cont1;
						BSONObj* obj2 = (BSONObj*)cont2;

						result = (*obj1 != *obj2);
					}
				case BSONARRAY_TYPE:
					return false;
					break;
				default:
					break;
			}
			return result;
		}	
	}
}

BSONContent::operator int() {
	assert(_type == INT_TYPE);
	int* content = (int*)_element;
	return *content; 
}

BSONContent::operator long() {
	assert(_type == LONG_TYPE);
	long* content = (long*)_element;
	return *content; 

}

BSONContent::operator __LONG64() {
	assert(_type == LONG64_TYPE);
	__LONG64* content = (__LONG64*)_element;
	return *content; 

}

BSONContent::operator double() {
	assert(_type == DOUBLE_TYPE);
	double* content = (double*)_element;
	return *content; 
}
/*
 * To make the user of this library easier the char* will not be implemented, use std::string instead
 * otherwise the user should handle the free of the char*
 BSONContent::operator char*() {
 assert(_type == PTRCHAR_TYPE);
 char* content = (char*)_element;
 char* result = (char*)malloc(strlen(content + 1));
 memset(result, 0, strlen(content + 1));
 memcpy(result, content, strlen(content));
 return result; 
 }
 */

BSONContent::operator std::string() {
	assert(_type == STRING_TYPE);
	std::string* content = (std::string*)_element;
	return *content; 
}

BSONContent::operator BSONObj() {
	assert(_type == BSON_TYPE);
	BSONObj* content = (BSONObj*)_element;
	return *content; 
}

BSONContent::operator BSONArrayObj() {
	assert(_type == BSONARRAY_TYPE);
	BSONArrayObj* content = (BSONArrayObj*)_element;
	return *content; 
}
