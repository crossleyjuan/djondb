/*
 * =====================================================================================
 *
 *       Filename:  basetransaction.cpp
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
#include "stdtransaction.h"
#include "txbuffermanager.h"
#include "util.h"
#include <stdlib.h>

StdTransaction::StdTransaction(BaseTransaction* base, std::string transactionId)
   : BaseTransaction(base, transactionId) {
	_transactionId = new std::string(transactionId);
	_baseTransaction = base;
}

StdTransaction::StdTransaction(const StdTransaction& orig) 
   : BaseTransaction(orig) {
	this->_transactionId = orig._transactionId;
	this->_bufferManager = orig._bufferManager;
}

StdTransaction::~StdTransaction() {
}

const BSONObj* StdTransaction::insert(char* db, char* ns, BSONObj* bson, BSONObj* options) {
	return BaseTransaction::insert(db, ns, bson, options);
}

bool StdTransaction::dropNamespace(char* db, char* ns, BSONObj* options) {
	return BaseTransaction::dropNamespace(db, ns, options);
}

void StdTransaction::update(char* db, char* ns, BSONObj* bson, BSONObj* options) {
	BaseTransaction::update(db, ns, bson, options);
}

void StdTransaction::remove(char* db, char* ns, char* documentId, char* revision, BSONObj* options) {
	BaseTransaction::remove(db, ns, documentId, revision, options);
}

BSONArrayObj* StdTransaction::find(char* db, char* ns, const char* select, const char* filter, BSONObj* options) throw (ParseException) {
	return BaseTransaction::find(db, ns, select, filter, options);
}

BSONObj* StdTransaction::findFirst(char* db, char* ns, const char* select, const char* filter, BSONObj* options) throw (ParseException) {
	return BaseTransaction::findFirst(db, ns, select, filter, options);
}

std::vector<std::string>* StdTransaction::dbs(BSONObj* options) const {
	return BaseTransaction::dbs(options);
}

std::vector<std::string>* StdTransaction::namespaces(const char* db, BSONObj* options) const {
	return BaseTransaction::namespaces(db, options);
}

bool StdTransaction::commit() {
	Logger* log = getLogger(NULL);
	if (log->isDebug()) log->debug("StdTransaction::commit()");

	std::vector<TxBuffer*> buffers = _bufferManager->popAll();
	_baseTransaction->addBuffers(buffers);

	_bufferManager->dropControlFile();

	return true;
}

bool StdTransaction::rollback() {
	_bufferManager->dropAllBuffers();
	_bufferManager->dropControlFile();

	return true;
}
