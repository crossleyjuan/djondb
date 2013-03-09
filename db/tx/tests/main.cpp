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
#include "command.h"
#include "insertcommand.h"
#include "transactionmanager.h"
#include <stdlib.h>
#include <memory>
#include <string.h>
#include <cpptest.h>

using namespace std;

DBController* _controller;

class TestTXSuite: public Test::Suite
{
	public:
		TestTXSuite()
		{
			TEST_ADD(TestTXSuite::testTransaction);
			TEST_ADD(TestTXSuite::testTransactionCommit);
			TEST_ADD(TestTXSuite::testTransactionManager);
			TEST_ADD(TestTXSuite::testTransactionMergedData);
		}

	private:

		void testTransaction()
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
			delete tx;
			delete controller;
			delete id;
		}

		void testTransactionCommit()
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
			stx->commit();
			delete stx;

			delete tx;
			delete tuid;
		}

		void testTransactionMergedData()
		{
			cout << "\ntestTransactionMergedData()\n" << endl;

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
		}


		void testTransactionManager() {
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
		}

};

enum OutputType
{
	Compiler,
	Html,
	TextTerse,
	TextVerbose
};

	static void
usage()
{
	cout << "\nusage: mytest [MODE]\n"
		<< "where MODE may be one of:\n"
		<< "  --compiler\n"
		<< "  --html\n"
		<< "  --text-terse (default)\n"
		<< "  --text-verbose\n";
	exit(0);
}

	static auto_ptr<Test::Output>
cmdline(int argc, char* argv[])
{
	if (argc > 2)
		usage(); // will not return

	Test::Output* output = 0;

	if (argc == 1)
		output = new Test::TextOutput(Test::TextOutput::Verbose);
	else
	{
		const char* arg = argv[1];
		if (strcmp(arg, "--compiler") == 0)
			output = new Test::CompilerOutput;
		else if (strcmp(arg, "--html") == 0)
			output =  new Test::HtmlOutput;
		else if (strcmp(arg, "--text-terse") == 0)
			output = new Test::TextOutput(Test::TextOutput::Terse);
		else if (strcmp(arg, "--text-verbose") == 0)
			output = new Test::TextOutput(Test::TextOutput::Verbose);
		else
		{
			cout << "\ninvalid commandline argument: " << arg << endl;
			usage(); // will not return
		}
	}

	return auto_ptr<Test::Output>(output);
}

// Main test program
//
int main(int argc, char* argv[])
{
	try
	{
		// Demonstrates the ability to use multiple test suites
		//
		Test::Suite ts;
		ts.add(auto_ptr<Test::Suite>(new TestTXSuite));
		//        ts.add(auto_ptr<Test::Suite>(new CompareTestSuite));
		//        ts.add(auto_ptr<Test::Suite>(new ThrowTestSuite));

		// Run the tests
		//
		_controller = new DBController();
		_controller->initialize();
		auto_ptr<Test::Output> output(cmdline(argc, argv));
		ts.run(*output, true);

		Test::HtmlOutput* const html = dynamic_cast<Test::HtmlOutput*>(output.get());
		if (html)
			html->generate(cout, true, "MyTest");

		_controller->shutdown();
		delete _controller;
	}
	catch (...)
	{
		cout << "\nunexpected exception encountered\n";
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
