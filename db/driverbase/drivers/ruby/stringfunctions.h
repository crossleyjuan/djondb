/*
 * File:   stringfunctions.h
 * Author: cross
 *
 * Created on July 8, 2010, 2:54 PM
 */

#ifndef _STRINGFUNCTIONS_H
#define	_STRINGFUNCTIONS_H

#include <string>
#include <vector>

char* strcpy(char* str, int len);
char* strcpy(char* str, int offset, int len);
char* strcpy(std::string str);
bool endsWith(char* source, char* check);
bool startsWith(const char* source, const char* check);
std::vector<std::string*>* tokenizer(const std::string source, const char* tokens);
std::string format(const char * fmt, ...);

std::string toString(double a);
std::string toString(double a, int fixedPrecision);
std::string toString(int a);
std::vector<std::string> split(std::string str, std::string token);
long countChar(const char* s, const char c);
std::string concatStrings(const std::string& a, const std::string& b);

std::vector<std::string> splitLines(std::string);
bool compareInsensitive(const char* text1, const char* text2);

char* trim(char* str, int len);

#endif	/* _STRINGFUNCTIONS_H */

