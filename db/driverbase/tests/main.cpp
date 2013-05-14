// *********************************************************************************************************************
// file:
// author: Juan Pablo Crossley (crossleyjuan@gmail.com) // created:
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
#include "djondb_client.h"

#include "fileoutputstream.h"
#include "fileinputstream.h"
#include "config.h"
#include "util.h"
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <gtest/gtest.h> 
#include <limits.h> 
#include <memory>

using namespace std; 
using namespace djondb;

bool __running;
char* _host = "localhost";
int _port = 1243;

/*
TEST(TestDriver, testConnection) {
	cout << "\ntestConnection\n" << endl;

	DjondbConnection* conn = DjondbConnectionManager::getConnection(_host);

	EXPECT_THROW(conn->insert("db1", "ns1", "{ name: 'Test'  }"), DjondbException) << "If the client is not connected then an exception should be thrown";

	// test connection and then shutdown the server
	if (conn->open()) {
		conn->shutdown();
		EXPECT_THROW(conn->insert("db1", "ns1", "{ name: 'Test'  }"), DjondbException); 
	} else {
		FAIL() << "Cannot establish a connection with the server";
	}
}
*/

TEST(TestDriver, testDbsNamespaces) {
	DjondbConnection* conn = DjondbConnectionManager::getConnection(_host);

	if (!conn->open()) {
		cout << "\nCannot connect to localhost" << endl;
		exit(0);
	}

	std::string bson = "{ name: 'Test'}";
	conn->insert("db1", "ns1", bson);
	conn->insert("db2", "ns1", bson);
	conn->insert("db3", "ns1", bson);

	std::vector<std::string>* dbs = conn->dbs();

	EXPECT_TRUE(dbs->size() >= 3);

	delete dbs;
}

TEST(TestDriver, testDropNamespace) {
	DjondbConnection* conn = DjondbConnectionManager::getConnection(_host);

	if (!conn->open()) {
		cout << "\nCannot connect to localhost" << endl;
		exit(0);
	}

	conn->insert("db", "testdrop.namespace", "{ name: 'Test' }");

	bool result = conn->dropNamespace("db", "testdrop.namespace");

	EXPECT_TRUE(result);

	BSONArrayObj* testresult = conn->find("db", "testdrop.namespace", "*", std::string(""));

	EXPECT_TRUE(testresult->length() == 0);
	delete testresult;
}

TEST(TestDriver, testInsertComplex) {
	cout << "\nTesting complex" << endl;
	DjondbConnection* conn = DjondbConnectionManager::getConnection(_host);

	if (!conn->open()) {
		cout << "\nCould not connect to " << _host << endl;
		exit(0);
	}

	conn->dropNamespace("test", "ns");

	BSONObj obj;
	std::string* id = uuid();
	obj.add("_id", const_cast<char*>(id->c_str()));
	obj.add("name", "John");
	BSONObj inner;
	inner.add("innername", "Test");
	obj.add("inner", inner);

	conn->insert("test", "ns", obj);

	BSONArrayObj* res = conn->find("test", "ns", "*", "$'_id' == '" + *id + "'");
	EXPECT_TRUE(res->length() == 1);
	BSONObj* bres = *res->begin();
	EXPECT_TRUE(bres->has("inner"));
	BSONObj* innerres = bres->getBSON("inner");
	EXPECT_TRUE(innerres != NULL);
	EXPECT_TRUE(innerres->has("innername"));
	EXPECT_TRUE(((std::string)"Test").compare(innerres->getString("innername")) == 0);

	// testing arrays
	cout << "testInsertComplex: Testing arrays" << endl;
	std::string* id2 = uuid();
	conn->insert("test", "ns", "{ '_id': '" + *id2 + "', 'array': [ { 'x': 'test', 'y': 3},  { 'x': 'test2', 'y': 4}]  }");

	BSONArrayObj* res2 = conn->find("test", "ns", "*", "$'_id' == '" + *id2 + "'");
	EXPECT_TRUE(res2->length() == 1);
	BSONObj* o2 = *res2->begin();
	EXPECT_TRUE(o2 != NULL);

	EXPECT_TRUE(o2->has("array"));

	delete res;
	delete res2;

	// testing a customer
	conn->dropNamespace("db", "testcustomer");
	BSONObj* customer = BSONParser::parse("{ 'name': 'Martin', 'lastName': 'Scor', 'finantial': { 'salary': 150000, 'rent': 10000} }");
	conn->insert("db", "testcustomer", *customer);
	delete customer;

	res2 = conn->find("db", "testcustomer", "*", "$'name' == 'Martin'");
	EXPECT_TRUE(res2->length() == 1);
	if (res2->length() == 1) {
		BSONObj* objCustomer = *res2->begin();
		int d = *objCustomer->getXpath("finantial.salary");
		EXPECT_TRUE(d == 150000);
	}
	delete res2;
}

