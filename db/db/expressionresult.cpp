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

ExpressionResult::ExpressionResult(RESULT_TYPE type, void* value) {
	_type = type;
	_value = copyValue(_type, value);
}

ExpressionResult::ExpressionResult(const ExpressionResult& orig) {
	this->_type = orig._type;
	this->_value = copyValue(this->_type, orig._value);
}

ExpressionResult::~ExpressionResult() {
	if (_value != NULL) {
		switch (_type) {
			case ExpressionResult::RT_INT:
				delete ((int*)_value);
				break;
			case ExpressionResult::RT_LONG:
				delete ((long*)_value);
				break;
			case ExpressionResult::RT_LONG64:
				delete ((__LONG64*)_value);
				break;
			case ExpressionResult::RT_DOUBLE:
				delete ((double*)_value);
				break;
			case ExpressionResult::RT_BOOLEAN:
				delete ((bool*)_value);
				break;
			case ExpressionResult::RT_BSON:
				delete ((BSONObj*)_value);
				break;
			case ExpressionResult::RT_STRINGDB:
				delete ((std::string*)_value);
				break;
			default:
				assert(false);
				break;
		}
	}
	_value = NULL;
}


ExpressionResult::RESULT_TYPE ExpressionResult::type() {
	return _type;
}

void* ExpressionResult::value() {
	return _value;
}




