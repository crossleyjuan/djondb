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

// Copyright 2011 the V8 project authors. All rights reserved.
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
//       copyright notice, this list of conditions and the following
//       disclaimer in the documentation and/or other materials provided
//       with the distribution.
//     * Neither the name of Google Inc. nor the names of its
//       contributors may be used to endorse or promote products derived
//       from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR 
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include <v8.h>
#include <assert.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "djondb_client.h"
#include <vector>
#include <sstream>
#ifndef WINDOWS
#include "linenoise.h"
#endif
#include "util.h"

#ifdef COMPRESS_STARTUP_DATA_BZ2
#error Using compressed startup data is not supported for this sample
#endif

/**
 * This sample program shows how to implement a simple javascript shell
 * based on V8.  This includes initializing V8 with command line options,
 * creating global functions, compiling and executing strings.
 *
 * For a more sophisticated shell, consider using the debug shell D8.
 */

using namespace djondb;

Connection* __djonConnection;

CircularQueue<std::string> _commands(10);

v8::Persistent<v8::Context> CreateShellContext();
void RunShell(v8::Handle<v8::Context> context);
int RunMain(int argc, char* argv[]);
bool ExecuteString(v8::Handle<v8::String> source,
		v8::Handle<v8::Value> name,
		bool print_result,
		bool report_exceptions);
v8::Handle<v8::Value> Print(const v8::Arguments& args);
v8::Handle<v8::Value> find(const v8::Arguments& args);
v8::Handle<v8::Value> dropNamespace(const v8::Arguments& args);
v8::Handle<v8::Value> showDbs(const v8::Arguments& args);
v8::Handle<v8::Value> showNamespaces(const v8::Arguments& args);
v8::Handle<v8::Value> Read(const v8::Arguments& args);
v8::Handle<v8::Value> Load(const v8::Arguments& args);
v8::Handle<v8::Value> Quit(const v8::Arguments& args);
v8::Handle<v8::Value> Version(const v8::Arguments& args);
v8::Handle<v8::Value> insert(const v8::Arguments& args);
v8::Handle<v8::Value> update(const v8::Arguments& args);
v8::Handle<v8::Value> shutdown(const v8::Arguments& args);
v8::Handle<v8::Value> fuuid(const v8::Arguments& args);
v8::Handle<v8::Value> connect(const v8::Arguments& args);
v8::Handle<v8::Value> help(const v8::Arguments& args);
v8::Handle<v8::String> ReadFile(const char* name);
void ReportException(v8::TryCatch* handler);

static bool run_shell;

char* commands[] = {
	"print",
	"find",
	"dropNamespace",
	"showDbs",
	"showNamespaces",
	"Read",
	"Load",
	"quit",
	"version",
	"insert",
	"update",
	"shutdown",
	"fuuid",
	"connect",
	"help",
	"readfile"
};

int main(int argc, char* argv[]) {
	v8::V8::SetFlagsFromCommandLine(&argc, argv, true);
	run_shell = (argc == 1);
	v8::HandleScope handle_scope;
	v8::Persistent<v8::Context> context = CreateShellContext();
	if (context.IsEmpty()) {
		printf("Error creating context\n");
		return 1;
	}
	context->Enter();
	int result = RunMain(argc, argv);
	if (run_shell) RunShell(context);
	context->Exit();
	context.Dispose();
	v8::V8::Dispose();
	return result;
}

#ifndef WINDOWS
void completion(const char *buf, linenoiseCompletions *lc) {
	for (int x = 0; x < 13; x++) {
		if (startsWith(commands[x], buf)) {
			linenoiseAddCompletion(lc, commands[x]);
		}
	}
}
#endif

// Extracts a C string from a V8 Utf8Value.
std::string ToCString(const v8::String::Utf8Value& value) {
	return *value ? std::string(*value) : "<string conversion failed>";
}


