// =====================================================================================
//  Filename:  simpleexpression.cpp
// 
//  Description: Implementation of the "Constant" expressions, like ints, strings, double, etc
// 
//  Version:  1.0
//  Created:  04/24/2012 10:42:09 AM
//  Revision:  none
//  Compiler:  gcc
// 
//  Author:  YOUR NAME (), 
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

#include "constantexpression.h"
#include "bson.h"
#include "expressionresult.h"
#include <assert.h>

	ConstantExpression::ConstantExpression(char* expression)
:BaseExpression(ET_CONSTANT)
{
	_expression = strcpy(expression, strlen(expression));
	_intValue = NULL;
	_longValue = NULL;
	_doubleValue = NULL;
	_value = new ExpressionResult(djondb::string(_expression, strlen(_expression)));
}

	ConstantExpression::ConstantExpression(__int32 expression)
:BaseExpression(ET_CONSTANT)
{
	_expression = NULL;
	_intValue = new __int32();
	*_intValue = expression;
	_longValue = NULL;
	_doubleValue = NULL;
	_value = new ExpressionResult(*_intValue);
}

	ConstantExpression::ConstantExpression(__int64 expression)
:BaseExpression(ET_CONSTANT)
{
	_expression = NULL;
	_intValue = NULL;
	_longValue = new __int64();
	*_longValue = expression;
	_doubleValue = NULL;
	_value = new ExpressionResult(*_longValue);
}

	ConstantExpression::ConstantExpression(double expression)
:BaseExpression(ET_CONSTANT)
{
	_expression = NULL;
	_intValue = NULL;
	_doubleValue = new double();
	*_doubleValue = expression;
	_value = new ExpressionResult(*_doubleValue);
}

	ConstantExpression::ConstantExpression(const ConstantExpression& orig)
:BaseExpression(ET_CONSTANT)
{
	_expression = orig._expression;
	if (orig._value) {
		_value = new ExpressionResult(*orig._value);
	} else {
		_value = NULL;
	}
}

ConstantExpression::~ConstantExpression() {
	if (_value) {
		delete _value;
	}
	if (_expression != NULL) free(_expression);
	if (_intValue != NULL) delete _intValue;
	if (_doubleValue != NULL) delete _doubleValue;
	_value = NULL;
}

ExpressionResult* ConstantExpression::eval(const BSONObj& bson) {
	return new ExpressionResult(*_value); 
}

BaseExpression* ConstantExpression::copyExpression() {
	ConstantExpression* result;
	if (_expression != NULL) {
		result = new ConstantExpression(this->_expression);
	} else if (_intValue != NULL) {
		result = new ConstantExpression(*this->_intValue);
	} else if (_longValue != NULL) {
		result = new ConstantExpression(*this->_longValue);
	} else {
		result = new ConstantExpression(*this->_doubleValue);
	}
	return result;
}