TEST(TestDriver, testInsert) {
	int inserts = 1;

	Logger* log = getLogger(NULL);

	cout << "\nStarting " << endl;

	log->startTimeRecord();
	__running = true;

	DjondbConnection* conn = DjondbConnectionManager::getConnection(std::string(_host));

	if (!conn->open()) {
		cout << "\nCould not connect to " << _host << endl;
		exit(0);
	}
	std::vector<std::string> ids;
	for (int x = 0; x < inserts; x++) {

		BSONObj obj;
		std::auto_ptr<std::string> guid(uuid());
		obj.add("_id", const_cast<char*>(guid->c_str()));
		int test = rand() % 10;
		if (test > 0) {
			ids.push_back(*guid.get());
		}
		//        obj->add("name", "John");
		char* temp = (char*)malloc(2000);
		memset(temp, 0, 2000);
		memset(temp, 'a', 1999);
		int len = strlen(temp);
		obj.add("content", temp);
		free(temp);

		conn->insert("db", "driverbase.test", obj);

		if ((inserts > 9) && (x % (inserts / 10)) == 0) {
			cout << "\n" << x << " Records sent" << endl;
		}
	}
	FileOutputStream* fosIds = new FileOutputStream("results.txt", "wb");
	fosIds->writeInt(ids.size());
	for (std::vector<std::string>::iterator i2 = ids.begin(); i2!= ids.end(); i2++) {
		std::string s = *i2;
		fosIds->writeString(s);
	}
	fosIds->close();
	cout << "\nall sent" << endl;

	log->stopTimeRecord();

	DTime rec = log->recordedTime();

	int secs = rec.totalSecs();
	cout << "\ninserts " << inserts << ", time: " << rec.toChar() << endl;

	if (secs > 0) {
		cout << "\nThroughput: " << (inserts / secs) << " ops." << endl;
	}
	cout << "\n------------------------------------------------------------" << endl;
	cout << "\nReady to close the connection" << endl;
	//getchar();
	__running = false;

	//    cout << "Closing the connection" << endl;
	//    conn->close();
	//
	//    delete conn;
}

TEST(TestDriver, testFinds) {
	int maxfinds = 1;
	Logger* log = getLogger(NULL);

	cout << "\nStarting testFinds" << endl;

	__running = true;
	DjondbConnection* conn = DjondbConnectionManager::getConnection(_host);

	if (!conn->open()) {
		cout << "\nCannot connect to localhost" << endl;
		exit(0);
	}

	BSONObj test;
	std::string* guid = uuid();
	test.add("_id", const_cast<char*>(guid->c_str()));
	test.add("int", 1);
	test.add("long", (__int64) 10L);
	test.add("longmax",(__int64) LONG_MAX);
	test.add("char", "testing");

	conn->insert("db", "driver.test", test);

	log->debug("Data inserted");

	BSONObj* objResult = conn->findByKey("db", "driver.test", "*", *guid);

	EXPECT_TRUE(objResult != NULL);
	EXPECT_TRUE(objResult->has("int"));
	EXPECT_TRUE(objResult->getInt("int") == 1);
	EXPECT_TRUE(objResult->has("long"));
	EXPECT_TRUE(objResult->getLong("long") == 10);
	EXPECT_TRUE(objResult->has("longmax"));
	EXPECT_TRUE(objResult->getLong("longmax") == LONG_MAX);
	EXPECT_TRUE(objResult->has("char"));
	EXPECT_TRUE(objResult->getString("char").compare("testing") == 0);

	__running = false;

}

