#include "connection.h"

#include "networkinputstream.h"
#include "networkoutputstream.h"
#include "commandwriter.h"
#include "insertcommand.h"
#include "findbykeycommand.h"
#include "updatecommand.h"
#include "bsoninputstream.h"
#include "connectionmanager.h"

using namespace djondb;

Connection::Connection(std::string host)
{
    _host = host;
    _inputStream = NULL;
    _outputStream = NULL;
    _commandWriter = NULL;
    _open = false;
}

Connection::Connection(const Connection& orig) {
    this->_host = orig._host;
    this->_inputStream = orig._inputStream;
    this->_open =  orig._open;
    this->_outputStream = orig._outputStream;
    this->_commandWriter = orig._commandWriter;
}

Connection::~Connection()
{
    internalClose();
}

void Connection::open() {
    _outputStream = new NetworkOutputStream();
    int socket = _outputStream->open(_host.c_str(), 1243);
    _inputStream = new NetworkInputStream(socket);
    _open = true;
    _commandWriter = new CommandWriter(_outputStream);
}

void Connection::close() {
    ConnectionManager::releaseConnection(this);
}

void Connection::internalClose() {
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

bool Connection::insert(const std::string& ns, const std::string& json) {
    BSONObj* obj = BSONParser::parse(json);
    bool result = insert(ns, *obj);
    delete obj;
    return result;
}

bool Connection::insert(const std::string& ns, const BSONObj& bson) {
    InsertCommand cmd;
    cmd.setBSON(bson);
    cmd.setNameSpace(ns);
    _commandWriter->writeCommand(&cmd);
    return true;
}

bool Connection::update(const std::string& ns, const std::string& json) {
    BSONObj* obj = BSONParser::parse(json);
    bool result = update(ns, *obj);
    delete obj;
    return result;
}

bool Connection::update(const std::string& ns, const BSONObj& obj) {
    UpdateCommand cmd;
    cmd.setBSON(obj);
    cmd.setNameSpace(ns);

    _commandWriter->writeCommand(&cmd);
}

BSONObj* Connection::findByKey(const std::string& ns, const std::string& id) {
    BSONObj filter;
    filter.add("_id", id);

    FindByKeyCommand cmd;
    cmd.setBSON(filter);
    cmd.setNameSpace(ns);
    _commandWriter->writeCommand(&cmd);

    BSONInputStream is(_inputStream);
    BSONObj* res = is.readBSON();
    return res;
}

bool Connection::isOpen() const {
    return _open;
}

std::string Connection::host() const {
    return _host;
}