// =====================================================================================
// 
//  @file:  findfsworker.cpp
// 
//  @brief:  Implementation of the find full scan worker class, this will hold every state required
//           to pause and resume full scan operations, it will not block any other action.
//           This is intended to be used by commands, but can be used on any other class that could
//           require pausing and resuming full scans
//
//  @version:  1.0
//  @date:     07/10/2013 01:26:25 PM
//  Compiler:  g++
// 
//  @author:  Juan Pablo Crossley (Cross), cross@djondb.com
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
#include "findworker.h"

#include "util.h"
#include "bson.h"
#include "dbfileinputstream.h"
#include "mmapinputstream.h"
#include "dbcontroller.h"
#include "bsoninputstream.h"
#include "filterparser.h"
#include "bsonbufferedobj.h"
#include "bsonoutputstream.h"
#include "expressionresult.h"
#include "command.h"
#include "findcommand.h"
#include "findresultworker.h"
#include "dbcursor.h"

#include <sstream>

#define FREE(element) \
	if (element != NULL) free(element);

#define DELETE(element) \
	if (element != NULL) delete(element);

#define COPYSTR(dest, element) \
	if (element  != NULL) { \
		dest = strcpy(element, strlen(element)); \
	} else { \
		dest = NULL; \
	}

FindWorker::FindWorker(FindCommand* command, InputStream* input, OutputStream* output) {
	_command = command;
	_input = input;
	_output = output;
	setState(WS_SLEEP);
}

FindWorker::~FindWorker() {
	DELETE(_command);
	DELETE(_input);
	DELETE(_output);
}

void FindWorker::resume() {
	if (state() != WS_SLEEP) {
		// the state is not in a waitable task, it should return
		return;
	}

	_command->execute();
	_command->writeResult(_output);
	setState(WS_END_READY);
}

void* FindWorker::result() {
	// the result of a find worker is a result worker which will be processed by the worker engine
	return NULL;
}

Worker* FindWorker::nextActionWorker() {
	return NULL;
}

FindResultWorker::FindResultWorker(BSONArrayObj* array, InputStream* input, OutputStream* output) {
	_array = array;
	_input = input;
	if (output != NULL) {
		_output = output;
		_outputBSONStream = new BSONOutputStream(output);
		setState(WS_SLEEP);
	} else {
		_output = NULL;
		setState(WS_ABORTED); // Nothing to write, so lets abort the operation
	}
	_currentPosition = -1; // This means that it will need to write the length first
}

FindResultWorker::~FindResultWorker() {
	if (_array != NULL) {
		delete _array;
	}
}

void FindResultWorker::resume() {
	if (_currentPosition < 0) {
		__int32 length = _array->length();
		_output->writeLong(length);
		_currentPosition = 0;
	} else {
		if (_currentPosition < _array->length()) {
			BSONObj* obj = _array->get(_currentPosition);
			_outputBSONStream->writeBSON(*obj);
			_currentPosition++;
		} else {
			_output->flush();
			setState(WS_END_READY);
		}
	}
}

void* FindResultWorker::result() {
	return NULL;
}

Worker* FindResultWorker::nextActionWorker() {
	return NULL;
}

