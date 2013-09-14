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

#include "findcommand.h"

#include "bsonoutputstream.h"
#include "bsoninputstream.h"
#include "dbcontroller.h"
#include "outputstream.h"
#include "util.h"
#include "dbcursor.h"

FindCommand::FindCommand()
: Command(FIND)
{
	//ctor
	_arrayresult = NULL;
	_select = NULL;
	_filter = NULL;
	_namespace = NULL;
	_db = NULL;
	_cursorId = NULL;
	_readedResult = NULL;
}

FindCommand::~FindCommand()
{
	if (_select != NULL) delete _select;
	if (_filter != NULL) delete _filter;
	if (_namespace != NULL) delete _namespace;
	if (_db != NULL) delete _db;

	if (_cursorId != NULL) {
		free(_cursorId);
	}
}

FindCommand::FindCommand(const FindCommand& other) :Command(FIND)
{
	this->_select = new std::string(*other._select);
	this->_filter = new std::string(*other._filter);
	this->_namespace = new std::string(*other._namespace);
	this->_db = new std::string(*other._db);

	if (other._cursorId != NULL) {
		this->_cursorId = strcpy(other._cursorId);
	} else {
		this->_cursorId = NULL;
	}
}

void FindCommand::execute() {
	Logger* log = getLogger(NULL);
	if (log->isDebug()) log->debug("executing find command on %s", nameSpace()->c_str());

	DBCursor* cursor = dbController()->find(const_cast<char*>(DB()->c_str()), const_cast<char*>(nameSpace()->c_str()), select()->c_str(), filter()->c_str(), options());

	_cursorId = strcpy(cursor->cursorId);

	// IF the version of the client is less than 0.3.1 means the client is expecting a full BSONArrayObj
	if (*version() <= Version("0.3.1")) {
		BSONArrayObj* buffer = new BSONArrayObj();

		while ((cursor = dbController()->fetchCursor(cursor->cursorId))->currentPage != NULL) {
			buffer->addAll(*cursor->currentPage);
		}
		_arrayresult = buffer;
	}
}

void* FindCommand::result() {
	void* result;
	if (*version() < Version("0.3.1")) {
		result = _arrayresult;
	} else {
		result = _readedResult;
	}
	return result;
}

void FindCommand::writeCommand(OutputStream* out) const {
	out->writeString(*_db);
	out->writeString(*_namespace);
	if (_filter != NULL) {
		out->writeString(*_filter);
	} else {
		out->writeString("");
	}
	out->writeString(*_select);
}

void FindCommand::readResult(InputStream* is) {
	Logger* log = getLogger(NULL);
	if (log->isDebug()) log->debug("writing result of find command on %s", nameSpace()->c_str());

	if (*version() < Version("0.3.1")) {
		BSONInputStream* bsonin = new BSONInputStream(is);
		BSONArrayObj* result = bsonin->readBSONArray();
		_arrayresult = result;
		delete bsonin;
	} else {
		_cursorId = is->readChars();
		int flag = is->readInt();
		if (flag == 1) {
			BSONInputStream* bis = new BSONInputStream(is);
			_readedResult = bis->readBSONArray();
			delete bis;
		}
	}
}

void FindCommand::writeResult(OutputStream* out) const {
	Logger* log = getLogger(NULL);
	if (log->isDebug()) log->debug("writing result of find command on %s", nameSpace()->c_str());
	if (*version() < Version("0.3.1")) {
		BSONOutputStream* bsonout = new BSONOutputStream(out);
		bsonout->writeBSONArray(_arrayresult);
		delete bsonout;
	} else {
		out->writeChars(_cursorId, strlen(_cursorId));
		if (_arrayresult != NULL) {
			out->writeInt(1); // flag - There's a page to be loaded
			BSONOutputStream* bos = new BSONOutputStream(out);
			bos->writeBSONArray(_arrayresult);
			delete bos;
		} else {
			out->writeInt(0); // flag - There's a page to be loaded
		}
	}
	out->flush();
}

void FindCommand::setNameSpace(const std::string& ns) {
	_namespace = new std::string(ns);
}

std::string* FindCommand::nameSpace() const {
	return _namespace;
}

void FindCommand::setSelect(const std::string& select) {
	_select = new std::string(select);
}

std::string* FindCommand::select() const {
	return _select;
}

void FindCommand::setFilter(const std::string& filter) {
	_filter = new std::string(filter);
}

std::string* FindCommand::filter() const {
	return _filter;
}

void FindCommand::setDB(const std::string& db) {
	_db = new std::string(db);
}

const std::string* FindCommand::DB() const {
	return _db;
}

BSONArrayObj* const FindCommand::arrayResult() const {
	return _arrayresult;
}

BSONArrayObj* const FindCommand::readedResult() const {
	return _readedResult;
}

const char* FindCommand::cursorId() const {
	return _cursorId;
}