// Creates a new execution environment containing the built-in
// functions.
v8::Persistent<v8::Context> CreateShellContext() {
	// Create a template for the global object.
	v8::Handle<v8::ObjectTemplate> global = v8::ObjectTemplate::New();
	// Bind the global 'print' function to the C++ Print callback.
	global->Set(v8::String::New("print"), v8::FunctionTemplate::New(Print));
	// Bind the gloabl 'find' function to the C++ Find callback.
	global->Set(v8::String::New("find"), v8::FunctionTemplate::New(find));
	// Bind the gloabl 'dropNamespace' function to the C++ Find callback.
	global->Set(v8::String::New("dropNamespace"), v8::FunctionTemplate::New(dropNamespace));
	// Bind the gloabl 'showDbs' function to the C++ Find callback.
	global->Set(v8::String::New("showDbs"), v8::FunctionTemplate::New(showDbs));
	// Bind the gloabl 'showNamespaces' function to the C++ Find callback.
	global->Set(v8::String::New("showNamespaces"), v8::FunctionTemplate::New(showNamespaces));
	// Bind the global 'read' function to the C++ Read callback.
	global->Set(v8::String::New("read"), v8::FunctionTemplate::New(Read));
	// Bind the global 'load' function to the C++ Load callback.
	global->Set(v8::String::New("load"), v8::FunctionTemplate::New(Load));
	// Bind the 'quit' function
	global->Set(v8::String::New("quit"), v8::FunctionTemplate::New(Quit));
	// Bind the 'version' function
	global->Set(v8::String::New("version"), v8::FunctionTemplate::New(Version));
	// Bind the 'db.insert' function
	global->Set(v8::String::New("insert"), v8::FunctionTemplate::New(insert));
	// Bind the 'db.update' function
	global->Set(v8::String::New("update"), v8::FunctionTemplate::New(update));
	// Bind the 'db.uuid' function
	global->Set(v8::String::New("uuid"), v8::FunctionTemplate::New(fuuid));
	// Bind the 'connect' function
	global->Set(v8::String::New("connect"), v8::FunctionTemplate::New(connect));
	// Bind the 'db.help' function
	global->Set(v8::String::New("help"), v8::FunctionTemplate::New(help));
	// Bind the global 'shutdown' function to the C++ Load callback.
	global->Set(v8::String::New("shutdown"), v8::FunctionTemplate::New(shutdown));

	return v8::Context::New(NULL, global);
}

/* 
	std::string toJson(const v8::Local<v8::Object>& obj) {
	std::stringstream ss;
	ss << "{";
	v8::Local<v8::Array> propertyNames = obj->GetPropertyNames();

	for (int x = 0; x < propertyNames->Length(); x++) {
	if (x != 0) {
	ss << ", ";
	}
	v8::String::Utf8Value name(propertyNames->Get(x));
	ss << "\"" << ToCString(name) << "\":";
	v8::Local<v8::Value> val = obj->GetInternalField(x);
	if (val->IsObject()) {
	ss << toJson(val->ToObject());
	} else {
	ss << "\"" << ToCString(v8::String::Utf8Value(val)) << "\"";
	}
	}

	ss << "}";

	std::string sresult = ss.str();
	return sresult;
	}

*/

v8::Handle<v8::Value> parseJSON(v8::Handle<v8::Value> object)
{
	v8::HandleScope scope;

	v8::Handle<v8::Context> context = v8::Context::GetCurrent();
	v8::Handle<v8::Object> global = context->Global();

	v8::Handle<v8::Object> JSON = global->Get(v8::String::New("JSON"))->ToObject();
	v8::Handle<v8::Function> JSON_parse = v8::Handle<v8::Function>::Cast(JSON->Get(v8::String::New("parse")));

	return scope.Close(JSON_parse->Call(JSON, 1, &object));
}

v8::Handle<v8::Value> toJson(v8::Handle<v8::Value> object, bool beautify = false)
{
	v8::HandleScope scope;

	v8::Handle<v8::Context> context = v8::Context::GetCurrent();
	v8::Handle<v8::Object> global = context->Global();

	v8::Handle<v8::Object> JSON = global->Get(v8::String::New("JSON"))->ToObject();
	v8::Handle<v8::Function> JSON_stringify = v8::Handle<v8::Function>::Cast(JSON->Get(v8::String::New("stringify")));

	if (!beautify) {
		return scope.Close(JSON_stringify->Call(JSON, 1, &object));
	} else {
		v8::Handle<v8::Value> args[3];
		args[0] = object;
		args[1] = v8::Null();
		args[2] = v8::Integer::New(4);
		return scope.Close(JSON_stringify->Call(JSON, 3, args));
	}
}

