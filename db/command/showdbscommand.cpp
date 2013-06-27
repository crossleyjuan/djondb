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

#include "showdbscommand.h"

#include "dbcontroller.h"
#include "outputstream.h"
#include "inputstream.h"

ShowdbsCommand::ShowdbsCommand()
    : Command(SHOWDBS)
{
	_results = NULL;
}

ShowdbsCommand::ShowdbsCommand(const ShowdbsCommand& orig)
: Command(SHOWDBS) {
	_results = NULL;
	if (orig._results != NULL) {
		this->_results = new std::vector<std::string>();
		for (std::vector<std::string>::iterator i = orig._results->begin(); i != orig._results->end(); i++) {
			this->_results->push_back(*i);
		}
	}
}

ShowdbsCommand::~ShowdbsCommand() {
	if (_results != NULL)  {
		delete _results;
	}
}

void ShowdbsCommand::execute() {
	_results = dbController()->dbs();
}

void* ShowdbsCommand::result() {
	std::vector<std::string>* result = NULL;
  	if (_results != NULL) {
		result = new std::vector<std::string>();
		for (std::vector<std::string>::iterator i = _results->begin(); i != _results->end(); i++) {
			result->push_back(*i);
		}
	}
  return result;
}

void ShowdbsCommand::writeCommand(OutputStream* out) const {
}

void ShowdbsCommand::readResult(InputStream* is)  {
	int results = is->readInt();
	_results = new std::vector<std::string>();
	for (int x = 0; x < results; x++) {
		std::string* db = is->readString();
		_results->push_back(*db);
		delete db;
	}
}

void ShowdbsCommand::writeResult(OutputStream* out) const {
	out->writeInt(_results->size());
	for (std::vector<std::string>::iterator i = _results->begin(); i != _results->end(); i++) {
		std::string db = *i;
		out->writeString(db);
	}
}

