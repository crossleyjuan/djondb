/*
 * =====================================================================================
 *
 *       Filename:  stdtransaction.h
 *
 *    Description: This class is an specialization of the base transaction, that allows
 *                 the commit, rollback of a transaction.
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

#ifndef STDTRANSACTION_INCLUDED_H
#define STDTRANSACTION_INCLUDED_H

#include "filterdefs.h"
#include "memorystream.h"
#include "controller.h"
#include "basetransaction.h"
#include "transactiondefs.h"

#include <string>
#include <list>
#include <map>
#include <queue>

class BSONObj;
class FileInputOutputStream;
class FileInputStream;
class FileOutputStream;
class Command;
class TxBufferManager;
class TxBuffer;

class StdTransaction: public BaseTransaction 
{
	public:
		StdTransaction(BaseTransaction* base, std::string transactionId);
		StdTransaction(const StdTransaction& orig);
		~StdTransaction();

		virtual BSONObj* insert(char* db, char* ns, BSONObj* bson);
		virtual bool dropNamespace(char* db, char* ns);
		virtual void update(char* db, char* ns, BSONObj* bson);
		virtual void remove(char* db, char* ns, char* documentId, char* revision);
		virtual BSONArrayObj* find(char* db, char* ns, const char* select, const char* filter) throw (ParseException);
		virtual BSONObj* findFirst(char* db, char* ns, const char* select, const char* filter) throw (ParseException);
		virtual std::vector<std::string>* dbs() const;
		virtual std::vector<std::string>* namespaces(const char* db) const;

		virtual bool commit();
		virtual bool rollback();

	private:
		BaseTransaction* _baseTransaction;
};

#endif // STDTRANSACTION_INCLUDED_H