v8::Handle<v8::Value> insert(const v8::Arguments& args) {
	if (args.Length() < 3) {
		return v8::ThrowException(v8::String::New("usage: db.insert(db, namespace, json)"));
	}

	v8::HandleScope handle_scope;
	v8::String::Utf8Value str(args[0]);
	std::string db = ToCString(str);
	v8::String::Utf8Value str2(args[1]);
	std::string ns = ToCString(str2);
	std::string json;
	if (args[2]->IsObject()) {
		v8::String::Utf8Value strValue(toJson(args[2]));
		json = ToCString(strValue);
	} else {
		v8::String::Utf8Value sjson(args[2]);
		json = ToCString(sjson);
	}

	if (__djonConnection == NULL) {
		return v8::ThrowException(v8::String::New("You're not connected to any db, please use: connect(server, [port])"));
	}
	__djonConnection->insert(db, ns, json);

	return v8::String::New("");
}

v8::Handle<v8::Value> update(const v8::Arguments& args) {
	if (args.Length() < 3) {
		return v8::ThrowException(v8::String::New("usage: update(db, namespace, json)"));
	}

	v8::HandleScope handle_scope;
	v8::String::Utf8Value str(args[0]);
	std::string db = ToCString(str);
	v8::String::Utf8Value str2(args[1]);
	std::string ns = ToCString(str2);
	std::string json;
	if (args[2]->IsObject()) {
		v8::String::Utf8Value strValue(toJson(args[2]));
		json = ToCString(strValue);
	} else {
		v8::String::Utf8Value sjson(args[2]);
		json = ToCString(sjson);
	}

	if (__djonConnection == NULL) {
		return v8::ThrowException(v8::String::New("You're not connected to any db, please use: connect(server, [port])"));
	}
	__djonConnection->update(db, ns, json);

	return v8::String::New("");
}

v8::Handle<v8::Value> shutdown(const v8::Arguments& args) {
	if (args.Length() != 0) {
		return v8::ThrowException(v8::String::New("usage: db.shutdown()"));
	}

	v8::HandleScope handle_scope;

	if (__djonConnection == NULL) {
		return v8::ThrowException(v8::String::New("You're not connected to any db, please use: connect(server, [port])"));
	}
	__djonConnection->shutdown();

	return v8::String::New("");
}

v8::Handle<v8::Value> dropNamespace(const v8::Arguments& args) {
	if (args.Length() < 2) {
		return v8::ThrowException(v8::String::New("usage: dropNamespace(db, namespace)"));
	}

	v8::HandleScope handle_scope;
	v8::String::Utf8Value strDB(args[0]);
	std::string db = ToCString(strDB);
	v8::String::Utf8Value str(args[1]);
	std::string ns = ToCString(str);

	if (__djonConnection == NULL) {
		return v8::ThrowException(v8::String::New("You're not connected to any db, please use: connect(server, [port])"));
	}
	bool result = __djonConnection->dropNamespace(db, ns);

	if (result) {
		printf("ns dropped: %s", ns.c_str());
	} else {
		printf("ns cannot be dropped: %s", ns.c_str());
	}
	return v8::String::New("");
}

v8::Handle<v8::Value> showDbs(const v8::Arguments& args) {
	if (args.Length() != 0) {
		return v8::ThrowException(v8::String::New("usage: showDbs"));
	}

	v8::HandleScope handle_scope;

	if (__djonConnection == NULL) {
		return v8::ThrowException(v8::String::New("You're not connected to any db, please use: connect(server, [port])"));
	}
	std::vector<std::string>* dbs = __djonConnection->dbs();

	v8::Handle<v8::Array> result = v8::Array::New();
	int index = 0;
	for (std::vector<std::string>::iterator i = dbs->begin(); i != dbs->end(); i++) {
		std::string n = *i;
		result->Set(v8::Number::New(index), v8::String::New(n.c_str()));
		index++;
	}
	delete dbs;
	return result;
}

