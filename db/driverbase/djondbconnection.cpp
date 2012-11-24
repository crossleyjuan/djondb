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

#include "djondbconnection.h"

#include "networkinputstream.h"
#include "networkoutputstream.h"
#include "commandwriter.h"
#include "insertcommand.h"
#include "dropnamespacecommand.h"
#include "findcommand.h"
#include "updatecommand.h"
#include "removecommand.h"
#include "shutdowncommand.h"
#include "shownamespacescommand.h"
#include "showdbscommand.h"
#include "bsoninputstream.h"
#include "djondbconnectionmanager.h"
#include "util.h"
#include "bson.h"

using namespace djondb;

DjondbConnection::DjondbConnection(std::string host)
{
	_host = host;
	_port = SERVER_PORT;
	_inputStream = NULL;
	_outputStream = NULL;
	_commandWriter = NULL;
	_open = false;
	_logger = getLogger(NULL);
}

DjondbConnection::DjondbConnection(std::string host, int port)
{
	_host = host;
	_port = port;
	_inputStream = NULL;
	_outputStream = NULL;
	_commandWriter = NULL;
	_open = false;
	_logger = getLogger(NULL);
}

DjondbConnection::DjondbConnection(const DjondbConnection& orig) {
	this->_host = orig._host;
	this->_port = orig._port;
	this->_inputStream = orig._inputStream;
	this->_open =  orig._open;
	this->_outputStream = orig._outputStream;
	this->_commandWriter = orig._commandWriter;
	_logger = getLogger(NULL);
}

DjondbConnection::~DjondbConnection()
{
	internalClose();
	delete _logger;
}

bool DjondbConnection::open() {
	if (_logger->isDebug()) _logger->debug("Openning connection");
	_outputStream = new NetworkOutputStream();
	int socket = _outputStream->open(_host.c_str(), _port);
	if (socket > 0) {
		_inputStream = new NetworkInputStream(socket);
		_open = true;
		_commandWriter = new CommandWriter(_outputStream);
		if (_logger->isDebug()) _logger->debug("DjondbConnection opened");
		return true;
	} else {
		delete _outputStream;
		return false;
	}
}

void DjondbConnection::close() { 
	if (_logger->isDebug()) _logger->debug("Closing connection");
	DjondbConnectionManager::releaseConnection(this);
	_open = false;
}

void DjondbConnection::internalClose() {
	if (_open) {
		_inputStream->closeStream();
		_outputStream->closeStream();
		if (_inputStream)   {
			delete (_inputStream);
			_inputStream = NULL;
		}
		if (_outputStream)  {
			delete (_outputStream);
			_outputStream = NULL;
		}
		if (_commandWriter) {
			delete (_commandWriter);
			_commandWriter = NULL;
		}
		_open = false;
	}
}

bool DjondbConnection::insert(const std::string& db, const std::string& ns, const std::string& json) {
	if (_logger->isDebug()) _logger->debug(2, "Insert command. db: %s, ns: %s, json: %s", db.c_str(), ns.c_str(), json.c_str());
	BSONObj* obj = BSONParser::parse(json);
	bool result = insert(db, ns, *obj);
	delete obj;
	return result;
}

bool DjondbConnection::shutdown() const {
	if (_logger->isDebug()) _logger->debug(2, "Shutdown command");
	ShutdownCommand cmd;

	_commandWriter->writeCommand(&cmd);
	return true;
}

bool DjondbConnection::insert(const std::string& db, const std::string& ns, const BSONObj& bson) {
	if (_logger->isDebug()) _logger->debug(2, "Insert command. db: %s, ns: %s", db.c_str(), ns.c_str());
	BSONObj obj(bson);
	InsertCommand cmd;
	cmd.setDB(db);
	if (!obj.has("_id")) {
		std::string* id = uuid();
		obj.add("_id", *id);
		delete id;
	}
	if (!obj.has("_revision")) {
		std::string* rev = uuid();
		obj.add("_revision", *rev);
		delete rev;
	}
	cmd.setBSON(obj);
	cmd.setNameSpace(ns);
	_commandWriter->writeCommand(&cmd);

	cmd.readResult(_inputStream);

	int hasResults = false; //_inputStream->readInt();

	if (hasResults) {
		// When the bson didnt contain an id the server will return a bson with it
		// At this moment this will never occur, but I will leave this code for later
	}
	return true;
}

bool DjondbConnection::update(const std::string& db, const std::string& ns, const std::string& json) {
	if (_logger->isDebug()) _logger->debug(2, "Update command. db: %s, ns: %s", db.c_str(), ns.c_str());
	BSONObj* obj = BSONParser::parse(json);
	bool result = update(db, ns, *obj);
	delete obj;
	return result;
}

