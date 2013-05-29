// =====================================================================================
//  Filename:  unaryexpression.cpp
// 
//  Description:  
// 
//  Version:  1.0
//  Created:  04/24/2012 10:42:18 AM
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

#include "filterparser.h"
#include "util.h"
#include "unaryexpression.h"
#include "simpleexpression.h"
#include "expressionresult.h"

UnaryExpression::UnaryExpression(FILTER_OPERATORS oper)
	:BaseExpression(ET_UNARY)
{
	_expression = NULL;
	_oper = oper;
}

UnaryExpression::UnaryExpression(const UnaryExpression& orig)
	:BaseExpression(ET_UNARY)
{
	if (orig._expression != NULL) {
		this->_expression = _expression->copyExpression();
	}
}

UnaryExpression::~UnaryExpression() {
	if (_expression) delete _expression;
}

ExpressionResult* exists(BaseExpression* expression, const BSONObj& bson) {
	ExpressionResult* result;
	if (expression->type() == ET_SIMPLE) {
		SimpleExpression* sexpre = (SimpleExpression*)expression;
		const char* expression = sexpre->expression();
		bool bres = bson.has(expression);
		result = new ExpressionResult(bres);
		return result;
	} else {
		throw ParseException(D_ERROR_PARSEERROR, "Exists signature is wrong. Use Exists($'field').");
	}
}

ExpressionResult* not_expression(BaseExpression* expression, const BSONObj& bson) {
	ExpressionResult* tmpresult = expression->eval(bson);
	ExpressionResult* result = NULL;
	if (tmpresult->type() == ExpressionResult::RT_BOOLEAN) {
		bool bres = *tmpresult;
		result = new ExpressionResult(!bres);	
	} else {
		throw ParseException(D_ERROR_PARSEERROR, "Exists signature is wrong. Use Exists($'field').");
	}
	delete tmpresult;
	return result;
}

ExpressionResult* UnaryExpression::eval(const BSONObj& bson) {
	ExpressionResult* result = NULL;
	switch (_oper) {
		case FO_NOT:
			result = not_expression(_expression, bson);
			break;
		case FO_EXISTS:
			result = exists(_expression, bson);
			break;
	}
	return result;
}

BaseExpression* UnaryExpression::copyExpression() {
	UnaryExpression* result = new UnaryExpression(_oper);
	if (_expression != NULL) {
		result->push(_expression);
	}
	return result;
}

void UnaryExpression::push(BaseExpression* expression) {
	if (_expression == NULL) {
		_expression = expression;
	} else {
		// ERROR
	}
}

