/*
 * =====================================================================================
 *
 *       Filename:  testRollback.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/26/2013 01:04:58 PM
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

#include <iostream>
#include "basetransaction.h"
#include "stdtransaction.h"
#include "controllertest.h"
#include "dbcontroller.h"
#include "bson.h"
#include "util.h"
#include "command.h"
#include "insertcommand.h"
#include "transactionmanager.h"
#include <stdlib.h>
#include <memory>
#include <string.h>
#include <cpptest.h>
#include <errno.h>
#include <sstream>
#include <boost/filesystem.hpp>


#define TEST_ASSERT(b) \
	if (!b) { \
		printf("%s\n", "test failed");\
	}

DBController* _controller;

void testTransactionSimplest()
{
	printf("%s\n", "testTransactionSimplest");
	DummyController* controller = new DummyController();

	BaseTransaction* tx = new BaseTransaction(controller);

	tx->dropNamespace("db", "txns");

	delete tx;
	delete controller;

	DummyController* controller2 = new DummyController();

	BaseTransaction* tx2 = new BaseTransaction(controller2);

	tx2->dropNamespace("db", "txns");

	delete tx2;
	delete controller2;
	printf("%s\n", "~testTransactionSimplest");
}

void testTransaction()
{
	printf("%s\n", "testTransaction");
	DummyController* controller = new DummyController();

	BaseTransaction* tx = new BaseTransaction(controller);

	tx->dropNamespace("db", "txns");

	BSONObj o;
	std::string* id = uuid();
	o.add("_id", const_cast<char*>(id->c_str()));
	o.add("name", "John");
	tx->insert("db", "txns", &o);

	BSONArrayObj* res = tx->find("db", "txns", "*", "");

	TEST_ASSERT(res->length() == 1);
	BSONObj* test1 = *res->begin();
	TEST_ASSERT(test1->getString("name").compare("John") == 0);
	test1->add("name", "Peter");
	tx->update("db", "txns", test1);

	delete res;

	res = tx->find("db", "txns", "*", "");

	TEST_ASSERT(res->length() == 1);
	BSONObj* test2 = *res->begin();
	TEST_ASSERT(test2->getString("name").compare("Peter") == 0);

	delete res;
	printf("%s\n", "Deleting tx");
	delete tx;
	delete controller;
	delete id;
	printf("%s\n", "~testTransaction");
}

void testTransactionSimpleCommit()
{
	printf("%s\n", "testTransactionSimpleCommit");

	BaseTransaction* tx = new BaseTransaction(_controller);
	std::string* tuid = uuid();
	StdTransaction* stx = new StdTransaction(tx, *tuid);

	BSONObj* obj = BSONParser::parse("{name: 'test', lastName: 'testln'}");
	stx->insert("db", "testcommit", obj);

	printf("%s\n", "Doing commit");
	stx->commit();
	printf("%s\n", "deleting stx");
	delete stx;

	printf("%s\n", "deleting tx");
	delete tx;
	delete obj;
	delete tuid;
	printf("%s\n", "~testTransactionSimpleCommit");
}

void testTransactionRollback()
{
	printf("%s\n", "testTransactionRollback");

	BaseTransaction* tx = new BaseTransaction(_controller);
	std::string* tuid = uuid();
	StdTransaction* stx = new StdTransaction(tx, *tuid);

	BSONObj* obj = BSONParser::parse("{'_id': '1'}");
	stx->insert("db", "testrollback", obj);

	printf("%s\n", "Doing rollback");
	stx->rollback();

	delete stx;

	delete tx;
	delete obj;
	printf("~testTransactionRollback");
}

void testTransactionCommit()
{
	printf("%s\n", "testTransactionCommit");

	BaseTransaction* tx = new BaseTransaction(_controller);
	std::string* tuid = uuid();
	StdTransaction* stx = new StdTransaction(tx, *tuid);

	stx->dropNamespace("db", "testcommit");

	std::vector<std::string> idsCheck;
	for (int y = 0; y < 300; y++) {
		BSONObj o;
		std::string* id = uuid();
		o.add("_id", const_cast<char*>(id->c_str()));
		o.add("name", "John");
		stx->insert("db", "testcommit", &o);
		if ((y % 10) == 0) {
			idsCheck.push_back(*id);
		}
		delete id;
	}

	for (std::vector<std::string>::iterator i = idsCheck.begin(); i != idsCheck.end(); i++) {
		std::string id = *i;
		std::string filter = format("$'_id' == '%s'", id.c_str());
		BSONObj* res = stx->findFirst("db", "testcommit", "*", filter.c_str());
		delete res;
	}
	printf("%s\n", "Doing commit");
	stx->commit();
	delete stx;

	delete tx;
	printf("%s\n", "~testTransactionCommit");
}

void testTransactionMergedData()
{
	printf("%s\n", "testTransactionMergedData");
	BaseTransaction* tx = new BaseTransaction(_controller);
	tx->dropNamespace("db", "testcommit");

	BSONObj ooutTX;
	std::string* idOut = uuid();
	ooutTX.add("_id", const_cast<char*>(idOut->c_str()));
	ooutTX.add("name", "JohnOut");
	tx->insert("db", "testcommit", &ooutTX);

	// Insert out of the transaction
	std::string* tuid = uuid();
	StdTransaction* stx = new StdTransaction(tx, *tuid);

	BSONObj o;
	std::string* id = uuid();
	o.add("_id", const_cast<char*>(id->c_str()));
	o.add("name", "John");
	stx->insert("db", "testcommit", &o);

	BSONArrayObj* res = stx->find("db", "testcommit", "*", "");
	TEST_ASSERT(res->length() == 2);

	BSONArrayObj* resOut = tx->find("db", "testcommit", "*", "");
	TEST_ASSERT(resOut->length() == 1);

	stx->commit();
	delete stx;

	BSONArrayObj* resOut2 = tx->find("db", "testcommit", "*", "");
	TEST_ASSERT(resOut2->length() == 2);

	delete tx;
	delete res;
	delete resOut;
	delete resOut2;
	delete tuid;
	printf("%s\n", "~testTransactionMergedData");
}


void testTransactionManager() {
	printf("%s\n", "testTransactionManager");

	// Insert a document in wal
	BaseTransaction* wal = new BaseTransaction(_controller);
	wal->dropNamespace("db", "mtx");

	TransactionManager* manager = new TransactionManager(wal);

	BSONObj testA;
	testA.add("cod", 1);
	testA.add("name", "William");
	wal->insert("db", "mtx", &testA);

	std::string* t1 = uuid();

	StdTransaction* transaction = manager->getTransaction(*t1);
	std::auto_ptr<BSONArrayObj> array(transaction->find("db", "mtx", "*", "$'cod' == 1"));
	BSONObj* obj1up = array->get(0);
	obj1up->add("lastName", "Shakespeare");
	transaction->update("db", "mtx", obj1up);

	std::auto_ptr<BSONArrayObj> array0(wal->find("db", "mtx", "*", "$'cod' == 1"));
	BSONObj* origin1 = array0->get(0); 
	TEST_ASSERT(!origin1->has("lastName"));

	std::auto_ptr<BSONArrayObj> array1(transaction->find("db", "mtx", "*", "$'cod' == 1"));
	BSONObj* objtx1 = array1->get(0);
	TEST_ASSERT(objtx1->has("lastName"));

	transaction->commit();
	manager->dropTransaction(*t1);

	std::auto_ptr<BSONArrayObj> array2(wal->find("db", "mtx", "*", "$'cod' == 1"));
	BSONObj* origin2 = array2->get(0);
	TEST_ASSERT(origin2->has("lastName"));

	delete t1;
	delete manager;
	printf("%s\n", "~testTransactionManager");
}

int main(int argc, char** arg) {
	_controller = new DBController();
	char currentDir[256];
	memset(currentDir, 0, 256);
	getcwd(currentDir, 256);
	printf("%s\n", currentDir);
	char* datadir = combinePath(currentDir, "datadir/");
	boost::filesystem::remove_all(datadir);
	makeDir(datadir);
	setSetting("DATA_DIR", std::string(datadir));
	_controller->initialize(datadir);

	/*
	testTransactionSimplest();
	testTransaction();
	testTransactionSimpleCommit();
	testTransactionCommit();
	testTransactionRollback();
	testTransactionManager();
	testTransactionMergedData();
	*/
	_controller->shutdown();
	free(datadir);
	return 0;
}