TEST(TestDriver, testFindByFilter) {
	// Insert record to search for

	BSONObj* obj = BSONParser::parse("{'name': 'Test', 'inner': { 'x': 1 }}");

	//delete id;

	DjondbConnection* conn = DjondbConnectionManager::getConnection(_host);

	if (!conn->open()) {
		cout << "\nCannot connect to localhost" << endl;
		exit(0);
	}


	// Drops the current namespace to start from scratch
	conn->dropNamespace("db", "test.filter2");

	conn->insert("db", "test.filter2", *obj);

	// doing search
	//

	cout << "\nTestbyfilter" << endl;
	std::string filter = "";
	BSONArrayObj* result = conn->find("db", "test.filter2", "*", filter);			
	EXPECT_TRUE(result->length() > 0);
	filter = "$'name' == 'Test'";
	delete result;
	result = conn->find("db", "test.filter2", filter);			
	EXPECT_TRUE(result->length() > 0);

	BSONObj* objR = *result->begin();
	EXPECT_TRUE(objR != NULL);
	EXPECT_TRUE(objR->has("name"));
	EXPECT_TRUE(objR->getString("name").compare("Test") == 0);

	char* temp = objR->toChar();
	cout << "\nobj: " << temp << endl;

	result = conn->find("db", "test.filter2", "*", "$'name' == 'Test'");
	EXPECT_TRUE(result->length() == 1);

	result = conn->find("db", "test.filter2", "$'inner.x' == 1");
	EXPECT_TRUE(result->length() == 1);

	result = conn->find("db", "test.filter2", "*", "$'inner.x' > 0");
	EXPECT_TRUE(result->length() == 1);

	result = conn->find("db", "test.filter2", "$'inner.x' > 1");
	EXPECT_TRUE(result->length() == 0);

	delete objR;
	delete result;

}

TEST(TestDriver, testUpdate) {
	int maxupdates = 1;
	Logger* log = getLogger(NULL);

	cout << "\nStarting " << endl;

	log->startTimeRecord();
	__running = true;

	DjondbConnection* conn = DjondbConnectionManager::getConnection(_host);

	if (!conn->open()) {
		cout << "\nCannot connect to localhost" << endl;
		exit(0);
	}
	FileInputStream* fisIds = new FileInputStream("results.txt", "rb");
	int x = 0;
	int count = fisIds->readInt();
	if ((maxupdates > -1) && (count > maxupdates)) {
		count = maxupdates;
	}
	cout << "\nRecords to update: " << count << endl;

	std::vector<std::string> idsUpdated;
	for (x =0; x < count; x++) {
		std::auto_ptr<std::string> guid(fisIds->readString());

		BSONObj* obj = conn->findByKey("db", "driverbase.test", guid->c_str());

		idsUpdated.push_back(*guid.get());
		char* temp = (char*)malloc(100);
		memset(temp, 0, 100);
		memset(temp, 'b', 99);
		int len = strlen(temp);
		obj->add("content", temp);
		free(temp);

		conn->update("db", "driverbase.test", *obj);

		if ((count > 9) && (x % (count / 10)) == 0) {
			cout << "\n" << x << " Records received" << endl;
		}
		delete obj;
	}

	log->stopTimeRecord();

	cout << "\nExecuting a verification" << endl;

	for (std::vector<std::string>::iterator i = idsUpdated.begin(); i != idsUpdated.end(); i++) {
		std::string guid = *i;

		std::auto_ptr<BSONObj> resObj(conn->findByKey("db", "driverbase.test", "*", guid));

		EXPECT_TRUE(resObj.get() != NULL);
		EXPECT_TRUE(resObj->has("_id"));
		EXPECT_TRUE(resObj->has("content"));

		char* temp = (char*)malloc(100);
		memset(temp, 0, 100);
		memset(temp, 'b', 99);
		EXPECT_TRUE(resObj->getString("content").compare(temp) == 0);
		free(temp);
	}
	DTime rec = log->recordedTime();

	int secs = rec.totalSecs();
	cout<< "finds " << count << ", time: " << rec.toChar() << endl;

	if (secs > 0) {
		cout << "\nThroughput: " << (count / secs) << " ops." << endl;
	}

	cout << "\n------------------------------------------------------------" << endl;
	cout << "\nReady to close the connection" << endl;
	//getchar();
	__running = false;

	//    cout << "Closing the connection" << endl;
	//    conn->close();
	//
	//    delete conn;
}

