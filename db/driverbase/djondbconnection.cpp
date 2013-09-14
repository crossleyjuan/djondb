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
#include "commitcommand.h"
#include "rollbackcommand.h"
#include "djondbcursor.h"
#include "dqlParser.h"
#include "dqlLexer.h"
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
	_activeTransactionId = NULL;
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
	_activeTransactionId = NULL;
}

DjondbConnection::DjondbConnection(const DjondbConnection& orig) {
	this->_host = orig._host;
	this->_port = orig._port;
	this->_inputStream = orig._inputStream;
	this->_open =  orig._open;
	this->_outputStream = orig._outputStream;
	this->_commandWriter = orig._commandWriter;
	_logger = getLogger(NULL);
	this->_activeTransactionId = orig._activeTransactionId;
}

DjondbConnection::~DjondbConnection()
{
	internalClose();
	if (_activeTransactionId != NULL) free(_activeTransactionId);
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

const char* DjondbConnection::beginTransaction() {
	_activeTransactionId = uuid();
	return _activeTransactionId->c_str();
}

void DjondbConnection::commitTransaction() {
	if (_logger->isDebug()) _logger->debug(2, "commitTransaction. transactionId: %s", _activeTransactionId->c_str());
	CommitCommand cmd;
	prepareOptions((Command*)&cmd);
	cmd.setTransactionId(*_activeTransactionId);
	_commandWriter->writeCommand(&cmd);

	cmd.readResult(_inputStream);
	delete _activeTransactionId;
	_activeTransactionId = NULL;
}

void DjondbConnection::rollbackTransaction() {
	if (_logger->isDebug()) _logger->debug(2, "rollbackTransaction. transactionId: %s", _activeTransactionId->c_str());
	RollbackCommand cmd;
	prepareOptions((Command*)&cmd);
	cmd.setTransactionId(*_activeTransactionId);
	_commandWriter->writeCommand(&cmd);

	cmd.readResult(_inputStream);
	delete _activeTransactionId;
	_activeTransactionId = NULL;
}

void DjondbConnection::internalClose() {
	if (_open) {
		_inputStream->close();
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

void DjondbConnection::prepareOptions(Command* cmd) {
	BSONObj* options;
	if (cmd->options() != NULL) {
		options = new BSONObj(*cmd->options());
	} else {
		options = new BSONObj();
	}
	if (_activeTransactionId != NULL) {
		options->add("_transactionId", const_cast<char*>(_activeTransactionId->c_str()));
	}

	cmd->setOptions(options);
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

	if (!isOpen()) {
		throw DjondbException(D_ERROR_CONNECTION, "Not connected to any server");
	}
	BSONObj* obj = new BSONObj(bson);
	InsertCommand cmd;
	cmd.setDB(db);
	if (!obj->has("_id")) {
		std::string* id = uuid();
		obj->add("_id", const_cast<char*>(id->c_str()));
		delete id;
	}
	if (!obj->has("_revision")) {
		std::string* rev = uuid();
		obj->add("_revision", const_cast<char*>(rev->c_str()));
		delete rev;
	}
	cmd.setBSON(obj);
	cmd.setNameSpace(ns);
	prepareOptions((Command*)&cmd);
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
	if (_logger->isDebug()) _logger->debug(2, "Update command. db: %s, ns: %s, json: %s", db.c_str(), ns.c_str(), json.c_str());
	BSONObj* obj = BSONParser::parse(json);
	bool result = update(db, ns, *obj);
	delete obj;
	return result;
}

bool DjondbConnection::remove(const std::string& db, const std::string& ns, const std::string& id, const std::string& revision) {
	if (_logger->isDebug()) _logger->debug(2, "Remove command. db: %s, ns: %s, id: %s, revision: %s", db.c_str(), ns.c_str(), id.c_str(), revision.c_str());

	if (!isOpen()) {
		throw DjondbException(D_ERROR_CONNECTION, "Not connected to any server");
	}
	RemoveCommand cmd;
	cmd.setDB(db);
	cmd.setNameSpace(ns);
	cmd.setId(id);
	cmd.setRevision(revision);

	prepareOptions((Command*)&cmd);
	_commandWriter->writeCommand(&cmd);
	cmd.readResult(_inputStream);

	return true;
}

bool DjondbConnection::update(const std::string& db, const std::string& ns, const BSONObj& obj) {
	if (_logger->isDebug()) _logger->debug(2, "Update command. db: %s, ns: %s, bson: %s", db.c_str(), ns.c_str(), BSONObj(obj).toChar());

	if (!isOpen()) {
		throw DjondbException(D_ERROR_CONNECTION, "Not connected to any server");
	}

	if (!obj.has("_id") || !obj.has("_revision")) {
		throw DjondbException(D_ERROR_INVALID_STATEMENT, "The update command requires a document with _id and _revision.");
	}
	UpdateCommand cmd;
	cmd.setBSON(obj);
	cmd.setDB(db);
	cmd.setNameSpace(ns);

	prepareOptions((Command*)&cmd);
	_commandWriter->writeCommand(&cmd);
	cmd.readResult(_inputStream);

	return true;
}

std::vector<std::string>* DjondbConnection::dbs() const {
	if (_logger->isDebug()) _logger->debug(2, "dbs command.");

	if (!isOpen()) {
		throw DjondbException(D_ERROR_CONNECTION, "Not connected to any server");
	}
	ShowdbsCommand cmd;
	_commandWriter->writeCommand(&cmd);

	cmd.readResult(_inputStream);

	std::vector<std::string>* result = (std::vector<std::string>*)cmd.result();

	return result;
}

std::vector<std::string>* DjondbConnection::namespaces(const std::string& db) const {
	if (_logger->isDebug()) _logger->debug(2, "namespaces command. db: %s", db.c_str());

	if (!isOpen()) {
		throw DjondbException(D_ERROR_CONNECTION, "Not connected to any server");
	}
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

	if (!isOpen()) {
		throw DjondbException(D_ERROR_CONNECTION, "Not connected to any server");
	}
	std::string filter = "$'_id' == '" + id + "'";

	DjondbCursor* result = find(db, ns, select, filter);

	BSONObj* res = NULL;
	if (result->length() == 1) {
		if (_logger->isDebug()) _logger->debug(2, "findByKey found 1 result");
		if (result->next()) {
			res = result->current();
		} else {
			res = NULL;
		}
	} else {
		if (result->length() > 1) {
			throw DjondbException(D_ERROR_TOO_MANY_RESULTS, "The result contains more than 1 result");
		}
	}
   BSONObj* bsonresult = NULL;
	if (res != NULL) {
		// creates a copy of the result before deleting the temporal objects
		bsonresult = new BSONObj(*res);
	}

	result->releaseCursor();
	return bsonresult;
}

DjondbCursor* DjondbConnection::find(const std::string& db, const std::string& ns) {
	return find(db, ns, "*", "", BSONObj());
}

DjondbCursor* DjondbConnection::find(const std::string& db, const std::string& ns, const BSONObj& options) {
	return find(db, ns, "*", "", options);
}

DjondbCursor* DjondbConnection::find(const std::string& db, const std::string& ns, const std::string& filter) {
	return find(db, ns, "*", filter, BSONObj());
}

DjondbCursor* DjondbConnection::find(const std::string& db, const std::string& ns, const std::string& filter, const BSONObj& options) {
	return find(db, ns, "*", filter, options);
}

DjondbCursor* DjondbConnection::find(const std::string& db, const std::string& ns, const std::string& select, const std::string& filter) {
	return find(db, ns, select, filter, BSONObj());
}

DjondbCursor* DjondbConnection::find(const std::string& db, const std::string& ns, const std::string& select, const std::string& filter, const BSONObj& options) {
	if (_logger->isDebug()) _logger->debug("executing find db: %s, ns: %s, select: %s, filter: %s", db.c_str(), ns.c_str(), select.c_str(), filter.c_str());

	if (!isOpen()) {
		throw DjondbException(D_ERROR_CONNECTION, "Not connected to any server");
	}
	try {
		FilterParser* parser = FilterParser::parse(filter);
		delete parser;
	} catch (ParseException e) {
		_logger->error("An error ocurred parsing the filter %s", filter.c_str());
		throw DjondbException(e.errorCode(), e.what());
	}
	FindCommand cmd;
	cmd.setFilter(filter);
	cmd.setSelect(select);
	cmd.setDB(db);
	cmd.setNameSpace(ns);
	cmd.setOptions(&options);
	prepareOptions((Command*)&cmd);
	_commandWriter->writeCommand(&cmd);
	
	cmd.readResult(_inputStream);

	const char* cursorId = (const char*)cmd.cursorId();
	BSONArrayObj* array = cmd.readedResult();

	DjondbCursor* cursor = new DjondbCursor(_outputStream, _inputStream, _commandWriter, cursorId, array);

	return cursor;
}

bool DjondbConnection::isOpen() const {
	return _open;
}

std::string DjondbConnection::host() const {
	return _host;
}


bool DjondbConnection::dropNamespace(const std::string& db, const std::string& ns) {
	if (!isOpen()) {
		throw DjondbException(D_ERROR_CONNECTION, "Not connected to any server");
	}
	DropnamespaceCommand cmd;

	cmd.setDB(db);
	cmd.setNameSpace(ns);

	prepareOptions((Command*)&cmd);
	_commandWriter->writeCommand(&cmd);
	cmd.readResult(_inputStream);

	return true;
}

BSONArrayObj* DjondbConnection::executeQuery(const std::string& query) {
	Command* cmd = parseCommand(query);

	prepareOptions(cmd);
	_commandWriter->writeCommand(cmd);
	cmd->readResult(_inputStream);
	BSONArrayObj* result = NULL;
	if (cmd->commandType() == FIND) {
		BSONArrayObj* tmp = (BSONArrayObj*)cmd->result();
		if (tmp != NULL) {
			result = new BSONArrayObj(*tmp);
			delete tmp;
		}
	}
	delete cmd;
	return result;
}

bool DjondbConnection::executeUpdate(const std::string& query) {
	Command* cmd = parseCommand(query);

	prepareOptions(cmd);
	_commandWriter->writeCommand(cmd);
	cmd->readResult(_inputStream);
	delete cmd;
	return true;
}

Command* DjondbConnection::parseCommand(const std::string& expression) {
	Logger* log = getLogger(NULL);
	Command* cmd = NULL;

	int errorCode = -1;
	const char* errorMessage;
	if (expression.length() != 0) {
		//throw (ParseException) {
		pANTLR3_INPUT_STREAM           input;
		pdqlLexer               lex;
		pANTLR3_COMMON_TOKEN_STREAM    tokens;
		pdqlParser              parser;

		const char* cexpr = expression.c_str();
		if (log->isDebug()) log->debug("query expression: %s, len: %d, Size Hint: %d", cexpr, strlen(cexpr), ANTLR3_SIZE_HINT);
		input  = antlr3StringStreamNew((pANTLR3_UINT8)cexpr, 8, (ANTLR3_UINT32)strlen(cexpr), (pANTLR3_UINT8)"name");
		lex    = dqlLexerNew                (input);
		tokens = antlr3CommonTokenStreamSourceNew  (ANTLR3_SIZE_HINT, TOKENSOURCE(lex));
		parser = dqlParserNew               (tokens);

		cmd = parser ->start_point(parser);

		if (parser->pParser->rec->state->exception != NULL) {
			errorCode = 1;
			errorMessage = (char*)parser->pParser->rec->state->exception->message;
		}

		// Must manually clean up
		//
		parser ->free(parser);
		tokens ->free(tokens);
		lex    ->free(lex);
		input  ->close(input);
	}
	if (errorCode > -1) {
		throw ParseException(errorCode, errorMessage);
	}

	return cmd;
	}
