// =====================================================================================
//  Filename:  expressionresult.h
// 
//  Description:  
// 
//  Version:  1.0
//  Created:  07/17/2012 08:45:15 AM
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
#ifndef EXPRESSIONRESULT_INCLUDED_H
#define EXPRESSIONRESULT_INCLUDED_H

#include "filterdefs.h"

class ExpressionResult {
	public: 
		enum RESULT_TYPE {
			RT_INT,
			RT_DOUBLE,
			RT_LONG,
			RT_LONG64,
			RT_BOOLEAN,
			RT_STRINGDB,
			RT_BSON,
			RT_NULL
		};
	public:
		ExpressionResult(RESULT_TYPE type, void* value);
		ExpressionResult(const ExpressionResult& orig);
		~ExpressionResult();

		RESULT_TYPE type();
		void* value();

	private:
		void* _value;
		RESULT_TYPE _type;
};

#endif // EXPRESSIONRESULT_INCLUDED_H
