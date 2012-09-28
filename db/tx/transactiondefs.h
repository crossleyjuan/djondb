/*
 * =====================================================================================
 *
 *       Filename:  transactiondefs.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  09/27/2012 11:14:26 PM
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
#ifndef TRANSACTIONDEFS_INCLUDED_H
#define TRANSACTIONDEFS_INCLUDED_H 

enum TRANSACTION_ACTION {
	TA_INSERT,
	TA_DELETE,
	TA_UPDATE
};

class Transaction {
	public:
		Transaction(TRANSACTION_ACTION action) {
			_action = action;
		}

		TRANSACTION_ACTION action() const {
			return _action;
		}

	private:
		TRANSACTION_ACTION _action;
};

class InsertTransaction: public Transaction {
	public:
		InsertTransaction(const std::string& db, const std::string& ns, const BSONObj& obj): Transaction(TA_INSERT){
			_db = db;
			_ns = ns;
			_bson = obj;
		}

	private:
		std::string _db;
		std::string _ns;
		std::string _bson;
};

#endif /* TRANSACTIONDEFS_INCLUDED_H */
