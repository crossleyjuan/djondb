/*
 * =====================================================================================
 *
 *       Filename:  transactioncontroller.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  09/26/2012 08:26:29 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Juan Pablo Crossley (Cross), crossleyjuan@gmail.com
 *   Organization:  djondb
 *
 * This file is part of the djondb project, for license information please refer to the LICENSE file,
 * the application and libraries are provided as-is and free of use under the terms explained in the file LICENSE
 * Its authors create this application in order to make the world a better place to live, but you should use it on
 * your own risks.
 * 
 * Also, be adviced that, the GPL license force the committers to ensure this application will be free of use, thus
 * if you do any modification you will be required to provide it for free unless you use it for personal use (you may 
 * charge yourself if you want), bare in mind that you will be required to provide a copy of the license terms that ensures
 * this program will be open sourced and all its derivated work will be too.
 * =====================================================================================
 */
#include "transactioncontroller.h"
#include "controller.h"
#include "settings.h"
#include "fileinputoutputstream.h"
#include "memorystream.h"
#include "util.h"
#include "command.h"
#include "commandwriter.h"
#include "commandreader.h"
#include "insertcommand.h"
#include "dropnamespacecommand.h"
#include "updatecommand.h"
#include "removecommand.h"

TransactionController::TransactionController(Controller* controller) {
	_controller = controller;
	_transactionId = NULL;

	_dataDir = getSetting("DATA_DIR");

	loadControlFile();
}

TransactionController::TransactionController(Controller* controller, std::string transactionId) {
	_controller = controller;
	_transactionId = new std::string(transactionId);

	_dataDir = getSetting("DATA_DIR");

	loadControlFile();
}

void TransactionController::loadControlFile() {
	std::string controlFile = (_transactionId == NULL)? "main.trc": *_transactionId + ".trc";
	std::string controlFileName = _dataDir + FILESEPARATOR + controlFile;
	bool existControl = existFile(controlFileName.c_str());

	_controlFile = new FileInputOutputStream(controlFileName.c_str(), "bw+"); 
	if (existControl) {
		_control.startPos 	  = _controlFile->readLong();
		_control.lastValidPos  = _controlFile->readLong();

		while (!_controlFile->eof()) {
			std::string* fileName = _controlFile->readString();
			FileInputOutputStream* logFile = new FileInputOutputStream(fileName->c_str(), "bw+");
			_control.logFiles.push_back(logFile);
			_control.currentFile = logFile;
			delete fileName;
		}
	} else {
		_control.startPos = 0;
		_control.lastValidPos = 0;

		std::string logfile = (_transactionId == NULL)? "main.tlo": *_transactionId + ".tlo";
		std::string logFileName = _dataDir + FILESEPARATOR + logfile;
		FileInputOutputStream* fios = new FileInputOutputStream(logFileName, "wb+");
		_control.logFiles.push_back(fios);
		_control.currentFile = fios;
	}
}

TransactionController::TransactionController(const TransactionController& orig) {
	this->_controller = orig._controller;
	this->_transactionId = orig._transactionId;
	_controlFile = orig._controlFile; 
	_control = orig._control;
}

TransactionController::~TransactionController() {
	if (_controlFile) delete _controlFile;
	if (_control.currentFile) delete _control.currentFile;
	if (_transactionId) delete _transactionId;
}

void TransactionController::writeCommandToRegister(char* db, char* ns, Command* cmd) {
	long statusPos = _control.currentFile->currentPos();

	_control.currentFile->writeChar(DIRTY);
	_control.currentFile->writeChars(db, strlen(db));
	_control.currentFile->writeChars(ns, strlen(ns));
	MemoryStream ms;
	CommandWriter writer(&ms);
	writer.writeCommand(cmd);
	// writing the length will allow to jump the command if does not match the db and ns
	_control.currentFile->writeInt(ms.length());
	_control.currentFile->writeChars(ms.toChars(), ms.length());

	long lastValidPos = _control.currentFile->currentPos();
	_control.currentFile->seek(statusPos);
	_control.currentFile->writeChar(NORMAL);

	_controlFile->seek(sizeof(long));
	_controlFile->writeLong(lastValidPos);
	_control.lastValidPos = lastValidPos;
}

Command* TransactionController::readCommandFromRegister(char* db, char* ns) {
	OPERATION_STATUS status = (OPERATION_STATUS)_control.currentFile->readChar();
	char* rdb = _control.currentFile->readChars();
	char* rns = _control.currentFile->readChars();

	Command* result = NULL;
	int length = _control.currentFile->readInt();
	if ((strcmp(rdb, db) == 0) && (strcmp(rns, ns) == 0)) {
		CommandReader reader(_control.currentFile);
		result = reader.readCommand();
	} else {
		_control.currentFile->seek(_control.currentFile->currentPos() + length);
	}
	return result;
}

std::vector<Command*>* TransactionController::findCommands(char* db, char* ns) {
	std::vector<Command*>* result = new std::vector<Command*>();
	for (std::vector<FileInputOutputStream*>::iterator i = _control.logFiles.begin(); i != _control.logFiles.end(); i++) {
		FileInputOutputStream* file = *i;
		_control.currentFile = file;
		while (!_control.currentFile->eof()) {
			Command* cmd = readCommandFromRegister(db, ns);
			if (cmd != NULL) {
				result->push_back(cmd);
			}
		}
	}
	return result;
}

BSONObj* TransactionController::insert(char* db, char* ns, BSONObj* bson) {
	InsertCommand cmd;
	cmd.setDB(std::string(db));
	cmd.setNameSpace(std::string(ns));
	cmd.setBSON(*bson);

	writeCommandToRegister(db, ns, &cmd);

	_controller->insert(db, ns, bson);
}

bool TransactionController::dropNamespace(char* db, char* ns) {
	DropnamespaceCommand cmd;
	cmd.setDB(std::string(db));
	cmd.setNameSpace(std::string(ns));

	writeCommandToRegister(db, ns, &cmd);

	_controller->dropNamespace(db, ns);
}

void TransactionController::update(char* db, char* ns, BSONObj* bson) {
	UpdateCommand cmd;
	cmd.setDB(std::string(db));
	cmd.setNameSpace(std::string(ns));
	cmd.setBSON(*bson);

	writeCommandToRegister(db, ns, &cmd);

	_controller->update(db, ns, bson);
}

void TransactionController::remove(char* db, char* ns, const std::string& documentId, const std::string& revision) {
	RemoveCommand cmd;
	cmd.setDB(std::string(db));
	cmd.setNameSpace(std::string(ns));
	cmd.setId(documentId);
	cmd.setRevision(revision);

	writeCommandToRegister(db, ns, &cmd);

	_controller->remove(db, ns, documentId, revision);
}

std::vector<BSONObj*>* TransactionController::find(char* db, char* ns, const char* select, const char* filter) throw (ParseException) {
	std::vector<Command*> cmds = findCommands(db, ns);
	std::vector<BSONObj*>* result = new std::vector<BSONObj*>();
	for (std::vector<Command*>::iterator i = cmds.begin(); i != cmds.end(); i++) {

	}

	std::vector<BSONObj*> result = _controller->find(db, ns, select, filter);
}

BSONObj* TransactionController::findFirst(char* db, char* ns, const char* select, const char* filter) throw (ParseException) {

}

std::vector<std::string>* TransactionController::dbs() const {

}

std::vector<std::string>* TransactionController::namespaces(const char* db) const {

}

