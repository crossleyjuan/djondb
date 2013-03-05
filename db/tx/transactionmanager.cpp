/*
 * =====================================================================================
 *
 *       Filename:  transactionmanager.cpp
 *
 *    Description:  This file implements the transaction manager.
 *
 *        Version:  1.0
 *        Created:  02/28/2013 06:20:08 PM
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
#include "transactionmanager.h"

#include "basetransaction.h"
#include "stdtransaction.h"
#include "util.h"
#include "cache.h"
#include <map>

TransactionManager* TransactionManager::_instance;

TransactionManager::TransactionManager(BaseTransaction* wal) {
	_wal = wal;
	_cacheTransactions = new Cache<std::string, StdTransaction*>();
}

TransactionManager::~TransactionManager() {
	for (Cache<std::string, StdTransaction*>::iterator i = _cacheTransactions->begin(); i != _cacheTransactions->end(); i++) {
		StdTransaction* trans = i->second;
		delete trans;
	}
	_cacheTransactions->clear();
	delete _cacheTransactions;
}

StdTransaction* TransactionManager::getTransaction(std::string id) {
	if (_cacheTransactions->containsKey(id)) {
		return _cacheTransactions->get(id);
	} else {
		StdTransaction* transaction = new StdTransaction(_wal, id);
		_cacheTransactions->add(id, transaction);
		return transaction;
	}
}

void TransactionManager::dropTransaction(const std::string& id) {
	if (_cacheTransactions->containsKey(id)) {
		StdTransaction* trans = _cacheTransactions->get(id);
		_cacheTransactions->remove(id);
		delete trans;
	}
}
		
void TransactionManager::initializeTransactionManager(BaseTransaction* wal) {
	_instance = new TransactionManager(wal);
}

TransactionManager* TransactionManager::getTransactionManager() {
	return _instance;
}
