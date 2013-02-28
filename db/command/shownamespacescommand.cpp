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

#include "shownamespacescommand.h"

#include "dbcontroller.h"
#include "outputstream.h"
#include "inputstream.h"

ShownamespacesCommand::ShownamespacesCommand()
    : Command(SHOWNAMESPACES)
{
	_results = NULL;
	_db = NULL;
}

ShownamespacesCommand::ShownamespacesCommand(const ShownamespacesCommand& orig)
: Command(SHOWNAMESPACES) {
	_results = NULL;
	if (orig._results != NULL) {
		this->_results = new std::vector<std::string>();
		for (std::vector<std::string>::iterator i = orig._results->begin(); i != orig._results->end(); i++) {
			this->_results->push_back(*i);
		}
	}
	this->_db = NULL;
	if (orig._db != NULL) {
		this->_db = new std::string(*orig._db);
	}
}

ShownamespacesCommand::~ShownamespacesCommand() {
	if (_db != NULL) delete _db;
}

void ShownamespacesCommand::execute() {
	_results = dbController()->namespaces(_db->c_str());
}

void* ShownamespacesCommand::result() {
    return _results;
}

void ShownamespacesCommand::writeCommand(OutputStream* out) const {
	out->writeString(*_db);
}

void ShownamespacesCommand::readResult(InputStream* is)  {
	int results = is->readInt();
	_results = new std::vector<std::string>();
	for (int x = 0; x < results; x++) {
		std::string* db = is->readString();
		_results->push_back(*db);
		delete db;
	}
}

void ShownamespacesCommand::writeResult(OutputStream* out) const {
	out->writeInt(_results->size());
	for (std::vector<std::string>::iterator i = _results->begin(); i != _results->end(); i++) {
		std::string db = *i;
		out->writeString(db);
	}
}

void ShownamespacesCommand::setDB(const std::string& db) {
	_db = new std::string(db);
}

const std::string* ShownamespacesCommand::DB() const {
	return _db;
}

