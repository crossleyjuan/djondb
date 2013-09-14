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
#include "findfsworker.h"

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

#include <sstream>

FindFullscanWorker::FindFullscanWorker(Command* command, InputStream* input, OutputStream* output) {
	_bufferedObj = NULL;
	_stream = NULL;
	_dataDir = getSetting("DATA_DIR");
	_filterParser = NULL;
	_bufferedObj = NULL;
	_input = input;
	_output = output;
	setState(WS_SLEEP);
	if (command->commandType() != FIND) {
		// This should not happen ever, FindFullscanWorker only works with Find Commands
		throw "Invalid command type for this worker";
	}
	_command = (FindCommand*)command;
	initialize(_command->DB()->c_str(), _command->nameSpace()->c_str(), _command->select()->c_str(), _command->filter()->c_str(), _command->options());
}

FindFullscanWorker::~FindFullscanWorker() {
	if (_stream != NULL) {
		_stream->close();
		delete _stream;
	}
	delete _filterParser;
}

void FindFullscanWorker::resume() {
	if (state() != WS_SLEEP) {
		// the state is not in a waitable task, it should return
		return;
	}
	if (!_stream->eof() && (_count < _maxResults)) {
		if (_bufferedObj == NULL) {
			_bufferedObj = new BSONBufferedObj(_stream->pointer(), _stream->length() - _stream->currentPos());
		} else {
			_bufferedObj->reset(_stream->pointer(), _stream->length() - _stream->currentPos());
		}
		_stream->seek(_stream->currentPos() + _bufferedObj->bufferLength());
		// Only "active" Records
		if (_bufferedObj->getInt("_status") == 1) {
			bool match = false;
			ExpressionResult* expresult = _filterParser->eval(*_bufferedObj);
			if (expresult->type() == ExpressionResult::RT_BOOLEAN) {
				match = *expresult;
			}
			delete expresult;
			if (match) {
				BSONObj* objSubselect = _bufferedObj->select(_select);
				_result->add(*objSubselect);
				delete objSubselect;
				_count++;
			}
		}
	} else {
		setState(WS_END_READY);
		if (_bufferedObj != NULL) delete _bufferedObj;
	}

}

void FindFullscanWorker::initialize(const char* db, const char* ns, const char* select, const char* filter, const BSONObj* options) {
	if (_log->isDebug()) _log->debug(2, "DBController::find db: %s, ns: %s, select: %s, filter: %s", db, ns, select, filter);
	_db = db;
	_ns = ns;
	_select = select;
	_filter = filter;
	_options = options;

	setState(WS_SLEEP);

	initializeFullScan();
}

void FindFullscanWorker::initializeFullScan() {
	std::string filedir = combinePath(_dataDir.c_str(), _db);
	filedir = filedir + FILESEPARATOR;

	std::stringstream ss;
	ss << _ns << ".dat";
	std::string fileName = ss.str();

	char* fullFileName = combinePath(filedir.c_str(), fileName.c_str());

	// Execute open on streammanager, just to check that the file was alrady opened
	StreamManager::getStreamManager()->open(_db, _ns, INDEX_FTYPE);
	// Execute open on streammanager, just to check that the file was alrady opened
	StreamManager::getStreamManager()->open(_db, _ns, DATA_FTYPE);

	//FileInputStream* fis = new FileInputStream(fullFileName.c_str(), "rb");
	_stream = new MMapInputStream(fullFileName, 0);

	_filterParser = FilterParser::parse(_filter);
	std::set<std::string> tokens = _filterParser->xpathTokens();
	std::string filterSelect;

	if ((strcmp(_select, "*") != 0) && (tokens.size() > 0)) {
		// this will reserve enough space to concat the filter tokens
		filterSelect.reserve(tokens.size() * 100);
		filterSelect.append("$'_status'");
		for (std::set<std::string>::iterator i = tokens.begin(); i != tokens.end(); i++) {
			std::string token = *i;
			filterSelect.append(", ");
			filterSelect.append("$'");
			filterSelect.append(token);
			filterSelect.append("'");
		}
	} else {
		filterSelect = "*";
	}

	_stream->seek(29);

	_maxResults = 3000;
	if ((_options != NULL) && _options->has("limit")) {
		BSONContent* content = _options->getContent("limit");
		if (content->type() == INT_TYPE) {
			_maxResults = _options->getInt("limit");
		} else if (content->type() == LONG_TYPE) {
			_maxResults = _options->getLong("limit");
		}
	} else {
		std::string smax = getSetting("max_results");
		if (smax.length() > 0) {
#ifdef WINDOWS
			_maxResults = _atoi64(smax.c_str());
#else
			_maxResults = atoll(smax.c_str());
#endif
		}
	}
	_count = 0;

	_result = new BSONArrayObj();
	free(fullFileName);
}

void* FindFullscanWorker::result() {
	if (state() == WS_END_READY) {
		return _result;
	} else {
		return NULL;
	}
}

Worker* FindFullscanWorker::nextActionWorker() {
	Worker* worker = new FindResultWorker(_result, _input, _output);
	return worker;
}
