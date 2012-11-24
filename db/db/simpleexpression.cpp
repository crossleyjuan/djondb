// =====================================================================================
//  Filename:  simpleexpression.cpp
// 
//  Description: Implementation of the "Simple" expressions, like simple xpaths
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
#include "simpleexpression.h"

#include "filterparser.h"
#include "expressionresult.h"
#include "bson.h"
#include <stdlib.h>

SimpleExpression::SimpleExpression(const char* expression)
	:BaseExpression(ET_SIMPLE)
{
	_expression = strcpy(const_cast<char*>(expression), 2, strlen(expression) - 3);
}

SimpleExpression::SimpleExpression(const SimpleExpression& orig)
	:BaseExpression(ET_SIMPLE)
{
	_expression = strcpy(orig._expression, strlen(orig._expression));
}

SimpleExpression::~SimpleExpression() {
	if (_expression != NULL) free(_expression);
}

ExpressionResult* SimpleExpression::eval(const BSONObj& bson) {
	BSONContent content = bson.getXpath(_expression);

	ExpressionResult::RESULT_TYPE type;
	ExpressionResult* result = NULL;
	switch (content.type()) {
		case INT_TYPE:
			{
				type = ExpressionResult::RT_INT;
				int i = (int)content;
				result = new ExpressionResult(type, &i);
				break;
			}
		case LONG_TYPE:
			{
				type = ExpressionResult::RT_LONG;
				long i = (long)content;
				result = new ExpressionResult(type, &i);
				break;
			}
		case LONG64_TYPE:
			{
				type = ExpressionResult::RT_LONG64;
				__LONG64 i = (__LONG64)content;
				result = new ExpressionResult(type, &i);
				break;
			}
		case DOUBLE_TYPE:
			{
				type = ExpressionResult::RT_DOUBLE;
				double d = (double)content;
				result = new ExpressionResult(type, &d);
				break;
			}
		case STRING_TYPE:
			{
				type = ExpressionResult::RT_STRINGDB;
				std::string s = (std::string)content;
				result = new ExpressionResult(type, &s);
				break;
			}
		case NULL_TYPE:
			{
				type = ExpressionResult::RT_NULL;
				result = new ExpressionResult(type, NULL);
				break;
			}
	}
	return result;
}

BaseExpression* SimpleExpression::copyExpression() {
	SimpleExpression* result = new SimpleExpression(this->_expression);
	return result;
}

const char* SimpleExpression::expression() const {
	return const_cast<const char*>(_expression);
}
