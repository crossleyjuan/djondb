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

#include "stringfunctions.h"
#include "util.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <sstream>
#include <boost/algorithm/string.hpp>

djondb::string::string() {
	_holder = NULL;
	_len = 0;
}

djondb::string::string(char* c, __int32 len) {
	_holder = new stringHolder(c);
	_len = len;
}

djondb::string::string(const char* c, __int32 len) {
	_holder = new stringHolder(c);
	_len = len;
}

djondb::string::string(const djondb::string& orig) {
	this->_holder = orig._holder;
	if (this->_holder != NULL) {
		this->_holder->addReference();
	}
	this->_len = orig._len;
}

djondb::string& djondb::string::operator=(const djondb::string& rvar) {
	this->_holder = rvar._holder;
	if (this->_holder != NULL) {
		this->_holder->addReference();
	}
	this->_len = rvar._len;
	return *this;
}

djondb::string::~string() {
	if (this->_holder != NULL) {
		this->_holder->removeReference();
		if (this->_holder->references() == 0) {
			delete this->_holder;
		}
	}
}

std::string djondb::string::str() const {
	char* chr = (char*)malloc(length() + 1);
	strncpy(chr, (char*)*_holder, length());
	chr[length()] = 0;
	std::string result(chr);

	free(chr);
	return result;
}

const char* djondb::string::c_str() const {
	if (_holder != NULL) {
		return const_cast<const char*>((char*)*_holder);
	} else {
		return NULL;
	}
}

int djondb::string::compare(const djondb::string s2) const {
	return compare(s2.c_str(), s2.length());
}

int djondb::string::compare(const char* s2, __int32 len) const {
    const char* s1 = (const char*)*_holder;
    // the null char should not be compared to avoid unexpected results on buffers
	 if (_len < len) {
		 return -1;
	 } else if (_len > len) {
		 return 1;
	 }
    return strncmp(s1, s2, _len);
}

__int32 djondb::string::length() const {
	return _len;
}

djondb::string::operator const char*() const {
	return *_holder;
}

djondb::string::operator char*() const {
	return *_holder;
}

bool djondb::string::operator ==(const djondb::string& str) {
	if (this->_len != str._len) {
		return false;
	} else {
		if (strncmp(c_str(), str.c_str(), this->_len) == 0) {
			return true;
		} else {
			return false;
		}
	}
}

bool djondb::string::operator !=(const djondb::string& str) {
	if (this->_len != str._len) {
		return true;
	} else {
		if (strncmp(c_str(), str.c_str(), this->_len) == 0) {
			return false;
		} else {
			return true;
		}
	}
}

char* strcpy(std::string str) {
	return strcpy(const_cast<char*>(str.c_str()), str.length());
}

char* strcpy(char* str, int offset, int len) {
	char* result = (char*)malloc(len + 1);
	memset(result, 0, len +1);
	memcpy(result, str + offset, len);
	return result;
}

char* strcpy(const char* str, int len) {
	return strcpy(const_cast<char*>(str), len);
}

char* strcpy(char* str, int len) {
	return strcpy(str, 0, len);
}

bool endsWith(char* source, char* check) {
	if (strlen(source) < strlen(check)) {
		return false;
	}
	char* test = source + (strlen(source) - strlen(check));
	if (strcmp(test, check) == 0) {
		return true;
	} else {
		return false;
	}
}

bool startsWith(const char* source, const char* check) {
	if (strlen(source) < strlen(check)) {
		return false;
	}
	int r = strncmp(source, check, strlen(check));
	return (r == 0);
}

std::vector<std::string*>* tokenizer(const std::string source, const char* tokens) {
	std::vector<std::string*>* result = new std::vector<std::string*>();
	char* csource = strcpy(source);
	char* token = strtok(csource, tokens);
	while (token != NULL) {
		std::string* item = new std::string(token);
		result->push_back(item);
		token = strtok(NULL, tokens);
	}
	free(csource);

	return result;
}

std::string format(const char * fmt, ...) {
	char* buffer = (char*)malloc(1000);
	memset(buffer, 0, 1000);
	va_list args;
	va_start (args, fmt);
	vsprintf (buffer,fmt, args);
	va_end(args);
	std::string result;
	std::stringstream ss;
	ss << buffer;
	result = ss.str();
	free(buffer);
	return result;
}

std::string toString(double a) {
	std::stringstream ss;

	ss << a;
	string res = ss.str();
	return res;
}

std::string toString(double a, int fixedPrecision) {
	std::stringstream ss;

	ss.precision(fixedPrecision);
	ss.setf(ios::fixed,ios::floatfield);

	ss << a;
	string res = ss.str();
	return res;
}

std::string toString(int a) {
	std::stringstream ss;
	ss << a;
	string s = ss.str();
	return s;
}

bool endsWith(const char* text, const char* end) {
	return endsWith(const_cast<char*>(text), const_cast<char*>(end));
}
/* 
	bool endsWith(const char* text, const char* end) {
	if (strlen(end) > strlen(text)) {
	return false;
	}
	for (unsigned int pos = 0; strlen(end) > pos; pos++) {
	if (end[strlen(end) - pos] != text[strlen(text) - pos]) {
	return false;
	}
	}
	return true;
	}
	*/
std::vector<string> split(const string str, string token) {
	//    char* s = (char*) malloc(str.size());
	const char* delim = token.c_str();

	//strcpy(s, str.c_str());
	char* s = strcpy((char*)str.c_str());

	vector<string> res;
	char* ptr;
	ptr = strtok(s, delim);
	while (ptr != NULL) {
		res.push_back(string(ptr));
		ptr = strtok(NULL, delim);
	}

	free (s);
	return res;
}


long countChar(const char* s, const char c) {
	long num = 0;
	for (unsigned int x = 0; x < strlen(s); x++) {
		if (s[x] == c) {
			num++;
		}
	}
	return num;
}

std::vector<std::string> splitLines(std::string str) {
	std::vector<std::string> lines;
	std::stringstream ss;
	bool line = false;
	int chars = 0;
	for (int i = 0; i < str.length(); i++) {
		char c = str[i];
		if (!((c == 10) || (c == 13))) {
			ss << (char)c;
			line = true;
			chars++;
		} else {
			if (line) {
				if (c > 0) {
					lines.push_back(ss.str());
					ss.str("");
					line = false;
					chars = 0;
				}
			}
		}
	}
	if (chars > 0) {
		lines.push_back(ss.str());
	}

	return lines;
}


std::string concatStrings(const std::string& a, const std::string& b) {
	std::stringstream ss;
	ss << a << b;

	std::string result = ss.str();

	return result;
}

bool compareInsensitive(const char* text1, const char* text2) {
	return boost::iequals(std::string(text1), std::string(text2));
}

char* trim(char* str, int len) {
	int pos = 0;
	// removes the first spaces
	while (str[pos] == ' ') {
		pos++;
	}

	char* result = strcpy(str + pos, len - pos);
	// Removes the spaces at the end
	pos = strlen(result) - 1;
	while (result[pos] == ' ') {
		result[pos] = 0;
		pos--;
	}
	return result;
}