bool DjondbConnection::remove(const std::string& db, const std::string& ns, const std::string& id, const std::string& revision) {
	if (_logger->isDebug()) _logger->debug(2, "Remove command. db: %s, ns: %s, id: %s, revision: %s", db.c_str(), ns.c_str(), id.c_str(), revision.c_str());
	RemoveCommand cmd;
	cmd.setDB(db);
	cmd.setNameSpace(ns);
	cmd.setId(id);
	cmd.setRevision(revision);

	_commandWriter->writeCommand(&cmd);
	cmd.readResult(_inputStream);

	return true;
}

bool DjondbConnection::update(const std::string& db, const std::string& ns, const BSONObj& obj) {
	if (_logger->isDebug()) _logger->debug(2, "Update command. db: %s, ns: %s", db.c_str(), ns.c_str());
	UpdateCommand cmd;
	cmd.setBSON(obj);
	cmd.setDB(db);
	cmd.setNameSpace(ns);

	_commandWriter->writeCommand(&cmd);
	cmd.readResult(_inputStream);

	return true;
}

std::vector<std::string>* DjondbConnection::dbs() const {
	if (_logger->isDebug()) _logger->debug(2, "dbs command.");

	ShowdbsCommand cmd;
	_commandWriter->writeCommand(&cmd);

	cmd.readResult(_inputStream);

	std::vector<std::string>* result = (std::vector<std::string>*)cmd.result();

	return result;
}

std::vector<std::string>* DjondbConnection::namespaces(const std::string& db) const {
	if (_logger->isDebug()) _logger->debug(2, "namespaces command. db: %s", db.c_str());

	ShownamespacesCommand cmd;
	cmd.setDB(db);
	_commandWriter->writeCommand(&cmd);

	cmd.readResult(_inputStream);

	std::vector<std::string>* result = (std::vector<std::string>*)cmd.result();

	return result;
}

BSONObj* DjondbConnection::findByKey(const std::string& db, const std::string& ns, const std::string& id) {
	return findByKey(db, ns, "*", id);
}

BSONObj* DjondbConnection::findByKey(const std::string& db, const std::string& ns, const std::string& select, const std::string& id) {
	if (_logger->isDebug()) _logger->debug("executing findByKey db: %s, ns: %s, select: %s, id: %s", db.c_str(), ns.c_str(), select.c_str(), id.c_str());

	std::string filter = "$'_id' == '" + id + "'";

	std::vector<BSONObj*>* result = find(db, ns, select, filter);

	BSONObj* res = NULL;
	if (result->size() == 1) {
		if (_logger->isDebug()) _logger->debug(2, "findByKey found 1 result");
		res = *result->begin();
	} else {
		if (result->size() > 1) {
			throw "The result contains more than 1 result";
		}
	}
   BSONObj* bsonresult = NULL;
	if (res != NULL) {
		// creates a copy of the result before deleting the temporal objects
		bsonresult = new BSONObj(*res);
	}

	delete result;
	return bsonresult;
}

std::vector<BSONObj*>* DjondbConnection::find(const std::string& db, const std::string& ns, const std::string& filter) throw(ParseException) {
	return find(db, ns, "*", filter);
}

std::vector<BSONObj*>* DjondbConnection::find(const std::string& db, const std::string& ns, const std::string& select, const std::string& filter) throw(ParseException) {
	if (_logger->isDebug()) _logger->debug("executing find db: %s, ns: %s, select: %s, filter: %s", db.c_str(), ns.c_str(), select.c_str(), filter.c_str());

	try {
		FilterParser* parser = FilterParser::parse(filter);
		delete parser;
	} catch (ParseException e) {
		_logger->error("An error ocurred parsing the filter %s", filter.c_str());
		throw e;
	}
	FindCommand cmd;
	cmd.setFilter(filter);
	cmd.setSelect(select);
	cmd.setDB(db);
	cmd.setNameSpace(ns);
	_commandWriter->writeCommand(&cmd);
	
	cmd.readResult(_inputStream);
	std::vector<BSONObj*>* result = (std::vector<BSONObj*>*)cmd.result();

	return result;
}

bool DjondbConnection::isOpen() const {
	return _open;
}

std::string DjondbConnection::host() const {
	return _host;
}


bool DjondbConnection::dropNamespace(const std::string& db, const std::string& ns) {
	DropnamespaceCommand cmd;

	cmd.setDB(db);
	cmd.setNameSpace(ns);

	_commandWriter->writeCommand(&cmd);
	cmd.readResult(_inputStream);

	return true;
}
