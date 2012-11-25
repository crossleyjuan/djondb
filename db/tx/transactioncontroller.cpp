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
#include "settings.h"
#include "fileinputoutputstream.h"
#include "memorystream.h"
#include "util.h"
#include "commandwriter.h"
#include "insertcommand.h"
#include "dropnamespacecommand.h"
#include "updatecommand.h"
#include "removecommand.h"

TransactionController::TransactionController(DBController* dbcontroller) {
	_dbcontroller = dbcontroller;
	_transactionId = NULL;

	_dataDir = getSetting("DATA_DIR");

	loadControlFile();
}

TransactionController::TransactionController(DBController* dbcontroller, std::string transactionId) {
	_dbcontroller = dbcontroller;
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
	this->_dbcontroller = orig._dbcontroller;
	this->_transactionId = orig._transactionId;
	_controlFile = orig._controlFile; 
	_control = orig._control;
}

TransactionController::~TransactionController() {
	if (_controlFile) delete _controlFile;
	if (_control.currentFile) delete _control.currentFile;
	if (_transactionId) delete _transactionId;
}

void TransactionController::writeRegister(MemoryStream* ms) {
	char* chars = ms->toChars();
	long len = ms->length();
	long statusPos = _control.currentFile->currentPos();

	// _control.currentFile struct
	// struct {
	//       state      char // Record state DIRTY -> NORMAL
	//       content    char* // Record content
	// }	
	_control.currentFile->writeChar(DIRTY);
	_control.currentFile->writeChars(chars, len);
	long lastValidPos = _control.currentFile->currentPos();
	_control.currentFile->seek(statusPos);
	_control.currentFile->writeChar(NORMAL);

	// _controlFile struct
	// struct {
	//       firstValidPos   long // Contains the first record that is flagged as NORMAL
	//       lastValidPos    long // Last position that contains a NORMAL record
	//       active_logs     int  // Number of active logs
	//       records         char** // Records
	// }	
	_controlFile->seek(sizeof(long));
	_controlFile->writeLong(lastValidPos);
	_control.lastValidPos = lastValidPos;
}

BSONObj* TransactionController::insert(char* db, char* ns, BSONObj* bson) {
	InsertCommand* cmd = new InsertCommand();
	cmd->setDB(std::string(db));
	cmd->setNameSpace(std::string(ns));
	cmd->setBSON(*bson);

	MemoryStream* ms = new MemoryStream();
	CommandWriter writer(ms);
	writer.writeCommand(cmd);
	ms->flush();

	writeRegister(ms);

	delete ms;
	delete cmd;
}

bool TransactionController::dropNamespace(char* db, char* ns) {
	DropnamespaceCommand* cmd = new DropnamespaceCommand();
	cmd->setDB(std::string(db));
	cmd->setNameSpace(std::string(ns));

	MemoryStream* ms = new MemoryStream();
	CommandWriter writer(ms);
	writer.writeCommand(cmd);
	ms->flush();

	writeRegister(ms);

	delete ms;
	delete cmd;
}

void TransactionController::update(char* db, char* ns, BSONObj* bson) {
	UpdateCommand* cmd = new UpdateCommand();
	cmd->setDB(std::string(db));
	cmd->setNameSpace(std::string(ns));
	cmd->setBSON(*bson);

	MemoryStream* ms = new MemoryStream();
	CommandWriter writer(ms);
	writer.writeCommand(cmd);
	ms->flush();

	writeRegister(ms);

	delete ms;
	delete cmd;
}

void TransactionController::deleteRecord(char* db, char* ns, const std::string& documentId, const std::string& revision) {
	// Not supported yet
}

std::vector<BSONObj*>* TransactionController::find(char* db, char* ns, const char* select, const char* filter) throw (ParseException) {

}

BSONObj* TransactionController::findFirst(char* db, char* ns, const char* select, const char* filter) throw (ParseException) {

}

std::vector<std::string>* TransactionController::dbs() const {

}

std::vector<std::string>* TransactionController::namespaces(const char* db) const {

}

