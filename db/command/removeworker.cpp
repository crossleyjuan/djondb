// =====================================================================================
// 
//  @file:  removeworker.cpp
// 
//  @brief: 
// 
//  @version:  1.0
//  @date:     07/22/2013 11:02:34 AM
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
#include "removeworker.h"

#include "command.h"
#include "removecommand.h"
#include "inputstream.h"
#include "outputstream.h"
#include "bson.h"
#include "dbcontroller.h"

RemoveWorker::RemoveWorker(Command* command, InputStream* input, OutputStream* output) {
	if (command->commandType() != REMOVE) {
		throw "Remove worker only works with RemoveCommands";
	}

	_command = (RemoveCommand*)command;
	_input = input;
	_output = output;
	setState(WS_SLEEP);
}

RemoveWorker::~RemoveWorker() {
	delete _command;
}

void RemoveWorker::resume() {
	if (state() != WS_SLEEP) {
		return;
	}

	_command->execute();
	_command->writeResult(_output);

	setState(WS_END_READY);
}

void* RemoveWorker::result() {
	return NULL;
}

Worker* RemoveWorker::nextActionWorker() {
	return NULL;
}

