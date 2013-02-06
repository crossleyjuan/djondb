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

class BSONObj;

enum OPERATION_STATUS {
	TXOS_UNDEFINED = 0,  // Not selected yet

	// RECORD STATUS
	TXOS_DIRTY = 1, // in writing process, it's not trustable yet
	TXOS_NORMAL = 2, // the record it's ok and it's trustable
	TXOS_REMOVED = 4,  // rollbacked or any other state, should not be used
	TXOS_DONE = 8, // Moved to the main data repository, ready to be dropped.

	/// SYNC ELEMENTS
	TXOS_NOT_SYNC = 16, // Not sync yet
	TXOS_SYNC = 32, // This record is in sync with other nodes
	TXOS_RESERVED_SYNC1 = 64, // Flag reserved for other sync status
	TXOS_RESERVED_SYNC2 = 128, // Flag reserved for other sync status
};

enum TRANSACTION_OPER {
	TXO_UNDEFINED,
	TXO_DROPNAMESPACE,
	TXO_INSERT,
	TXO_UPDATE,
	TXO_REMOVE
};

struct BsonOper {
	BSONObj* bson;
};

struct RemoveOper {
	std::string key;
	std::string revision;
};

struct TransactionOperation {
	TRANSACTION_OPER code;
	OPERATION_STATUS status;
	std::string* db;
	std::string* ns;
	void* operation; // Oper Structs
};

struct TransactionBuffer {
	__int64 startPos;
	__int64 lastValidPos;
	__int64 length;
};

#endif /* TRANSACTIONDEFS_INCLUDED_H */