TEST(TestDriver, testUpdateValidations) {
	cout << "\ntestUpdateValidations\n" << endl;

	DjondbConnection* conn = DjondbConnectionManager::getConnection(_host);

	if (!conn->open()) {
		cout << "\nCannot connect to localhost" << endl;
		exit(0);
	}

	BSONObj obj;
	obj.add("test", "test");
	EXPECT_THROW(conn->update("mydb", "updatens", obj), DjondbException) << "The update should validate that an _id and _revision are required fields";
}

TEST(TestDriver, testRemove) {
	cout << "\ntestRemove\n" << endl;

	DjondbConnection* conn = DjondbConnectionManager::getConnection(_host);

	if (!conn->open()) {
		cout << "\nCannot connect to localhost" << endl;
		exit(0);
	}

	conn->dropNamespace("testdb", "deletens");

	BSONObj obj;
	std::string* id = uuid();
	std::string* revision = uuid();
	obj.add("_id", const_cast<char*>(id->c_str())); 
	obj.add("_revision", const_cast<char*>(revision->c_str())); 

	conn->insert("testdb", "deletens", obj);

	conn->remove("testdb", "deletens", *id, *revision);

	BSONObj* res = conn->findByKey("testdb", "deletend", *id);
	EXPECT_TRUE(res == NULL);

	DjondbConnectionManager::releaseConnection(conn);

	delete id;
	delete revision;
}


TEST(TestDriver, testTransactions) {
	cout << "\ntestTransactions()\n" << endl;

	DjondbConnection* connection = DjondbConnectionManager::getConnection(_host);

	if (connection->open()) {
		// Out of the transaction
		//
		connection->dropNamespace("testdb", "tx");
		BSONObj ontx;
		std::auto_ptr<std::string> idx(uuid());
		ontx.add("_id", const_cast<char*>(idx->c_str()));
		ontx.add("name", "TestOutTx");
		connection->insert("testdb", "tx", ontx);

		// Now the transaction is started
		const char* trans = connection->beginTransaction();

		BSONObj o;
		std::auto_ptr<std::string> id(uuid());
		o.add("_id", const_cast<char*>(id->c_str()));
		o.add("name", "John");
		o.add("lastName", "Crossley");

		connection->insert("testdb", "tx", o);

		BSONObj* res = connection->findByKey("testdb", "tx", id->c_str());

		EXPECT_TRUE(res != NULL);
		if (res != NULL) {
			EXPECT_TRUE(res->has("name"));
			if (res->has("name")) EXPECT_TRUE(res->getString("name").compare("John") == 0);
		}

		// Test records out of the transaction
		BSONArrayObj* array1 = connection->find("testdb", "tx", "*", "");
		EXPECT_TRUE(array1->length() == 2);

		connection->commitTransaction();

		BSONObj* res2 = connection->findByKey("testdb", "tx", id->c_str());

		EXPECT_TRUE(res2 != NULL);
		if (res2 != NULL) {
			EXPECT_TRUE(res2->has("name"));
			if (res2->has("name")) EXPECT_TRUE(res2->getString("name").compare("John") == 0);
			delete res2;
		}

		connection->dropNamespace("testdb", "tx");
		connection->beginTransaction();
		BSONObj* objRollback = BSONParser::parse("{'_id': '001'}");

		connection->insert("testdb", "tx", *objRollback);

		res2 = connection->findByKey("testdb", "tx", "001");
		EXPECT_TRUE(res2 != NULL);
		if (res2 != NULL) {
			EXPECT_TRUE(res2->getString("_id").compare("001") == 0);
			delete res2;
		}

		connection->rollbackTransaction();

		res2 = connection->findByKey("testdb", "tx", "001");
		EXPECT_TRUE(res2 == NULL);

		delete objRollback;
	}
}

TEST(TestDriver, testDQL) {

	cout << "\ntestDQL\n" << endl;

	DjondbConnection* connection = DjondbConnectionManager::getConnection(_host);

	if (connection->open()) {
		connection->dropNamespace("db", "TestQL");

		char* ql = "Insert { 'name': 'John', 'last': 'Smith'} INTO db:TestQL";
		connection->executeUpdate(ql);

		char* qlFind = "Select * FROM db:TestQL";
		BSONArrayObj* result = connection->executeQuery(qlFind);

		qlFind = "Select $'a', $'b' FROM db:TestQL";
		result = connection->executeQuery(qlFind);

	   ASSERT_TRUE(result->length() == 1);
	}

}
