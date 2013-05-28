/*
 * =====================================================================================
 *
 *       Filename:  filterdefs.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  07/17/2012 10:11:56 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include "filterdefs.h"

FILTER_OPERATORS parseFilterOperator(const char* text) {
	FILTER_OPERATORS result = FO_NONE;
	if (compareInsensitive(text, "or")) {
		result = FO_OR;
	} else if (compareInsensitive(text, "and")) {
		result = FO_AND;
	} else if (compareInsensitive(text, "!=")) {
		result = FO_NOT_EQUALS;
	} else if (compareInsensitive(text, "==")) {
		result = FO_EQUALS;
	} else if (compareInsensitive(text, ">")) {
		result = FO_GREATERTHAN;
	} else if (compareInsensitive(text, ">=")) {
		result = FO_GREATEREQUALTHAN;
	} else if (compareInsensitive(text, "<")) {
		result = FO_LESSTHAN;
	} else if (compareInsensitive(text, "<=")) {
		result = FO_LESSEQUALTHAN;
	}
	return result;
};

ParseException::ParseException(int code, const char* error) {
	_errorCode = code;
	_errorMessage = error;
}

ParseException::ParseException(const ParseException& orig) {
	this->_errorCode = orig._errorCode;
	this->_errorMessage = orig._errorMessage;
}

const char* ParseException::what() const throw() {
	return _errorMessage;
}

int ParseException::errorCode() const {
	return _errorCode;
}

