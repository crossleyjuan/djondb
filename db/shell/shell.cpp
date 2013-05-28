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
#include "djonv8.h"
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

CircularQueue<std::string> _commands(10);

DjondbConnection* __djonConnection;

v8::Persistent<v8::Context> CreateShellContext();
int RunMain(int argc, char* argv[]);
void RunShell(v8::Handle<v8::Context> context);

v8::Handle<v8::Value> Quit(const v8::Arguments& args);
v8::Handle<v8::Value> Version(const v8::Arguments& args);
v8::Handle<v8::Value> help(const v8::Arguments& args);

static bool run_shell;

char* commands[] = {
	"print",
	"find",
	"executeQuery",
	"executeUpdate",
	"dropNamespace",
	"showDbs",
	"showNamespaces",
	"Read",
	"Load",
	"quit",
	"version",
	"insert",
	"update",
	"remove",
	"shutdown",
	"fuuid",
	"connect",
	"help",
	"beginTransaction",
	"commitTransaction",
	"rollbackTransaction",
	"readfile"
};

int COMMANDS_LENGTH = 19;

char* getHistoryFilename() {
	std::string* homeDir = getHomeDir();
	char* file = (char*)malloc(2048);
	memset(file, 0, 2048);
	sprintf(file, "%s%s.djonshell", homeDir->c_str(), FILESEPARATOR);
	delete homeDir;
	return file;
}

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
	for (int x = 0; x < COMMANDS_LENGTH; x++) {
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
	// Bind the 'db.remove' function
	global->Set(v8::String::New("remove"), v8::FunctionTemplate::New(remove));
	// Bind the 'db.uuid' function
	global->Set(v8::String::New("uuid"), v8::FunctionTemplate::New(fuuid));
	// Bind the 'beginTransaction' function
	global->Set(v8::String::New("beginTransaction"), v8::FunctionTemplate::New(beginTransaction));
	// Bind the 'commitTransaction' function
	global->Set(v8::String::New("commitTransaction"), v8::FunctionTemplate::New(commitTransaction));
	// Bind the 'rollbackTransaction' function
	global->Set(v8::String::New("rollbackTransaction"), v8::FunctionTemplate::New(rollbackTransaction));
	// Bind the 'connect' function
	global->Set(v8::String::New("connect"), v8::FunctionTemplate::New(connect));
	// Bind the 'db.help' function
	global->Set(v8::String::New("help"), v8::FunctionTemplate::New(help));
	// Bind the global 'shutdown' function to the C++ Load callback.
	global->Set(v8::String::New("shutdown"), v8::FunctionTemplate::New(shutdown));
	// Bind the gloabl 'executeQuery' function to the C++ executeQuery callback.
	global->Set(v8::String::New("executeQuery"), v8::FunctionTemplate::New(executeQuery));
	// Bind the gloabl 'executeUpdate' function to the C++ executeUpdate callback.
	global->Set(v8::String::New("executeUpdate"), v8::FunctionTemplate::New(executeUpdate));

	return v8::Context::New(NULL, global);
}

v8::Handle<v8::Value> help(const v8::Arguments& args) {
	if (args.Length() > 0) {
		v8::String::Utf8Value str(args[0]);
		std::string cmd = ToCString(str);
	} else {
			printf("beginTransaction();\n\tStarts a new transaction\n");
			printf("commit()\n\tCommits the current transaction.\n");
			printf("connect('hostname', [port])\n\tEstablish a connection with a server.\n");
			printf("dropNamespace('db', 'namespace');\n\tDrops a namespace from the db.\n");
			printf("executeQuery('select query');\n\tExecutes a query using dql format.\n");
			printf("executeUpdate('update query');\n\tExecutes an insert, update, remove command using dql.\n");
			printf("find('db', 'namespace'[, 'select'][, 'filter']);\n\tExecutes a find using the provided filter.\n");
			printf("help();\n\tThis help\n");
			printf("insert('db', 'namespace', { json...object});\n\tInserts a new document.\n");
			printf("load('file');\n\tLoads and executes a script.\n");
			printf("print(data);\n\tPrint console messages.\n");
			printf("quit();\n\tBye bye fellow.\n");
			printf("remove('db', 'namespace', 'id', 'revision');\n\tRemoves a document.\n");
			printf("rollback()\n\tRollbacks the current transaction.\n");
			printf("read('file');\n\tReads the contents of a file.\n");
			printf("showDbs();\n\tReturns a list of the available databases.\n");
			printf("showNamespaces('db');\n\tReturns the namespaces in that database.\n");
			printf("shutdown();\n\tRemotely shutdowns the server (Are you sure?).\n");
			printf("update('db', 'namespace', { json...object});\n\tUpdates a document.\n");
			printf("uuid();\n\tGenerates a new UUID (Universal Unique Identifier).\n");
			printf("version();\n\tReturns the current shell version.\n");
	}
	return v8::Undefined();
}

// The callback that is invoked by v8 whenever the JavaScript 'quit'
// function is called.  Quits.
v8::Handle<v8::Value> Quit(const v8::Arguments& args) {
	// If not arguments are given args[0] will yield undefined which
	// converts to the integer value 0.
	if (__djonConnection != NULL) {
		__djonConnection->close();
		delete __djonConnection;
		__djonConnection = 0;
	}

	int exit_code = args[0]->Int32Value();
	fflush(stdout);
	fflush(stderr);

#ifndef WINDOWS
	char* file = getHistoryFilename();
	linenoiseHistorySave(file);
	free(file);
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
	printf("Use help(); to get the commands available. \n(hint: The first command should be \"connect\" to start playing with a server)\n");

	static const int kBufferSize = 256;
	// Enter the execution environment before evaluating any code.
	v8::Context::Scope context_scope(context);
	v8::Local<v8::String> name(v8::String::New("(shell)"));

#ifndef WINDOWS
	char* file = getHistoryFilename();
	linenoiseHistoryLoad(file);
	free(file);
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
				if ((startsWith(cmd, "exit")) || (startsWith(cmd, "quit"))) {
					if (strlen(cmd) <= 5)
						cmd = "quit();";
				}
				if (startsWith(cmd, "help")) {
					if (strlen(cmd) <= 5)
						cmd = "help();";
				}
				ExecuteString(v8::String::New(cmd), name, true, true);
			}
		}
	}
	printf("\n");
}