v8::Handle<v8::Value> showNamespaces(const v8::Arguments& args) {
	if (args.Length() < 1) {
		return v8::ThrowException(v8::String::New("usage: showNamespaces(db)"));
	}

	v8::HandleScope handle_scope;
	v8::String::Utf8Value strDB(args[0]);
	std::string db = ToCString(strDB);

	if (__djonConnection == NULL) {
		return v8::ThrowException(v8::String::New("You're not connected to any db, please use: connect(server, [port])"));
	}
	std::vector<std::string>* ns = __djonConnection->namespaces(db);

	v8::Handle<v8::Array> result = v8::Array::New();
	int index = 0;
	for (std::vector<std::string>::iterator i = ns->begin(); i != ns->end(); i++) {
		std::string n = *i;
		result->Set(v8::Number::New(index), v8::String::New(n.c_str()));
		index++;
	}
	delete ns;
	return result;
}

v8::Handle<v8::Value> help(const v8::Arguments& args) {
	if (args.Length() > 0) {
		v8::String::Utf8Value str(args[0]);
		std::string cmd = ToCString(str);
	} else {
		printf("connect('hostname', [port])\n\tEstablish a connection with a server.\n");
			printf("dropNamespace('db', 'namespace');\n\tDrops a namespace from the db.\n");
			printf("find('db', 'namespace'[, 'select'][, 'filter']);\n\tExecutes a find using the provided filter.\n");
			printf("help();\n\tThis help\n");
			printf("insert('db', 'namespace', { json...object});\n\tInserts a new document.\n");
			printf("update('db', 'namespace', { json...object});\n\tUpdates a document.\n");
			printf("load('file');\n\tLoads and executes a script.\n");
			printf("print(data);\n\tPrint console messages.\n");
			printf("quit();\n\tBye bye fellow.\n");
			printf("read('file');\n\tReads the contents of a file.\n");
			printf("showDbs();\n\tReturns a list of the available databases.\n");
			printf("showNamespaces('db');\n\tReturns the namespaces in that database.\n");
			printf("shutdown();\n\tRemotely shutdowns the server (Are you sure?).\n");
		printf("uuid();\n\tGenerates a new UUID (Universal Unique Identifier).\n");
			printf("version();\n\tReturns the current shell version.\n");
	}
	return v8::Undefined();
}

v8::Handle<v8::Value> find(const v8::Arguments& args) {
	if (args.Length() < 2) {
		return v8::ThrowException(v8::String::New("usage: db.find(db, namespace)\ndb.find(db, namespace, filter)\ndb.find(db, namespace, select, filter)"));
	}

	if (__djonConnection == NULL) {
		return v8::ThrowException(v8::String::New("You're not connected to any db, please use: connect(server, [port])"));
	}
	v8::HandleScope handle_scope;
	v8::String::Utf8Value strDB(args[0]);
	std::string db = ToCString(strDB);
	v8::String::Utf8Value str(args[1]);
	std::string ns = ToCString(str);
	std::string select = "*";
	std::string filter = "";
	if (args.Length() == 3) {
		v8::String::Utf8Value strFilter(args[2]);
		filter = ToCString(strFilter);
	}
	if (args.Length() == 4) {
		v8::String::Utf8Value strSelect(args[2]);
		select = ToCString(strSelect);
		v8::String::Utf8Value strFilter(args[3]);
		filter = ToCString(strFilter);
	}
	/* 
		std::string json;
		if (args[2]->IsObject()) {
		json = toJson(args[2]->ToObject());
		} else {
		json = ToCString(v8::String::Utf8Value(args[2]));
		}
		*/

	try {
		std::vector<BSONObj*>* result = __djonConnection->find(db, ns, select, filter);

		std::stringstream ss;
		ss << "[";
		if (result->size() > 0) {
			for (std::vector<BSONObj*>::const_iterator i = result->begin(); i != result->end(); i++) {
				BSONObj* obj = *i;
				if (i != result->begin()) {
					ss << ", ";
				}
				ss << obj->toChar();
			}
		}
		ss << "]";

		std::string sresult = ss.str();

		delete result;
		return parseJSON(v8::String::New(sresult.c_str()));
	} catch (ParseException e) {
		return v8::ThrowException(v8::String::New("the filter expression contains an error\n"));
	}
	/* 
		v8::Handle<v8::Context> context = v8::Context::GetCurrent();
		v8::Handle<v8::Object> global = context->Global();

		v8::Handle<v8::Object> objresult = global->Get(v8::String::New(sresult.c_str()))->ToObject();


		return objresult;
	//return v8::String::New(sresult.c_str());
	*/
}

