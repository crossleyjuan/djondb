// =====================================================================================
// 
//  @file:  djonv8.cpp
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

#include "djonv8.h"
#include <v8.h>
#include <assert.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "djondb_client.h"
#include <vector>
#include <sstream> 
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

DjondbConnection* __djonGlobalCon;

// Extracts a C string from a V8 Utf8Value.
std::string ToCString(const v8::String::Utf8Value& value) {
	return *value ? std::string(*value) : "<string conversion failed>";
}

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
		return v8::ThrowException(v8::String::New("usage: insert(db, namespace, json)"));
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

	try {
		if (__djonGlobalCon == NULL) {
			return v8::ThrowException(v8::String::New("You're not connected to any db, please use: connect(server, [port])"));
		}
		__djonGlobalCon->insert(db, ns, json);

		return v8::String::New("");
	} catch (DjondbException e) {
		return v8::ThrowException(v8::String::New(e.what()));
	}
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

	try {
		if (__djonGlobalCon == NULL) {
			return v8::ThrowException(v8::String::New("You're not connected to any db, please use: connect(server, [port])"));
		}
		__djonGlobalCon->update(db, ns, json);

		return v8::String::New("");
	} catch (DjondbException e) {
		return v8::ThrowException(v8::String::New(e.what()));
	}
}

v8::Handle<v8::Value> remove(const v8::Arguments& args) {
	if (args.Length() < 4) {
		return v8::ThrowException(v8::String::New("usage: remove(db, namespace, id, revision)"));
	}

	v8::HandleScope handle_scope;
	v8::String::Utf8Value str(args[0]);
	std::string db = ToCString(str);
	v8::String::Utf8Value str2(args[1]);
	std::string ns = ToCString(str2);
	v8::String::Utf8Value str3(args[2]);
	std::string id = ToCString(str3);
	v8::String::Utf8Value str4(args[3]);
	std::string revision = ToCString(str4);

	try {
		if (__djonGlobalCon == NULL) {
			return v8::ThrowException(v8::String::New("You're not connected to any db, please use: connect(server, [port])"));
		}
		__djonGlobalCon->remove(db, ns, id, revision);

		return v8::String::New("");
	} catch (DjondbException e) {
		return v8::ThrowException(v8::String::New(e.what()));
	}
}

v8::Handle<v8::Value> shutdown(const v8::Arguments& args) {
	if (args.Length() != 0) {
		return v8::ThrowException(v8::String::New("usage: shutdown()"));
	}

	v8::HandleScope handle_scope;

	try {
		if (__djonGlobalCon == NULL) {
			return v8::ThrowException(v8::String::New("You're not connected to any db, please use: connect(server, [port])"));
		}
		__djonGlobalCon->shutdown();

		return v8::String::New("");
	} catch (DjondbException e) {
		return v8::ThrowException(v8::String::New(e.what()));
	}
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

	try {
		if (__djonGlobalCon == NULL) {
			return v8::ThrowException(v8::String::New("You're not connected to any db, please use: connect(server, [port])"));
		}
		bool result = __djonGlobalCon->dropNamespace(db, ns);

		if (result) {
			printf("ns dropped: %s", ns.c_str());
		} else {
			printf("ns cannot be dropped: %s", ns.c_str());
		}
		return v8::String::New("");
	} catch (DjondbException e) {
		return v8::ThrowException(v8::String::New(e.what()));
	}
}

v8::Handle<v8::Value> beginTransaction(const v8::Arguments& args) {
	if (args.Length() > 0) {
		return v8::ThrowException(v8::String::New("usage: beginTransaction()"));
	}

	v8::HandleScope handle_scope;

	if (__djonGlobalCon == NULL) {
		return v8::ThrowException(v8::String::New("You're not connected to any db, please use: connect(server, [port])"));
	}
	bool result = __djonGlobalCon->beginTransaction();

	if (result) {
		printf("Transaction started\n");
	} else {
		printf("Error: Transaction not started");
	}
	return v8::String::New("");
}

v8::Handle<v8::Value> commitTransaction(const v8::Arguments& args) {
	if (args.Length() > 0) {
		return v8::ThrowException(v8::String::New("usage: commitTransaction()"));
	}

	v8::HandleScope handle_scope;

	if (__djonGlobalCon == NULL) {
		return v8::ThrowException(v8::String::New("You're not connected to any db, please use: connect(server, [port])"));
	}
	__djonGlobalCon->commitTransaction();

	printf("Transaction committed\n");
	return v8::String::New("");
}

v8::Handle<v8::Value> rollbackTransaction(const v8::Arguments& args) {
	if (args.Length() > 0) {
		return v8::ThrowException(v8::String::New("usage: rollbackTransaction()"));
	}

	v8::HandleScope handle_scope;

	if (__djonGlobalCon == NULL) {
		return v8::ThrowException(v8::String::New("You're not connected to any db, please use: connect(server, [port])"));
	}
	__djonGlobalCon->rollbackTransaction();

	printf("Transaction rollbacked\n");
	return v8::String::New("");
}

