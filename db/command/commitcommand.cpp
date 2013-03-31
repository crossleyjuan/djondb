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

#include "commitcommand.h"

#include "inputstream.h"
#include "outputstream.h"
#include "dbcontroller.h"
#include "transactionmanager.h"
#include "stdtransaction.h"

CommitCommand::CommitCommand()
    : Command(COMMIT)
{
}

CommitCommand::CommitCommand(const CommitCommand& orig)
: Command(COMMIT) {
}

CommitCommand::~CommitCommand() {
    delete(_transactionId);
}

void CommitCommand::execute() {
	TransactionManager* manager = TransactionManager::getTransactionManager();
	StdTransaction* tx = manager->getTransaction(*_transactionId);
	_result = tx->commit();
	manager->dropTransaction(*_transactionId);
}

void* CommitCommand::result() {
    return &_result;
}

void CommitCommand::writeCommand(OutputStream* out) const {
	out->writeString(*_transactionId);
}

void CommitCommand::readResult(InputStream* is) {
}

void CommitCommand::writeResult(OutputStream* out) const {
}

void CommitCommand::setTransactionId(const std::string& txId) {
	_transactionId = new std::string(txId);
}

const std::string* CommitCommand::transactionId() const {
	return _transactionId;
}

