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

#include "removecommand.h"

#include "bsonoutputstream.h"
#include "inputstream.h"
#include "outputstream.h"
#include "bsoninputstream.h"
#include "dbcontroller.h"
#include <memory>

RemoveCommand::RemoveCommand()
    : Command(REMOVE)
{
}

RemoveCommand::RemoveCommand(const RemoveCommand& orig)
: Command(REMOVE) {
}

RemoveCommand::~RemoveCommand() {
    delete(_db);
    delete(_namespace);
    delete(_id);
    delete(_revision);
}

void RemoveCommand::execute() {
    dbController()->remove(const_cast<char*>(_db->c_str()), const_cast<char*>(_namespace->c_str()), const_cast<char*>(_id->c_str()), const_cast<char*>(_revision->c_str()));
}

void* RemoveCommand::result() {
    return NULL;
}

void RemoveCommand::writeCommand(OutputStream* out) const {
	out->writeString(*_db);
	out->writeString(*_namespace);
	out->writeString(*_id);
	out->writeString(*_revision);
}

void RemoveCommand::readResult(InputStream* is)  {
}

void RemoveCommand::writeResult(OutputStream* out) const {
}

void RemoveCommand::setNameSpace(const std::string& ns) {
	_namespace = new std::string(ns);
}

const std::string* RemoveCommand::nameSpace() const {
	return _namespace;
}

void RemoveCommand::setId(const std::string& id) {
	_id = new std::string(id);
}

const std::string* RemoveCommand::id() const {
	return _id;
}

void RemoveCommand::setRevision(const std::string& revision) {
	_revision = new std::string(revision);
}

const std::string* RemoveCommand::revision() const {
	return _revision;
}

void RemoveCommand::setDB(const std::string& db) {
	_db = new std::string(db);
}

const std::string* RemoveCommand::DB() const {
	return _db;
}

