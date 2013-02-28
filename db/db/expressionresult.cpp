/*
 * =====================================================================================
 *
 *       Filename:  expressionresult.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/24/2012 10:14:02 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include "expressionresult.h"
#include "filterparser.h"
#include "bson.h"
#include <assert.h>

void* copyValue(ExpressionResult::RESULT_TYPE type, void* value) {
	void* result = NULL;
	switch (type) {
		case ExpressionResult::RT_INT:
			{
				int* i = new int();
				*i = *(int*)value;
				result = i;
				break;
			}
		case ExpressionResult::RT_LONG:
			{
				long* l = new long();
				*l = *(long*)value;
				result = l;
				break;
			}
		case ExpressionResult::RT_LONG64:
			{
				__LONG64* l = new __LONG64();
				*l = *(__LONG64*)value;
				result = l;
				break;
			}
		case ExpressionResult::RT_DOUBLE:
			{
				double* d = new double();
				*d = *(double*)value;
				result = d;
				break;
			}
		case ExpressionResult::RT_BOOLEAN:
			{
				bool* b = new bool();
				*b = *(bool*)value;
				result = b;
				break;
			}
		case ExpressionResult::RT_BSON:
			{
				BSONObj* o = new BSONObj(*(BSONObj*)value);
				result = o;
				break;
			}

		case ExpressionResult::RT_PTRCHAR:
			{
				char* s = (char*)value;
				result = s;
				break;
			}
		case ExpressionResult::RT_STRINGDB:
			{
				std::string* s = new std::string(*(std::string*)value);
				result = s;
				break;
			}
		case ExpressionResult::RT_NULL:
			break;
		default:
			assert(false);
			break;
	}

	return result;
}

ExpressionResult::ExpressionResult() {
	_type = RT_NULL;
	_ivalue = 0;
	_lvalue = 0;
	_dvalue = 0;
	_bvalue = false;
	_bsonvalue = NULL;	
}

ExpressionResult::ExpressionResult(__int32 value) {
	_type = RT_INT;
	_ivalue = value;
	_lvalue = 0;
	_dvalue = 0;
	_bvalue = false;
	_bsonvalue = NULL;	
}

ExpressionResult::ExpressionResult(__int64 value) {
	_type = RT_LONG;
	_ivalue = 0;
	_lvalue = value;
	_dvalue = 0;
	_bvalue = false;
	_bsonvalue = NULL;	
}

ExpressionResult::ExpressionResult(double value) {
	_type = RT_DOUBLE;
	_ivalue = 0;
	_lvalue = 0;
	_dvalue = value;
	_bvalue = false;
	_bsonvalue = NULL;	
}

ExpressionResult::ExpressionResult(bool value) {
	_type = RT_BOOLEAN;
	_ivalue = 0;
	_lvalue = 0;
	_dvalue = 0;
	_bvalue = value;
	_bsonvalue = NULL;	
}

ExpressionResult::ExpressionResult(std::string value) {
	_type = RT_STRINGDB;
	_ivalue = 0;
	_lvalue = 0;
	_dvalue = 0;
	_bvalue = false;
	_svalue = value;
	_bsonvalue = NULL;	
}

ExpressionResult::ExpressionResult(djondb::string value) {
	_type = RT_PTRCHAR;
	_ivalue = 0;
	_lvalue = 0;
	_dvalue = 0;
	_bvalue = false;
	_dsvalue = value;
	_bsonvalue = NULL;	
}

ExpressionResult::ExpressionResult(BSONObj* value) {
	_type = RT_BSON;
	_ivalue = 0;
	_lvalue = 0;
	_dvalue = 0;
	_bvalue = false;
	_bsonvalue = new BSONObj(*value);
}

ExpressionResult::ExpressionResult(const ExpressionResult& orig) {
		this->_ivalue = orig._ivalue;
		this->_lvalue = orig._lvalue;
		this->_dvalue = orig._dvalue;
		this->_bvalue = orig._bvalue;
		this->_svalue = orig._svalue;
		this->_dsvalue = orig._dsvalue;
		if (orig._bsonvalue != NULL) {
			this->_bsonvalue = new BSONObj(*orig._bsonvalue);
		} else {
			this->_bsonvalue = NULL;
		}

		this->_type = orig._type;
}

ExpressionResult::~ExpressionResult() {
	if (_bsonvalue != NULL) {
		delete _bsonvalue;
	}
}


ExpressionResult::RESULT_TYPE ExpressionResult::type() {
	return _type;
}

ExpressionResult::operator __int32() {
	if (_type == RT_INT) {
		return _ivalue;
	} else {
		throw "Invalid cast";
	}
}

ExpressionResult::operator __int64() {
	if ((_type == RT_LONG) || (_type == RT_LONG64)) {
		return _lvalue;
	} else {
		throw "Invalid cast";
	}
}

ExpressionResult::operator double() {
	if (_type == RT_DOUBLE) {
		return _dvalue;
	} else {
		throw "Invalid cast";
	}
}

ExpressionResult::operator bool() {
	if (_type == RT_BOOLEAN) {
		return _bvalue;
	} else {
		throw "Invalid cast";
	}
}

ExpressionResult::operator string() {
	if (_type == RT_STRINGDB) {
		return _svalue;
	} else {
		throw "Invalid cast";
	}
}

ExpressionResult::operator djondb::string() {
	if (_type == RT_PTRCHAR) {
		return _dsvalue;
	} else {
		throw "Invalid cast";
	}
}

ExpressionResult::operator BSONObj*() {
	if (_type == RT_BSON) {
		return _bsonvalue;
	} else {
		throw "Invalid cast";
	}
}