v8::Handle<v8::Value> fuuid(const v8::Arguments& args) {
	if (args.Length() > 0) {
		return v8::ThrowException(v8::String::New("usage: db.uuid()"));
	}

	std::string* suid = uuid();
	v8::String::Utf8Value str();

	const char* tmp = suid->c_str(); 
	v8::Handle<v8::String> result(v8::String::New(tmp));
	delete suid; 

	return result;
}

v8::Handle<v8::Value> connect(const v8::Arguments& args) {
	if (args.Length() < 1) {
		return v8::ThrowException(v8::String::New("usage: connect(server, [port])\n"));
	}

	v8::HandleScope handle_scope;
	v8::String::Utf8Value str(args[0]);
	std::string server = ToCString(str);
	if (__djonConnection != NULL) {
		__djonConnection->close();
	}
	int port = SERVER_PORT;
	if (args.Length() == 2) {
		port = args[1]->Int32Value();	
	}
	__djonConnection = ConnectionManager::getConnection(server, port);
	if (__djonConnection->open()) {
		printf("Connected to %s\n", server.c_str());
	} else {
		printf("Could not connect to: %s\n", server.c_str());
		__djonConnection->close();
		__djonConnection = NULL;
	}
	return v8::String::New("");
}

// The callback that is invoked by v8 whenever the JavaScript 'print'
// function is called.  Prints its arguments on stdout separated by
// spaces and ending with a newline.
void internalPrint(const v8::Handle<v8::Value> arg) {
	v8::HandleScope handle_scope;
	std::string cstr;
	if (arg->IsObject()) {
		v8::String::Utf8Value str(toJson(arg->ToObject(), true));
		cstr = ToCString(str);
	} else {
		v8::String::Utf8Value str(arg);
		cstr = ToCString(str);
	}
	printf("%s", cstr.c_str());
}

// The callback that is invoked by v8 whenever the JavaScript 'print'
// function is called.  Prints its arguments on stdout separated by
// spaces and ending with a newline.
v8::Handle<v8::Value> Print(const v8::Arguments& args) {
	bool first = true;
	for (int i = 0; i < args.Length(); i++) {
		v8::HandleScope handle_scope;
		if (first) {
			first = false;
		} else {
			printf(" ");
		}
		internalPrint(args[i]);
	}
	printf("\n");
	fflush(stdout);
	return v8::Undefined();
}


// The callback that is invoked by v8 whenever the JavaScript 'read'
// function is called.  This function loads the content of the file named in
// the argument into a JavaScript string.
v8::Handle<v8::Value> Read(const v8::Arguments& args) {
	if (args.Length() != 1) {
		return v8::ThrowException(v8::String::New("Bad parameters"));
	}
	v8::String::Utf8Value file(args[0]);
	if (*file == NULL) {
		return v8::ThrowException(v8::String::New("Error loading file"));
	}
	v8::Handle<v8::String> source = ReadFile(*file);
	if (source.IsEmpty()) {
		return v8::ThrowException(v8::String::New("Error loading file"));
	}
	return source;
}


// The callback that is invoked by v8 whenever the JavaScript 'load'
// function is called.  Loads, compiles and executes its argument
// JavaScript file.
v8::Handle<v8::Value> Load(const v8::Arguments& args) {
	for (int i = 0; i < args.Length(); i++) {
		v8::HandleScope handle_scope;
		v8::String::Utf8Value file(args[i]);
		if (*file == NULL) {
			return v8::ThrowException(v8::String::New("Error loading file"));
		}
		v8::Handle<v8::String> source = ReadFile(*file);
		if (source.IsEmpty()) {
			return v8::ThrowException(v8::String::New("Error loading file"));
		}
		if (!ExecuteString(source, v8::String::New(*file), false, false)) {
			return v8::ThrowException(v8::String::New("Error executing file"));
		}
	}
	return v8::Undefined();
}


