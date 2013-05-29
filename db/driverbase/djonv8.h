// =====================================================================================
// 
//  @file:  djonv8.h
// 
//  @brief:  
// 
//  @version:  1.0
//  @date:     05/14/2013 10:02:37 PM
//  Compiler:  g++
// 
//  @author:  Juan Pablo Crossley (Cross), crossleyjuan@gmail.com
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
#ifndef DJONV8_INCLUDED_H
#define DJONV8_INCLUDED_H 
#include <v8.h>
#include <string>

v8::Handle<v8::Value> Print(const v8::Arguments& args);
v8::Handle<v8::Value> find(const v8::Arguments& args);
v8::Handle<v8::Value> executeUpdate(const v8::Arguments& args);
v8::Handle<v8::Value> executeQuery(const v8::Arguments& args);
v8::Handle<v8::Value> dropNamespace(const v8::Arguments& args);
v8::Handle<v8::Value> showDbs(const v8::Arguments& args);
v8::Handle<v8::Value> showNamespaces(const v8::Arguments& args);
v8::Handle<v8::Value> Read(const v8::Arguments& args);
v8::Handle<v8::Value> Load(const v8::Arguments& args);
v8::Handle<v8::Value> insert(const v8::Arguments& args);
v8::Handle<v8::Value> update(const v8::Arguments& args);
v8::Handle<v8::Value> remove(const v8::Arguments& args);
v8::Handle<v8::Value> shutdown(const v8::Arguments& args);
v8::Handle<v8::Value> fuuid(const v8::Arguments& args);
v8::Handle<v8::Value> connect(const v8::Arguments& args);
v8::Handle<v8::String> ReadFile(const char* name);
v8::Handle<v8::Value> beginTransaction(const v8::Arguments& args);
v8::Handle<v8::Value> commitTransaction(const v8::Arguments& args);
v8::Handle<v8::Value> rollbackTransaction(const v8::Arguments& args);
void ReportException(v8::TryCatch* handler);

v8::Handle<v8::Value> parseJSON(v8::Handle<v8::Value> object);
bool ExecuteString(v8::Handle<v8::String> source, v8::Handle<v8::Value> name,	bool print_result, bool report_exceptions);
char* getFile(const char* name);
std::string ToCString(const v8::String::Utf8Value& value);
#endif /* DJONV8_INCLUDED_H */
