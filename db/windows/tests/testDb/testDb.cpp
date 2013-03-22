// testDb.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "dbcontroller.h"
#include "bson.h"

void testLockRemove() {
	DBController* dbController = new DBController();
	dbController->initialize();

	BSONObj* obj = BSONParser::parse("{ name: 'Juan'}");
	dbController->insert("testdb", "testns", obj);
	dbController->find("testdb", "testns", "*", "$'name' == 'Juan'");
	dbController->dropNamespace("testdb", "testns");
	dbController->shutdown();

	delete obj;
	delete dbController;
}

int _tmain(int argc, _TCHAR* argv[])
{
	testLockRemove();
	return 0;
}