// The callback that is invoked by v8 whenever the JavaScript 'quit'
// function is called.  Quits.
v8::Handle<v8::Value> Quit(const v8::Arguments& args) {
	// If not arguments are given args[0] will yield undefined which
	// converts to the integer value 0.
	int exit_code = args[0]->Int32Value();
	fflush(stdout);
	fflush(stderr);

#ifndef WINDOWS
	linenoiseHistorySave(".djonshell_history");
#endif
	exit(exit_code);
	return v8::Undefined();
}


v8::Handle<v8::Value> Version(const v8::Arguments& args) {
	std::stringstream ss;
	ss << "djondb shell version: " << VERSION;
	std::string smsg = ss.str();
	const char* message = smsg.c_str();

	return v8::String::New(message);
}


// Reads a file into a v8 string.
v8::Handle<v8::String> ReadFile(const char* name) {
	FILE* file = fopen(name, "rb");
	if (file == NULL) return v8::Handle<v8::String>();

	fseek(file, 0, SEEK_END);
	int size = ftell(file);
	rewind(file);

	char* chars = new char[size + 1];
	chars[size] = '\0';
	for (int i = 0; i < size;) {
		int read = fread(&chars[i], 1, size - i, file);
		i += read;
	}
	fclose(file);
	v8::Handle<v8::String> result = v8::String::New(chars, size);
	delete[] chars;
	return result;
}

char* getFile(const char* name) {
	FILE* file = fopen(name, "rb");
	if (file == NULL) return new char[0];

	fseek(file, 0, SEEK_END);
	int size = ftell(file);
	rewind(file);

	char* chars = new char[size + 1];
	chars[size] = '\0';
	for (int i = 0; i < size;) {
		int read = fread(&chars[i], 1, size - i, file);
		i += read;
	}
	fclose(file);
	return chars;
}

// Process remaining command line arguments and execute files
int RunMain(int argc, char* argv[]) {
	__djonConnection = NULL;

	for (int i = 1; i < argc; i++) {
		const char* str = argv[i];
		if (strcmp(str, "--shell") == 0) {
			run_shell = true;
		} else if (strcmp(str, "-f") == 0) {
			// Ignore any -f flags for compatibility with the other stand-
			// alone JavaScript engines.
			continue;
		} else if (strncmp(str, "--", 2) == 0) {
			printf("Warning: unknown flag %s.\nTry --help for options\n", str);
		} else if (strcmp(str, "-e") == 0 && i + 1 < argc) {
			// Execute argument given to -e option directly.
			v8::Handle<v8::String> file_name = v8::String::New("unnamed");
			v8::Handle<v8::String> source = v8::String::New(argv[++i]);
			if (!ExecuteString(source, file_name, false, true)) return 1;
		} else {
			// Use all other arguments as names of files to load and run.
			v8::Handle<v8::String> file_name = v8::String::New(str);
			v8::Handle<v8::String> source = ReadFile(str);
			if (source.IsEmpty()) {
				printf("Error reading '%s'\n", str);
				continue;
			}
			if (!ExecuteString(source, file_name, false, true)) return 1;
		}

	}
	__djonConnection = NULL;
	return 0;
}
/*
	int getkey(void) 
	{               
	union REGS in, out;
	in.h.ah = 0x08; 
	int86(0x21, &in, &out);
	if (out.h.al == 0)  
	{
	return(getkey()+0x100);
	}
	else         
	{      
	return(out.h.al);  
	}              
	} 
	*/

std::string readLine(char* prompt) {

#ifndef WINDOWS
	char* line = linenoise(prompt);
	linenoiseHistoryAdd(line);
#else
	static const int kBufferSize = 1024;
	char buffer[kBufferSize];
	printf(prompt);
	char* line = fgets(buffer, kBufferSize, stdin);
#endif
	return std::string(line);
}

