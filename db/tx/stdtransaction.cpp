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

BSONObj* StdTransaction::insert(char* db, char* ns, BSONObj* bson) {
	return BaseTransaction::insert(db, ns, bson);
}

bool StdTransaction::dropNamespace(char* db, char* ns) {
	return BaseTransaction::dropNamespace(db, ns);
}

void StdTransaction::update(char* db, char* ns, BSONObj* bson) {
	BaseTransaction::update(db, ns, bson);
}

void StdTransaction::remove(char* db, char* ns, char* documentId, char* revision) {
	BaseTransaction::remove(db, ns, documentId, revision);
}

BSONArrayObj* StdTransaction::find(char* db, char* ns, const char* select, const char* filter) throw (ParseException) {
	return BaseTransaction::find(db, ns, select, filter);
}

BSONObj* StdTransaction::findFirst(char* db, char* ns, const char* select, const char* filter) throw (ParseException) {
	return BaseTransaction::findFirst(db, ns, select, filter);
}

std::vector<std::string>* StdTransaction::dbs() const {
	return BaseTransaction::dbs();
}

std::vector<std::string>* StdTransaction::namespaces(const char* db) const {
	return BaseTransaction::namespaces(db);
}

bool StdTransaction::commit() {
	std::vector<TxBuffer*> buffers = _bufferManager->popAll();
	_baseTransaction->addBuffers(buffers);
}

bool StdTransaction::rollback() {
}
