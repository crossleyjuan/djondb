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

#include <iostream>
#include "basetransaction.h"
#include "stdtransaction.h"
#include "controllertest.h"
#include "dbcontroller.h"
#include "bson.h"
#include "util.h"
#include "transactionmanager.h"
#include <stdlib.h>
#include <memory>
#include <string.h>
#include <gtest/gtest.h>
#include <errno.h>
#include <sstream>

using namespace std;


class TestTX: public testing::Test
{
	protected:
		DBController* _controller;

		virtual void SetUp()
		{
			// Demonstrates the ability to use multiple test suites
			//
			std::string sdataDir = getSetting("DATA_DIR");
			char* dataDir = combinePath(sdataDir.c_str(), "*");
			std::stringstream ss;
			ss << "exec rm -rf " << dataDir;

			if (system(ss.str().c_str()) < 0) {
				cout << "the data dir " << dataDir << " could not be removed. error: " << strerror(errno) << endl;
				exit(1);
			}
			free(dataDir);
			_controller = new DBController();
			_controller->initialize();
		}

		virtual void SetDown() {
			_controller->shutdown();
			delete _controller;
		}

};

TEST_F(TestTX, testSimpleOperations) {
	Logger* log = getLogger(NULL);
	log->info("testSimpleOperations");

	BaseTransaction* tx = new BaseTransaction(_controller);
	tx->dropNamespace("test", "simple-tx");
	BSONObj test;
	std::string* id = uuid();
	test.add("_id", id->c_str());
	test.add("a", 1);
	tx->insert("test", "simple-tx", &test);
	BSONArrayObj* arr = tx->find("test", "simple-tx", "*", "");
	BSONObj* obj = arr->get(0);
	obj->add("a", 2);
	tx->update("test", "simple-tx", obj);
	delete id;
	delete arr;

	arr = tx->find("test", "simple-tx", "*", "$'a' == 1");
	EXPECT_TRUE(arr->length() == 0);

	log->info("~testSimpleOperations");
	delete arr;
}

TEST_F(TestTX, testTransactionSimplest)
{
	Logger* log = getLogger(NULL);
	log->info("testTransactionSimplest");
	DummyController* controller = new DummyController();

	BaseTransaction* tx = new BaseTransaction(controller);

	tx->dropNamespace("db", "txns");

	delete tx;
	delete controller;

	controller = new DummyController();

	tx = new BaseTransaction(controller);

	tx->dropNamespace("db", "txns");

	delete tx;
	delete controller;
}

TEST_F(TestTX, testTransaction)
{
	Logger* log = getLogger(NULL);
	log->info("testTransaction");
	DummyController* controller = new DummyController();

	BaseTransaction* tx = new BaseTransaction(controller);

	tx->dropNamespace("db", "txns");

	BSONObj o;
	std::string* id = uuid();
	o.add("_id", const_cast<char*>(id->c_str()));
	o.add("name", "John");
	tx->insert("db", "txns", &o);

	BSONArrayObj* res = tx->find("db", "txns", "*", "");

	EXPECT_TRUE(res->length() == 1);
	BSONObj* test1 = *res->begin();
	EXPECT_TRUE(test1->getString("name").compare("John") == 0);
	test1->add("name", "Peter");
	tx->update("db", "txns", test1);

	delete res;

	res = tx->find("db", "txns", "*", "");

	EXPECT_TRUE(res->length() == 1);
	BSONObj* test2 = *res->begin();
	EXPECT_TRUE(test2->getString("name").compare("Peter") == 0);

	delete res;
	delete tx;
	delete controller;
	delete id;
}

TEST_F(TestTX, testTransactionSimpleCommit)
{
	Logger* log = getLogger(NULL);
	log->info("testTransactionSimpleCommit");

	BaseTransaction* tx = new BaseTransaction(_controller);
	std::string* tuid = uuid();
	StdTransaction* stx = new StdTransaction(tx, *tuid);

	BSONObj* obj = BSONParser::parse("{name: 'test', lastName: 'testln'}");
	stx->insert("db", "testcommit", obj);

	log->info("Doing commit");
	stx->commit();
	delete stx;

	delete tx;
	delete obj;
	log->info("~testTransactionSimpleCommit");
}

TEST_F(TestTX, testTransactionRollback)
{
	Logger* log = getLogger(NULL);
	log->info("testTransactionRollback");

	BaseTransaction* tx = new BaseTransaction(_controller);
	std::string* tuid = uuid();
	StdTransaction* stx = new StdTransaction(tx, *tuid);

	BSONObj* obj = BSONParser::parse("{'_id': '1'}");
	stx->insert("db", "testrollback", obj);

	log->info("Doing rollback");
	stx->rollback();

	delete stx;

	delete tx;
	delete obj;
	log->info("~testTransactionRollback");
}

TEST_F(TestTX, testTransactionCommit)
{
	Logger* log = getLogger(NULL);
	log->info("testTransactionCommit");

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
	log->info("Doing commit");
	stx->commit();
	delete stx;

	delete tx;
	log->info("~testTransactionCommit");
}

TEST_F(TestTX, testTransactionMergedData)
{
	Logger* log = getLogger(NULL);
	log->info("testTransactionMergedData");

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
	EXPECT_TRUE(res->length() == 2);

	BSONArrayObj* resOut = tx->find("db", "testcommit", "*", "");
	EXPECT_TRUE(resOut->length() == 1);

	stx->commit();
	delete stx;

	BSONArrayObj* resOut2 = tx->find("db", "testcommit", "*", "");
	EXPECT_TRUE(resOut2->length() == 2);

	delete tx;
	delete res;
	delete resOut;
	delete resOut2;
	delete tuid;
	log->info("~testTransactionMergedData");
}


TEST_F(TestTX, testTransactionManager) {
	Logger* log = getLogger(NULL);
	log->info("testTransactionManager");

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
	EXPECT_TRUE(!origin1->has("lastName"));

	std::auto_ptr<BSONArrayObj> array1(transaction->find("db", "mtx", "*", "$'cod' == 1"));
	BSONObj* objtx1 = array1->get(0);
	EXPECT_TRUE(objtx1->has("lastName"));

	transaction->commit();
	manager->dropTransaction(*t1);

	std::auto_ptr<BSONArrayObj> array2(wal->find("db", "mtx", "*", "$'cod' == 1"));
	BSONObj* origin2 = array2->get(0);
	EXPECT_TRUE(origin2->has("lastName"));

	delete t1;
	delete manager;
	log->info("~testTransactionManager");
}
