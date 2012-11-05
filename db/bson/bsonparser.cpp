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

#include "bsonparser.h"

#include "bsonobj.h"
#include "bsonarrayobj.h"
#include <stdio.h>
#include <limits.h>

struct BSONStruct {
	char* name;
	int type;
	void* value; struct BSONStruct* next;
};

BSONParser::BSONParser()
{
	//ctor
}

BSONParser::~BSONParser()
{
	//dtor
}

void freeMemory(struct BSONStruct* s) {
	while (s != NULL) {
		BSONStruct* temp = s->next;
		free(s->name);
		if (s->type == 4) { //BSONStruct
			freeMemory((struct BSONStruct*)s->value);
		} else {
			free(s->value);
		}
		free(s);
		s = temp;
	}
}

BSONObj* convertStruct(struct BSONStruct* param) {
	BSONObj* obj = new BSONObj();
	struct BSONStruct* s = param;
	BSONStruct* head = s;
	while (s != NULL) {
		std::string name(s->name);
		int type = s->type;
		void* value = s->value;
		int* val;
		float* val2;
		char* val3;
		BSONObj* inner;
		switch (type) {
			case 1:
				val = (int*)value;
				obj->add(name, *val);
				break;
			case 2:
				val2 = (float*)value;
				obj->add(name, *val2);
				break;
			case 3:
				val3 = (char*)value;
				obj->add(name, val3);
				break;
			case 4:
				struct BSONStruct* str = (struct BSONStruct*)value;
				inner = convertStruct(str);
				obj->add(name, *inner);
				delete inner;
		}
		s = s->next;
	}

	return obj;
}

BSONObj* BSONParser::parseBSON(const char* c, int& pos) throw(BSONException) {
	BSONObj* res = new BSONObj();
	int state = 0; // 0 - nothing, 1 - name, 2- value
	int lenBuffer = strlen(c);
	char* buffer = (char*)malloc(lenBuffer);
	char* name = NULL;
	void* value = NULL;
	int len = 0;
	BSONTYPE type;
	int stringOpen = 0; // 0 - closed
	// 1 - Single quote opened
	// 2 - Double quote opened	
	int x;
	for (x= pos; x < strlen(c); x++) {
		if (c[x] == '{') {
			if (state == 2) {
				value = parseBSON(c, x);
				type = BSON_TYPE;
			} else if (state == 0) {
				memset(buffer, 0, lenBuffer);
				state = 1;// name
				type = LONG64_TYPE;
			} else { // state == 1
				throw "json value is not allowed as name";
			}
			continue;
		}
		if (c[x] == '[') {
			value = parseArray(c, x);
			type = BSONARRAY_TYPE;
		}
		if (c[x] == '}' || c[x] == ',') {
			if (name != NULL) {
				if ((type != BSON_TYPE) && (type != BSONARRAY_TYPE)) {
					value = (char*)malloc(len+1); 
					memset(value, 0, len + 1);
					strcpy((char*)value, buffer);
				}
				len = 0;
				memset(buffer, 0, lenBuffer);
				switch (type) {
					case INT_TYPE:{
										  int iVal = atoi((char*)value);
										  res->add(name, iVal);
										  break;
									  }
					case LONG_TYPE: {
												long lVal = atol((char*)value);
												res->add(name, lVal);
												break;
											}
					case LONG64_TYPE: {
#ifdef WINDOWS
												__LONG64 lVal = _atoi64((char*)value);
#else
												__LONG64 lVal = atoll((char*)value);
#endif
												if (lVal <= INT_MAX) {
													res->add(name, (int)lVal);
												} else if (lVal <= LONG_MAX) {
													res->add(name, (long)lVal);
												} else {
													res->add(name, lVal);
												}
												break;
											}
					case DOUBLE_TYPE: {
												double dVal = atof((char*)value);
												res->add(name, dVal);
												break;
											}
					case STRING_TYPE:
											{
												res->add(name, (char*)value);
												break;
											}
					case BSON_TYPE:
											{
												res->add(name, *((BSONObj*)value));
												delete (BSONObj*)value;
												break;
											}
					case BSONARRAY_TYPE:
											{
												res->add(name, *((BSONArrayObj*)value));
												delete (BSONArrayObj*)value;
												break;
											}

				}
				free(name);
				name = NULL;
				if ((type != BSON_TYPE) && (type != BSONARRAY_TYPE)) {
					free(value);
					value = NULL;
				}
				if (c[x] == '}')
					break;
				else {
					state = 1; // name
					type = LONG64_TYPE;
					continue;
				}
			}
		}
		if (c[x] == ':') {
			name = (char*)malloc(len+1);
			memset(name, 0, len + 1);
			strcpy(name, buffer);
			len = 0;
			memset(buffer, 0, lenBuffer);
			state = 2; //value
			// default type
			type = LONG64_TYPE;
		} else {
			if (c[x] == '\'' || (c[x] == '\"')) {
				// Collect all the characters
				type = STRING_TYPE;
				char stringChar = c[x];
				bool escaped = false;
				x++;
				int startPos = x;
				while ((x < strlen(c)) && ((c[x] != stringChar) || (escaped))) {
					if (c[x] == '\\') {
						escaped = true;
					} else {
						escaped = false;
					}
					buffer[len] = c[x];
					len++;
					x++;
				}
				if (x >= strlen(c)) {
					char c[100];
					sprintf(c, "An error ocurred parsing the bson. Error: unclosed string at %d",  startPos);

					throw new BSONException(c);
				}
				continue;
			}

			if (c[x] == ' ' && stringOpen == 0) {
				continue;
			}
			if (c[x] == '\r' || c[x] == '\n') {
				continue;
			}
			if (c[x] == '.' && state == 2) {
				type = DOUBLE_TYPE;
			}
			buffer[len] = c[x];
			len++;
		}

	}
	pos = x;

	free(buffer);
	return res;
}

BSONArrayObj* BSONParser::parseArray(const std::string& sbson) {
	int pos = 0;
	return parseArray(sbson.c_str(), pos);
}

BSONArrayObj* BSONParser::parseArray(const char* chrs, int& pos) {
	BSONArrayObj* result = NULL;
	while (chrs[pos] == ' ') {
		pos++;
	}
	if (chrs[pos] != '[') {
		// error
	} else {
		result = new BSONArrayObj();
	}

	while ((pos < strlen(chrs)) && (chrs[pos] != ']')) {
		while ((pos < strlen(chrs)) && (chrs[pos] != ']') && (chrs[pos] != '{'))
			pos++;
		if (chrs[pos] == '{') {
			BSONObj* bson = parseBSON(chrs, pos);
			result->add(*bson);
			delete bson;
		}
	}	
	return result;
}

BSONObj* BSONParser::parse(const std::string& sbson) {

	const char* c = sbson.c_str();

	BSONObj* res = NULL;
	for (int x= 0; x < strlen(c); x++) {
		if (c[x] == '{') {
			res = parseBSON(c, x);
			break;
		}

	}
	return res;
}
