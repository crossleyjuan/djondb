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

#include "insertcommand.h"

#include "bsonoutputstream.h"
#include "bsoninputstream.h"
#include "dbcontroller.h"
#include "inputstream.h"
#include "outputstream.h"
#include <memory>

InsertCommand::InsertCommand()
    : Command(INSERT)
{
	_bsonResult = NULL;
}

InsertCommand::InsertCommand(const InsertCommand& orig)
: Command(INSERT) {
	_bsonResult = NULL;
}

InsertCommand::~InsertCommand() {
    delete(_db);
    delete(_namespace);
    delete(_bson);
}

void InsertCommand::execute() {
    const char* ns = _namespace->c_str();
    _bsonResult = dbController()->insert(const_cast<char*>(_db->c_str()), const_cast<char*>(ns), _bson);
}

void* InsertCommand::result() {
    return _bsonResult;
}

void InsertCommand::writeCommand(OutputStream* out) const {
	out->writeString(*_db);
	out->writeString(*_namespace);

	/*
	out->writeString(_bson->toChar());
	*/
	std::auto_ptr<BSONOutputStream> bsonout(new BSONOutputStream(out));
	bsonout->writeBSON(*_bson);
}

void InsertCommand::readResult(InputStream* is) {
	/*
	int result = is->readInt();
	if (result) {
		BSONInputStream* bis = new BSONInputStream(is);
		_bsonResult = bis->readBSON();
		delete bis;
	}
	*/
}

void InsertCommand::writeResult(OutputStream* out) const {
	/*
	out->writeInt((_bsonResult != NULL)? 1: 0);
	if (_bsonResult) {
		BSONOutputStream* bsonout = new BSONOutputStream(out);
		bsonout->writeBSON(*_bsonResult);
	}
	*/
}

void InsertCommand::setNameSpace(const std::string ns) {
	_namespace = new std::string(ns);
}
const std::string* InsertCommand::nameSpace() const {
	return _namespace;
}

void InsertCommand::setBSON(const BSONObj bson) {
	_bson = new BSONObj(bson);
}

BSONObj* InsertCommand::bson() const {
	return _bson;
}

void InsertCommand::setDB(const std::string& db) {
	_db = new std::string(db);
}

const std::string* InsertCommand::DB() const {
	return _db;
}

