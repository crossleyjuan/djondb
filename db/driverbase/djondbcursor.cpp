// =====================================================================================
// 
//  @file:  djondbcursor.cpp
// 
//  @brief:  Implementation of the client side cursor
// 
//  @version:  1.0
//  @date:     08/30/2013 01:11:54 PM
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
#include "djondbcursor.h"

#include "djondbconnection.h"
#include "networkinputstream.h"
#include "networkoutputstream.h"
#include "commandwriter.h"
#include "commandfactory.h"
#include "util.h"

using namespace djondb;

DjondbCursor::DjondbCursor(NetworkOutputStream* nos, NetworkInputStream* nis, CommandWriter* writer, const char* cursorId, BSONArrayObj* firstPage) {
	_cursorId = strcpy(cursorId, strlen(cursorId));
	if (firstPage != NULL) {
		_rows = new BSONArrayObj(*firstPage);
	} else {
		_rows = new BSONArrayObj();
	}
	_current = NULL;
	_position = 0;
	if (firstPage != NULL) {
		_count = firstPage->length();
	} else {
		_count = 0;
	}
	_status = CS_LOADING;
	_inputStream = nis;
	_outputStream = nos;
	_commandWriter = writer;
}

DjondbCursor::DjondbCursor(const DjondbCursor& orig) {
	_cursorId = strcpy(orig._cursorId, strlen(orig._cursorId));
	_rows = new BSONArrayObj(*orig._rows);
	_current = NULL;
	_position = orig._position;
	_count = orig._count;
	_status = orig._status;
	_inputStream = orig._inputStream;
	_outputStream = orig._outputStream;
	_commandWriter = orig._commandWriter;
}

DjondbCursor::~DjondbCursor() {
	delete _rows;
	// Send release cursor message
	releaseCursor();
	free(_cursorId);
}

bool DjondbCursor::next() {
	bool result;
	if (_count > _position) {
		retrieveCurrent();
		_position++;
		result = true;
	} else {
		if (_status == CS_LOADING) {
			FetchCommand* cmd = fetchCommand(_cursorId);
			_commandWriter->writeCommand(cmd);
			cmd->readResult(_inputStream);
			BSONArrayObj* page = (BSONArrayObj*)cmd->result();
			if (page == NULL) {
				_status = CS_RECORDS_LOADED;
			} else {
				_rows->addAll(*page);
				_count += page->length();
				return next();
			}
		} else {
			result = false;
		}
	}
	return result;
}

void DjondbCursor::retrieveCurrent() {
	_current = _rows->get(_position);
}

bool DjondbCursor::previous() {

}

void DjondbCursor::seek(__int32 position) {
	if (position > _count) {
		if (_status != CS_LOADING) {
			_position = _count;
			retrieveCurrent();
		}
	}
}

BSONObj* const DjondbCursor::current() {
	return _current;
}

void DjondbCursor::releaseCursor() {
}

__int32 DjondbCursor::length() {
	if (_status == CS_LOADING) {
		__int32 savedPos = _position;
		while (true) {
			if (!next()) {
				break;
			}
		};
		_position = savedPos;
	}
	return _count;
}