v8::Handle<v8::Value> showDbs(const v8::Arguments& args) {
	if (args.Length() != 0) {
		return v8::ThrowException(v8::String::New("usage: showDbs"));
	}

	v8::HandleScope handle_scope;

	try {
		if (__djonGlobalCon == NULL) {
			return v8::ThrowException(v8::String::New("You're not connected to any db, please use: connect(server, [port])"));
		}
		std::vector<std::string>* dbs = __djonGlobalCon->dbs();

		v8::Handle<v8::Array> result = v8::Array::New();
		int index = 0;
		for (std::vector<std::string>::iterator i = dbs->begin(); i != dbs->end(); i++) {
			std::string n = *i;
			result->Set(v8::Number::New(index), v8::String::New(n.c_str()));
			index++;
		}
		delete dbs;
		return result;
	} catch (DjondbException e) {
		return v8::ThrowException(v8::String::New(e.what()));
	}
}

v8::Handle<v8::Value> showNamespaces(const v8::Arguments& args) {
	if (args.Length() < 1) {
		return v8::ThrowException(v8::String::New("usage: showNamespaces(db)"));
	}

	v8::HandleScope handle_scope;
	v8::String::Utf8Value strDB(args[0]);
	std::string db = ToCString(strDB);

	try {
		if (__djonGlobalCon == NULL) {
			return v8::ThrowException(v8::String::New("You're not connected to any db, please use: connect(server, [port])"));
		}
		std::vector<std::string>* ns = __djonGlobalCon->namespaces(db);

		v8::Handle<v8::Array> result = v8::Array::New();
		int index = 0;
		for (std::vector<std::string>::iterator i = ns->begin(); i != ns->end(); i++) {
			std::string n = *i;
			result->Set(v8::Number::New(index), v8::String::New(n.c_str()));
			index++;
		}
		delete ns;
		return result;
	} catch (DjondbException e) {
		return v8::ThrowException(v8::String::New(e.what()));
	}
}

v8::Handle<v8::Value> find(const v8::Arguments& args) {
	if (args.Length() < 2) {
		return v8::ThrowException(v8::String::New("usage: find(db, namespace)\nfind(db, namespace, filter)\nfind(db, namespace, select, filter)"));
	}

	if (__djonGlobalCon == NULL) {
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
		BSONArrayObj* result = __djonGlobalCon->find(db, ns, select, filter);

		char* str = result->toChar();

		v8::Handle<v8::Value> jsonValue = parseJSON(v8::String::New(str));
		free(str);
		delete result;
		return jsonValue;
	} catch (DjondbException e) {
		return v8::ThrowException(v8::String::New(e.what()));
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
		return v8::ThrowException(v8::String::New("usage: uuid()"));
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
	try {
		if (__djonGlobalCon != NULL) {
			__djonGlobalCon->close();
		}
		int port = SERVER_PORT;
		if (args.Length() == 2) {
			port = args[1]->Int32Value();	
		}
		__djonGlobalCon = DjondbConnectionManager::getConnection(server, port);
		if (__djonGlobalCon->open()) {
			printf("Connected to %s\n", server.c_str());
		} else {
			printf("Could not connect to: %s\n", server.c_str());
			__djonGlobalCon->close();
			__djonGlobalCon = NULL;
		}
		return v8::String::New("");
	} catch (DjondbException e) {
		return v8::ThrowException(v8::String::New(e.what()));
	}
}

v8::Handle<v8::Value> executeUpdate(const v8::Arguments& args) {
	if (args.Length() != 1) {
		return v8::ThrowException(v8::String::New("usage: executeUpdate(query)"));
	}

	v8::HandleScope handle_scope;
	v8::String::Utf8Value str(args[0]);
	std::string query = ToCString(str);

	try {
		if (__djonGlobalCon == NULL) {
			return v8::ThrowException(v8::String::New("You're not connected to any db, please use: connect(server, [port])"));
		}
		__djonGlobalCon->executeUpdate(query);

		return v8::Undefined();
	} catch (ParseException e) {
		return v8::ThrowException(v8::String::New(e.what()));
	} catch (DjondbException e) {
		return v8::ThrowException(v8::String::New(e.what()));
	}
}

v8::Handle<v8::Value> executeQuery(const v8::Arguments& args) {
	if (args.Length() != 1) {
		return v8::ThrowException(v8::String::New("usage: executeQuery(dql)"));
	}

	if (__djonGlobalCon == NULL) {
		return v8::ThrowException(v8::String::New("You're not connected to any db, please use: connect(server, [port])"));
	}
	v8::HandleScope handle_scope;
	v8::String::Utf8Value strQuery(args[0]);
	std::string query = ToCString(strQuery);

	try {
		BSONArrayObj* result = __djonGlobalCon->executeQuery(query);

		if (result != NULL) {
			char* str = result->toChar();

			v8::Handle<v8::Value> jsonValue = parseJSON(v8::String::New(str));
			free(str);
			delete result;
			return handle_scope.Close(jsonValue);
		} else {
			return v8::Undefined();
		}
	} catch (ParseException e) {
		return v8::ThrowException(v8::String::New(e.what()));
	} catch (DjondbException e) {
		return v8::ThrowException(v8::String::New(e.what()));
	}
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
