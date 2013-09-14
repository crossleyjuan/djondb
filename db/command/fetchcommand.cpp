// =====================================================================================
// 
//  @file:  fetchcommand.cpp
// 
//  @brief:  Execute fetch over a cursor retrieving the next page
// 
//  @version:  1.0
//  @date:     08/14/2013 09:41:06 PM
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
#include "fetchcommand.h"

#include "inputstream.h"
#include "outputstream.h"
#include "bsoninputstream.h"
#include "bsonoutputstream.h"
#include "bson.h"
#include "controller.h"
#include "dbcursor.h"

FetchCommand::FetchCommand() 
	: Command(FETCHCURSOR) {

	_cursorId = NULL;
	_result = NULL;
}

FetchCommand::~FetchCommand() {
	if (_cursorId != NULL) free(_cursorId);
	if (_result != NULL) delete(_result);
}

FetchCommand::FetchCommand(const FetchCommand& other) 
	: Command(FETCHCURSOR) {
	_cursorId = NULL;
	_result = NULL;
	if (other._cursorId != NULL) {
		this->_cursorId = strcpy(other._cursorId);
	}
	if (other._result != NULL) {
		this->_result = new BSONArrayObj(*other._result);
	}
}

void FetchCommand::execute() {
	DBCursor* cursor = dbController()->fetchCursor(_cursorId);
	_result = NULL;
	if (cursor != NULL) {
		if (cursor->currentPage != NULL) {
			_result = new BSONArrayObj(*cursor->currentPage);
		}
	}
}

void* FetchCommand::result() {
	return _result;
}

void FetchCommand::writeCommand(OutputStream* out) const {
	out->writeChars(_cursorId, strlen(_cursorId));
	out->flush();
}

void FetchCommand::writeResult(OutputStream* out) const {
	if (_result != NULL) {
		out->writeInt(1); // Flag to indicate that there're results
		BSONOutputStream* bout = new BSONOutputStream(out);
		bout->writeBSONArray(_result);
		delete bout;
	} else {
		out->writeInt(0);
	}
}

void FetchCommand::readResult(InputStream* is) {
	int flag = is->readInt();
	if (flag) {
		BSONInputStream* bin = new BSONInputStream(is);
		_result = bin->readBSONArray();
		delete bin;
	} else {
		_result = NULL;
	}
}

void FetchCommand::setCursorId(const char* cursorId) {
	if (_cursorId != NULL) free(_cursorId);
	_cursorId = strcpy(cursorId, strlen(cursorId));
}

const char* FetchCommand::cursorId() const {
	return _cursorId;
}
