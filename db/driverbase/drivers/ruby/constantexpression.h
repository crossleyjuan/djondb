// =====================================================================================
//  Filename:  constantexpression.h
// 
//  Description:  <F7>
// 
//  Version:  1.0
//  Created:  07/17/2012 08:43:09 AM
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
#ifndef CONSTANTEXPRESSION_INCLUDE_H
#define CONSTANTEXPRESSION_INCLUDE_H

#include "baseexpression.h"
#include "util.h"

class ExpressionResult;

class ConstantExpression: public BaseExpression {
	public:
		ConstantExpression(const char* expression);
		ConstantExpression(int expression);
		ConstantExpression(long expression);
		ConstantExpression(__LONG64 expression);
		ConstantExpression(double expression);
		ConstantExpression(const ConstantExpression& orig);
		virtual ~ConstantExpression();

		virtual ExpressionResult* eval(const BSONObj& bson);
		virtual BaseExpression* copyExpression();
	private:
		std::string* _expression;
		int* _intValue;
		long* _longValue;
		__LONG64* _long64Value;
		double* _doubleValue;
		ExpressionResult* _value;

	private:
		void parseConstantExpression();
};


#endif // CONSTANTEXPRESSION_INCLUDE_H