// The read-eval-execute loop of the shell.
void RunShell(v8::Handle<v8::Context> context) {
	printf("djondb shell version %s\n", VERSION);
	printf("Welcome to djondb shell.\n");
	printf("Use help(); to get the commands available. \n(hint: The first command should be \"connect\" to playing with a server)\n");

	static const int kBufferSize = 256;
	// Enter the execution environment before evaluating any code.
	v8::Context::Scope context_scope(context);
	v8::Local<v8::String> name(v8::String::New("(shell)"));

#ifndef WINDOWS
	linenoiseHistoryLoad(".djonshell_history");
	linenoiseSetCompletionCallback(completion);
#endif
	std::stringstream ss;
	bool line = false;
	const char* lastCmd = "";
	while (true) {
		char buffer[kBufferSize];
		char* prompt;
		if (!line) {
			prompt = "> ";
		} else {
			prompt = ". ";
		}
		line = false;
		std::string str = readLine(prompt);
		if (str.length() > 0) {
			//char* str = fgets(buffer, kBufferSize, stdin);
			if (str[0] == '~') {
				system("vi .tmp.js");
				lastCmd = getFile(".tmp.js");
				printf("Buffer loaded.\n");
				continue;
			} else if (str[0] == '.') {
				if ((lastCmd == NULL) || (strlen(lastCmd) == 0)) {
					lastCmd = getFile(".tmp.js");
					printf("Buffer loaded.\n");
				}
				str = const_cast<char*>(lastCmd);
			} else 
				if ((str.length() >= 2) && (str[str.length() - 2] == '\\')) {
					str[str.length() - 2] = ' '; 
					line = true;
				}
			ss << str;
			if (!line) {
				v8::HandleScope handle_scope;
				std::string sCmd = ss.str();
				const char* cmd = sCmd.c_str();
				ss.str("");
				lastCmd = cmd;
				_commands.push_back(std::string(cmd));
				ExecuteString(v8::String::New(cmd), name, true, true);
			}
		}
	}
	printf("\n");
}


// Executes a string within the current v8 context.
bool ExecuteString(v8::Handle<v8::String> source,
		v8::Handle<v8::Value> name,
		bool print_result,
		bool report_exceptions) {
	v8::HandleScope handle_scope;
	v8::TryCatch try_catch;
	v8::Handle<v8::Script> script = v8::Script::Compile(source, name);
	if (script.IsEmpty()) {
		// Print errors that happened during compilation.
		if (report_exceptions)
			ReportException(&try_catch);
		return false;
	} else {
		v8::Handle<v8::Value> result = script->Run();
		if (result.IsEmpty()) {
			assert(try_catch.HasCaught());
			// Print errors that happened during execution.
			if (report_exceptions)
				ReportException(&try_catch);
			return false;
		} else {
			assert(!try_catch.HasCaught());
			if (print_result && !result->IsUndefined()) {
				// If all went well and the result wasn't undefined then print
				// the returned value.
				if (result->IsObject()) {
					v8::Handle<v8::Value> val = toJson(result);
					v8::String::Utf8Value sval(val);
					std::string temp = ToCString(sval);
					printf("%s\n", temp.c_str());
				} else {
					v8::String::Utf8Value str(result);
					std::string cstr = ToCString(str);
					printf("%s\n", cstr.c_str());
				}
			}
			return true;
		}
	}
}


void ReportException(v8::TryCatch* try_catch) {
	v8::HandleScope handle_scope;
	v8::String::Utf8Value exception(try_catch->Exception());
	std::string exception_string = ToCString(exception);
	v8::Handle<v8::Message> message = try_catch->Message();
	if (message.IsEmpty()) {
		// V8 didn't provide any extra information about this error; just
		// print the exception.
		printf("%s\n", exception_string.c_str());
	} else {
		// Print (filename):(line number): (message).
		v8::String::Utf8Value filename(message->GetScriptResourceName());
		std::string filename_string = ToCString(filename);
		int linenum = message->GetLineNumber();
		printf("%s:%i: %s\n", filename_string.c_str(), linenum, exception_string.c_str());
		// Print line of source code.
		v8::String::Utf8Value sourceline(message->GetSourceLine());
		std::string sourceline_string = ToCString(sourceline);
		printf("%s\n", sourceline_string.c_str());
		// Print wavy underline (GetUnderline is deprecated).
		int start = message->GetStartColumn();
		for (int i = 0; i < start; i++) {
			printf(" ");
		}
		int end = message->GetEndColumn();
		for (int i = start; i < end; i++) {
			printf("^");
		}
		printf("\n");
		v8::String::Utf8Value stack_trace(try_catch->StackTrace());
		if (stack_trace.length() > 0) {
			std::string stack_trace_string = ToCString(stack_trace);
			printf("%s\n", stack_trace_string.c_str());
		}
	}
}
