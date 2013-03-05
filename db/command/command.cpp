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
// charge yourself if you want), bare in mind that you will be required to provide a copy of the license terms that ensures // this program will be open sourced and all its derivated work will be too.
// *********************************************************************************************************************

#include "command.h"
#include "bson.h"
#include "bsonoutputstream.h"
#include "inputstream.h"

Command::Command(COMMANDTYPE commandType)
{
    _commandType = commandType;
	 _options = NULL;
}

Command::Command(const Command& orig) {
    this->_commandType = orig._commandType;
	 if (orig._options != NULL) {
		 this->_options = new BSONObj(*orig._options);
	 } else {
		 this->_options = NULL;
	 }
}

Command::~Command() {
	if (_options) delete _options;
}

COMMANDTYPE Command::commandType() const
{
	return _commandType;
}

void Command::setOptions(const BSONObj* obj) {
	if (obj != NULL) {
		_options = new BSONObj(*obj);
	} else {
		_options = NULL;
	}
}

	CloseCommand::CloseCommand()
: Command(CLOSECONNECTION)
{
}

CloseCommand::CloseCommand(const CloseCommand& orig)
	: Command(CLOSECONNECTION) {
	}

CloseCommand::~CloseCommand() {
}

void CloseCommand::execute() {
}

void CloseCommand::writeCommand(OutputStream* out) const {

}

void CloseCommand::writeResult(OutputStream* out) const {

}

void CloseCommand::readResult(InputStream* is) {

}

