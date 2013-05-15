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

FindCommand::FindCommand()
: Command(FIND)
{
	//ctor
	_findresult = NULL;
	_select = NULL;
	_filter = NULL;
	_namespace = NULL;
	_db = NULL;
}

FindCommand::~FindCommand()
{
	if (_select != NULL) delete _select;
	if (_filter != NULL) delete _filter;
	if (_namespace != NULL) delete _namespace;
	if (_db != NULL) delete _db;

	if (_findresult != NULL) {
		delete _findresult;
	}
}

FindCommand::FindCommand(const FindCommand& other) :Command(FIND)
{
	this->_select = new std::string(*other._select);
	this->_filter = new std::string(*other._filter);
	this->_namespace = new std::string(*other._namespace);
	this->_db = new std::string(*other._db);

	if (other._findresult != NULL) {
		this->_findresult = new BSONArrayObj(*other._findresult);
	}
}

void FindCommand::execute() {
	Logger* log = getLogger(NULL);
	if (log->isDebug()) log->debug("executing find command on %s", nameSpace()->c_str());

	_findresult = dbController()->find(const_cast<char*>(DB()->c_str()), const_cast<char*>(nameSpace()->c_str()), select()->c_str(), filter()->c_str(), options());
}

void* FindCommand::result() {
	// Creates a copy
	BSONArrayObj* result = new BSONArrayObj(*_findresult);

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

	BSONInputStream* bsonin = new BSONInputStream(is);
	BSONArrayObj* result = bsonin->readBSONArray();
	_findresult = result;

	delete bsonin;
}

void FindCommand::writeResult(OutputStream* out) const {
	Logger* log = getLogger(NULL);
	if (log->isDebug()) log->debug("writing result of find command on %s", nameSpace()->c_str());
	BSONOutputStream* bsonout = new BSONOutputStream(out);
	bsonout->writeBSONArray(_findresult);
	delete bsonout;
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

