// =====================================================================================
// 
//  @file:  findresultworker.cpp
// 
//  @brief: Implementation of the find result worker, this will send the results using the resume/pause method 
// 
//  @version:  1.0
//  @date:     08/31/2013 08:08:30 AM
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
#include "findresultworker.h"
#include "outputstream.h"
#include "inputstream.h"
#include "bson.h"
#include "bsonoutputstream.h"

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

