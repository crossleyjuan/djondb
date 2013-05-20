/*
 * =====================================================================================
 *
 *       Filename:  bsonutil.cpp
 *
 *    Description:  This contains utility functions for bson
 *
 *        Version:  1.0
 *        Created:  09/09/2012 07:22:40 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Juan Pablo Crossley (crossleyjuan@gmail.com), 
 *   Organization:  
 *
 * =====================================================================================
 */

#include "bsonutil.h"
#include "util.h"
#include <string.h>
#include <vector>
#include <stdlib.h>
#include <stdio.h>


std::set<std::string> bson_splitSelect(const char* select) {
	std::vector<std::string> elements = split(std::string(select), ",");

	std::set<std::string> result;
	for (std::vector<std::string>::const_iterator i = elements.begin(); i != elements.end(); i++) {
		std::string s = *i;
		char* cs = trim(const_cast<char*>(s.c_str()), s.length());
		if (startsWith(cs, "$")) {
			s = std::string(std::string(cs), 2, strlen(cs) - 3);
		} else {
			s = std::string(cs);
		}
		int dotPos = s.find('.');
		if (dotPos != string::npos) {
			s = s.substr(0, dotPos);
		}
		result.insert(s);
		free(cs);
	}

	return result;
}

char* bson_subselect(const char* select, const char* name) {
	std::vector<std::string> elements = split(std::string(select), ", ");
	char* result = (char*)malloc(strlen(select) + 1);
	memset(result, 0, strlen(select) + 1);
	int pos = 0;

	std::string startXpath = format("%s.", name);
	int lenStartXpath = startXpath.length();
	bool first = true;
	for (std::vector<std::string>::const_iterator i = elements.begin(); i != elements.end(); i++) {
		std::string selement = *i;
		char* element = const_cast<char*>(selement.c_str());
		element = trim(element, strlen(element));
		if (startsWith(element, "$")) {
			// Remvoes the $" " from the element
			element = strcpy(element, 2, strlen(element) - 3);
			if (startsWith(element, startXpath.c_str())) {
				if (!first) {
					memcpy(result + pos, ", ", 2);
					pos += 2;
				}
				char* suffix = strcpy(element, lenStartXpath, strlen(element) - lenStartXpath);
				memcpy(result + pos, "$\"", 2);
				pos+=2;
				memcpy(result + pos, suffix, strlen(suffix));
				pos += strlen(suffix);
				memcpy(result + pos, "\"", 1);
				pos++;
				first = false;
				free(suffix);
			}
		}
	}
	return result;
}

__int32* convertToInt(BSONTYPE type, void* val) {
	__int32* result = (__int32*)malloc(sizeof(__int32));
	switch (type) {
		case INT_TYPE:
			*result = *(int*)val;
			break;
		case DOUBLE_TYPE:
			*result = (__int32)*(double*)val;
			break;
		case LONG_TYPE:
		case LONG64_TYPE:
			*result = (__int32)*(__int64*)val;
			break;
		case PTRCHAR_TYPE:
			*result = atoi((char*)val);
			break;
		case STRING_TYPE:
			*result = atoi(((std::string*)val)->c_str());
			break;
		case NULL_TYPE:
			free(result);
			result = NULL;
			break;
		default:
			throw BSONException("Not convertible");
	}
	return result;
}

double* convertToDouble(BSONTYPE type, void* val) {
	double* result = (double*)malloc(sizeof(double));
	switch (type) {
		case INT_TYPE:
			*result = *(int*)val;
			break;
		case DOUBLE_TYPE:
			*result = (double)*(double*)val;
			break;
		case LONG_TYPE:
		case LONG64_TYPE:
			*result = (double)*(__int64*)val;
			break;
		case PTRCHAR_TYPE:
			*result = atof((char*)val);
			break;
		case STRING_TYPE:
			*result = atof(((std::string*)val)->c_str());
			break;
		case NULL_TYPE:
			free(result);
			result = NULL;
			break;
		default:
			throw BSONException("Not convertible");
	}
	return result;
}

__int64* convertToLong(BSONTYPE type, void* val) {
	__int64* result = (__int64*)malloc(sizeof(__int64));
	switch (type) {
		case INT_TYPE:
			*result = *(int*)val;
			break;
		case DOUBLE_TYPE:
			*result = (__int64)*(double*)val;
			break;
		case LONG_TYPE:
		case LONG64_TYPE:
			*result = (__int64)*(__int64*)val;
			break;
		case PTRCHAR_TYPE:
			*result = atol((char*)val);
			break;
		case STRING_TYPE:
			*result = atol(((std::string*)val)->c_str());
			break;
		case NULL_TYPE:
			free(result);
			result = NULL;
			break;
		default:
			throw BSONException("Not convertible");
	}
	return result;
}

char* convertToCharPtr(BSONTYPE type, void* val) {
	// Max size is 100
	char* result = (char*)malloc(100); 
	switch (type) {
		case INT_TYPE:
			sprintf(result, "%d", *(int*)val);
			break;
		case DOUBLE_TYPE:
			sprintf(result, "%f", *(double*)val);
			break;
		case LONG_TYPE:
		case LONG64_TYPE:
			sprintf(result, "%ld", *(__int64*)val);
			break;
		case PTRCHAR_TYPE:
			strcpy(result, (char*)val);
			break;
		case STRING_TYPE:
			strcpy(result, ((std::string*)val)->c_str());
			break;
		case NULL_TYPE:
			free(result);
			result = NULL;
			break;
		default:
			throw BSONException("Not convertible");
	}
	return result;
}

std::string* convertToStdString(BSONTYPE type, void* val) {
	// Max size is 100
	char* cresult = convertToCharPtr(type, val);

	std::string* result = NULL;
	if (cresult != NULL) {
		result = new std::string(cresult);
	}
	return result;
}

void* convert(BSONTYPE fromType, BSONTYPE toType, void* value) {
	void *result = NULL;
	switch (toType) {
		case INT_TYPE:
			result = convertToInt(fromType, (__int32*)value);
			break;
		case DOUBLE_TYPE:
			result = convertToDouble(fromType, (double*)value);
			break;
		case LONG_TYPE:
		case LONG64_TYPE:
			result = convertToLong(fromType, (__int64*)value);
			break;
		case PTRCHAR_TYPE:
			result = convertToCharPtr(fromType, (char*)value);
			break;
		case STRING_TYPE:
			result = convertToStdString(fromType, (std::string*)value);
			break;
		default:
			throw BSONException("Not convertible");
	}
	return result;
}
