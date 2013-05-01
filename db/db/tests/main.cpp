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
#include <dbcontroller.h>
#include <util.h>
#include <defs.h>
#include <ctime>
#ifndef WINDOWS
#include <time.h>
#endif
#ifdef WINDOWS
#include <Windows.h>
#endif
#include "bson.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "fileoutputstream.h"
#include "fileinputstream.h"
//#include "bplusindexp.h"
#include "bplusindex.h"
#include "filterparser.h"
#include "indexfactory.h"
#include "math.h"
#include <gtest/gtest.h>
#include "filter_expressionLexer.h"
#include "filter_expressionParser.h"
#include "constantexpression.h"
#include "unaryexpression.h"
#include "simpleexpression.h"
#include "binaryexpression.h"
#include "expressionresult.h"

#include    <antlr3treeparser.h>
#include    <antlr3defs.h>
#include <memory>

using namespace std;



class TestDB: public testing::Test
{
	protected:
		static DBController* controller;
		std::vector<std::string> __ids;

	protected:
		static void SetUpTestCase()
		{
			remove("sp1.customer.dat");
			remove("sp1.customer.idx");
			remove("find.filter.dat");
			remove("find.filter.idx");

			FileOutputStream fos("simple.dat", "wb");
			/* 
				fos.writeString(std::string("1"));
				fos.writeString(std::string("4"));
				fos.writeString(std::string("6"));
				fos.writeString(std::string("2"));
				fos.writeString(std::string("3"));
				fos.writeString(std::string("5"));
				fos.writeString(std::string("8"));
				*/
			fos.writeString(std::string("13"));
			fos.writeString(std::string("1"));
			fos.writeString(std::string("4"));
			fos.writeString(std::string("11"));
			fos.writeString(std::string("6"));
			fos.writeString(std::string("15"));
			fos.writeString(std::string("20"));
			fos.writeString(std::string("18"));
			fos.writeString(std::string("8"));
			fos.writeString(std::string("19"));
			fos.writeString(std::string("7"));
			fos.writeString(std::string("13"));
			fos.writeString(std::string("9"));
			fos.writeString(std::string("14"));
			fos.writeString(std::string("5"));
			fos.writeString(std::string("17"));
			fos.writeString(std::string("10"));
			fos.writeString(std::string("16"));
			fos.writeString(std::string("12"));
			fos.close();

			controller = new DBController();
			controller->initialize();
		}

		static void TearDownTestCase() {
			controller->shutdown();
		}

		void testIndex(std::vector<std::string> ids)
		{
			std::set<std::string> keys;
			keys.insert("_id");
			std::auto_ptr<BPlusIndex> tree(new BPlusIndex(keys));
			//std::auto_ptr<BPlusIndexP> tree(new BPlusIndexP(keys, "testIndex"));

			Logger* log = getLogger(NULL);

			log->startTimeRecord();
			// Inserting
			int x = 0;
			for (std::vector<std::string>::iterator i = ids.begin(); i != ids.end(); i++)
			{
				BSONObj id;
				std::string sid = *i;

				log->debug("====================================");
				log->debug("Inserting %s", sid.c_str());

				id.add("_id", const_cast<char*>(sid.c_str()));
				if (log->isDebug()) {
					if (sid.compare("98979097-fce5-4265-ab0c-60b4e41a9d40") == 0) {
						log->debug("Error");
						tree->debug();
					}
				}
				tree->add(id, djondb::string(sid.c_str(), sid.length()), 0, 0);

				/*
					Index* test = tree->find(&id);
					if (test == NULL) {
					tree->debug();
					} else {
					if (log->isDebug()) tree->debug();
					}
					*/

				//getchar();
				x++;
			}
			log->stopTimeRecord();
			DTime time = log->recordedTime();

			log->info("Inserted in: %d", time.totalSecs());

			log->info("Starting find");
			log->startTimeRecord();
			while (ids.size() > 0)
			{
				int pos = rand() % ids.size();
				while (pos > ids.size())
				{
					pos = rand() % ids.size();
				}
				std::vector<std::string>::iterator i = ids.begin() + pos;
				std::string guid = *i;

				BSONObj id;
				id.add("_id", const_cast<char*>(guid.c_str()));
				Index* index = tree->find(&id);
				EXPECT_TRUE(index != NULL) << ("guid not found: " + guid).c_str();
				if (index != NULL) {
					BSONObj* key = index->key;
					EXPECT_TRUE(key != NULL) << format("Error searching for %s", guid.c_str()).c_str();
					EXPECT_TRUE(key->getString("_id").compare(guid.c_str()) == 0) << format("Error searching for %s", guid.c_str()).c_str();
				} else {
					log->debug("id: %s not found", guid.c_str());
				}

				ids.erase(i);
			}
			log->stopTimeRecord();
			time = log->recordedTime();
			if (log->isDebug()) tree->debug();
			log->info("found in: %d", time.totalSecs());

			BSONObj id2;
			id2.add("_id", "67c480cd-94cb-4acb-b039-c371357662dc");
			tree->find(&id2);
		}

		void testInsert(BSONObj* o)
		{
			controller->insert("dbtest", "sp1.customer", o);
		}

};

DBController* TestDB::controller = NULL;

TEST_F(TestDB, testExpressions) {
	cout << "\ntestExpressions" << endl;
	BSONObj dummy;
	ConstantExpression exp(35);
	ExpressionResult* result = exp.eval(dummy);

	EXPECT_TRUE(result->type() == ExpressionResult::RT_INT);
	int i = (int)*result;
	EXPECT_TRUE(i == 35);

	ConstantExpression exp2(3.324);
	ExpressionResult* result2 = exp2.eval(dummy);
	EXPECT_TRUE(result2->type() == ExpressionResult::RT_DOUBLE);
	double d = *result2;
	EXPECT_TRUE(d == 3.324);

	ConstantExpression exp3("Test");
	ExpressionResult* result3 = exp3.eval(dummy);
	EXPECT_TRUE(result3->type() == ExpressionResult::RT_PTRCHAR);
	djondb::string s = *result3;
	EXPECT_TRUE(strncmp(s.c_str(), "Test", s.length()) == 0);

	BSONObj obj;
	obj.add("age", 35);
	obj.add("name", "John");
	BSONObj inner;
	inner.add("i", 100);
	obj.add("child", inner);

	SimpleExpression exp4("$'age'");
	ExpressionResult* result4 = exp4.eval(obj);
	EXPECT_TRUE(result4->type() == ExpressionResult::RT_INT);
	int i2 = *result4;
	EXPECT_TRUE(i2 == 35);

	SimpleExpression exp5("$'name'");
	ExpressionResult* result5 = exp5.eval(obj);
	djondb::string s2 = *result5;
	EXPECT_TRUE(strncmp(s2.c_str(), "John", s2.length()) == 0);
	delete result5;

	SimpleExpression exp6("$'child.i'");
	ExpressionResult* result6 = exp6.eval(obj);
	int i3 = *result6;
	EXPECT_TRUE(i3 == 100);

	BinaryExpression exp7(FO_EQUALS);
	exp7.push(new SimpleExpression("$'age'"));
	exp7.push(new ConstantExpression(35));
	ExpressionResult* result7 = exp7.eval(obj);
	EXPECT_TRUE(result7->type() == ExpressionResult::RT_BOOLEAN);
	bool bresult7 = *result7;
	EXPECT_TRUE(bresult7 == true);

	BinaryExpression exp8(FO_GREATERTHAN);
	exp8.push(new SimpleExpression("$'age'"));
	exp8.push(new ConstantExpression(30));
	ExpressionResult* result8 = exp8.eval(obj);
	EXPECT_TRUE(result8->type() == ExpressionResult::RT_BOOLEAN);
	bool bresult8 = *result8;
	EXPECT_TRUE(bresult8 == true);

	BinaryExpression exp9(FO_NOT_EQUALS);
	exp9.push(new SimpleExpression("$'age'"));
	exp9.push(new ConstantExpression(36));
	ExpressionResult* result9 = exp9.eval(obj);
	EXPECT_TRUE(result9->type() == ExpressionResult::RT_BOOLEAN);
	bool bresult9 = *result9;
	EXPECT_TRUE(bresult9 == true);

}

TEST_F(TestDB, testUpdate) {
	cout << "\ntestUpdate" << endl;

	controller->dropNamespace("dbupdate", "ns");
	BSONObj obj;
	string* id = uuid();
	obj.add("_id", const_cast<char*>(id->c_str()));
	obj.add("name", "John");
	obj.add("age", 18);
	controller->insert("dbupdate", "ns", &obj);

	std::string filter = "$'_id' == '" + *id + "'";
	BSONObj* res1 = controller->findFirst("dbupdate", "ns", "*", filter.c_str());
	EXPECT_TRUE(res1->getInt("age") == 18);

	obj.add("age", 22);
	controller->update("dbupdate", "ns", &obj);

	BSONObj* res2 = controller->findFirst("dbupdate", "ns", "*", filter.c_str());

	EXPECT_TRUE(res2->getInt("age") == 22);

	delete res1;
	delete res2;
	delete id;

}

TEST_F(TestDB, testRemove) {
	cout << "\ntestRemove" << endl;

	controller->dropNamespace("dbdelete", "ns");
	BSONObj obj;
	string* id = uuid();
	obj.add("_id", const_cast<char*>(id->c_str()));
	string* revision = uuid();
	obj.add("_revision", const_cast<char*>(revision->c_str()));
	obj.add("name", "John");
	obj.add("age", 18);
	controller->insert("dbdelete", "ns", &obj);

	std::string filter = "$'_id' == '" + *id + "'";
	BSONObj* res1 = controller->findFirst("dbdelete", "ns", "*", filter.c_str());
	EXPECT_TRUE(res1->getInt("age") == 18);

	controller->remove("dbdelete", "ns", const_cast<char*>(id->c_str()), const_cast<char*>(revision->c_str()));

	BSONObj* res2 = controller->findFirst("dbdelete", "ns", "*", filter.c_str());

	EXPECT_TRUE(res2 == NULL);

	delete res1;
	delete id;

}

TEST_F(TestDB, testDbs) {
	cout << "\ntestDbs" << endl;
	BSONObj* obj = BSONParser::parse("{ 'a': 'a'}");

	controller->insert("db1", "ns1", obj);
	controller->insert("db2", "ns1", obj);
	controller->insert("db3", "ns1", obj);

	std::vector<std::string>* dbs = controller->dbs();
	EXPECT_TRUE(dbs->size() >= 3);

	delete dbs;
}

TEST_F(TestDB, testNamespaces) {
	cout << "\ntestNamespaces" << endl;
	BSONObj* obj = BSONParser::parse("{ 'a': 'a'}");

	std::vector<std::string>* nsTemp = controller->namespaces("testnamespacesdb");

	// Cleans up the previous namespaces
	for (std::vector<std::string>::iterator i = nsTemp->begin(); i != nsTemp->end(); i++) {
		controller->dropNamespace("testnamespacesdb", i->c_str());
	}
	delete nsTemp;

	controller->insert("testnamespacesdb", "ns1", obj);
	controller->insert("testnamespacesdb", "ns2", obj);
	controller->insert("testnamespacesdb", "ns3", obj);

	std::vector<std::string>* ns = controller->namespaces("testnamespacesdb");
	EXPECT_TRUE(ns->size() == 3);
	EXPECT_TRUE((*ns)[0].compare("ns1") == 0);
	EXPECT_TRUE((*ns)[1].compare("ns2") == 0);
	EXPECT_TRUE((*ns)[2].compare("ns3") == 0);
	delete ns;
}

TEST_F(TestDB, testFilterExpressionParser) {
	cout << "\ntestFilterExpressionParser" << endl;
	Logger* log = getLogger(NULL);
	try {
		BSONObj obj;
		obj.add("age", 35);
		obj.add("state", 1);
		obj.add("name", "John");
		obj.add("llong", (__int64)LLONG_MAX);

		FilterParser* parser = NULL;
		ExpressionResult* result = NULL;

		parser = FilterParser::parse("");
		result = parser->eval(obj);
		EXPECT_TRUE(result->type() == ExpressionResult::RT_BOOLEAN);
		bool bres = *result;
		EXPECT_TRUE(bres);

		parser = FilterParser::parse("$'age' == 35");
		result = parser->eval(obj);
		EXPECT_TRUE(result->type() == ExpressionResult::RT_BOOLEAN);
		bres = *result;
		EXPECT_TRUE(bres);

		parser = FilterParser::parse("($'age' == 35 )");
		result = parser->eval(obj);
		EXPECT_TRUE(result->type() == ExpressionResult::RT_BOOLEAN);
		bres = *result;
		EXPECT_TRUE(bres);

		parser = FilterParser::parse("(($'age' == 35 ) and ($'state' == 1 ))");
		result = parser->eval(obj);
		EXPECT_TRUE(result->type() == ExpressionResult::RT_BOOLEAN);
		bres = *result;
		EXPECT_TRUE(bres);

		parser = FilterParser::parse("(($'age' == 36 ) AnD ($'state' == 1 ))");
		result = parser->eval(obj);
		EXPECT_TRUE(result->type() == ExpressionResult::RT_BOOLEAN);
		bres = *result;
		EXPECT_TRUE(!bres);

		parser = FilterParser::parse("(($'age' == 35 ) AND ($'state' == 2 ))");
		result = parser->eval(obj);
		EXPECT_TRUE(result->type() == ExpressionResult::RT_BOOLEAN);
		bres = *result;
		EXPECT_TRUE(!bres);

		parser = FilterParser::parse("(($'age'==35) and ($'state'==1))");
		result = parser->eval(obj);
		EXPECT_TRUE(result->type() == ExpressionResult::RT_BOOLEAN);
		bres = *result;
		EXPECT_TRUE(bres);

		parser = FilterParser::parse("(('John' == $'name') and ($'age'==35))");
		result = parser->eval(obj);
		EXPECT_TRUE(result->type() == ExpressionResult::RT_BOOLEAN);
		bres = *result;
		EXPECT_TRUE(bres);

		parser = FilterParser::parse("(('John' == $'name') or ($'age'==36))");
		result = parser->eval(obj);
		EXPECT_TRUE(result->type() == ExpressionResult::RT_BOOLEAN);
		bres = *result;
		EXPECT_TRUE(bres);

		parser = FilterParser::parse("(('Johnny' == $'name') OR ($'age'==35))");
		result = parser->eval(obj);
		EXPECT_TRUE(result->type() == ExpressionResult::RT_BOOLEAN);
		bres = *result;
		EXPECT_TRUE(bres);

		parser = FilterParser::parse("($'age' > 15)");
		result = parser->eval(obj);
		EXPECT_TRUE(result->type() == ExpressionResult::RT_BOOLEAN);
		bres = *result;
		EXPECT_TRUE(bres);

		parser = FilterParser::parse("($'age' < 45)");
		result = parser->eval(obj);
		EXPECT_TRUE(result->type() == ExpressionResult::RT_BOOLEAN);
		bres = *result;
		EXPECT_TRUE(bres);

		parser = FilterParser::parse("($'age' >= 15)");
		result = parser->eval(obj);
		EXPECT_TRUE(result->type() == ExpressionResult::RT_BOOLEAN);
		bres = *result;
		EXPECT_TRUE(bres);

		parser = FilterParser::parse("($'age' >= 35)");
		result = parser->eval(obj);
		EXPECT_TRUE(result->type() == ExpressionResult::RT_BOOLEAN);
		bres = *result;
		EXPECT_TRUE(bres);

		parser = FilterParser::parse("($'age' <= 45)");
		result = parser->eval(obj);
		EXPECT_TRUE(result->type() == ExpressionResult::RT_BOOLEAN);
		bres = *result;
		EXPECT_TRUE(bres);

		parser = FilterParser::parse("($'age' <= 35)");
		result = parser->eval(obj);
		EXPECT_TRUE(result->type() == ExpressionResult::RT_BOOLEAN);
		bres = *result;
		EXPECT_TRUE(bres);

		parser = FilterParser::parse("($'name' == \"John\")");
		result = parser->eval(obj);
		EXPECT_TRUE(result->type() == ExpressionResult::RT_BOOLEAN);
		bres = *result;
		EXPECT_TRUE(bres);

		parser = FilterParser::parse("($'name' != \"Test\")");
		result = parser->eval(obj);
		EXPECT_TRUE(result->type() == ExpressionResult::RT_BOOLEAN);
		bres = *result;
		EXPECT_TRUE(bres);

		// Eval an attribute that does not exist
		parser = FilterParser::parse("($'nn' == \"John\")");
		result = parser->eval(obj);
		EXPECT_TRUE(result->type() == ExpressionResult::RT_BOOLEAN);
		bres = *result;
		EXPECT_TRUE(!bres);

		// Eval an attribute that does not exist
		log->info("Complex filter");
		parser = FilterParser::parse("($'process' == 'Complains') and ((($'values.complains__idCustomer__identification' == '63154446') or ($'values.complains__idCustomer__identification' == '09821150')))");
		result = parser->eval(obj);
		EXPECT_TRUE(result->type() == ExpressionResult::RT_BOOLEAN);
		bres = *result;
		EXPECT_TRUE(!bres);

		char filter[1000];
		memset(filter, 0, 1000);
		sprintf(filter, "$'llong' == %lld", LLONG_MAX);
		parser = FilterParser::parse(filter);
		result = parser->eval(obj);
		EXPECT_TRUE(result->type() == ExpressionResult::RT_BOOLEAN);
		bres = *result;
		EXPECT_TRUE(bres);

	} catch (ParseException& e) {
		FAIL() << "A ParseException should not rise in this method";
	}
}

TEST_F(TestDB, testFilterExpressionParserEquals) {
	cout << "\ntestFilterExpressionParserEquals" << endl;
	BSONObj obj;
	obj.add("age", 35);

	FilterParser* parser = FilterParser::parse("$'age' == $'age'");
	/* 
		BSONContent* content = (BSONContent*)parser->eval(obj);

		int* test = *content;

		EXPECT_TRUE(test != NULL);
		EXPECT_TRUE(*test == 35);
		*/
	delete parser;

}

TEST_F(TestDB, testInsertWithStringId)
{
	cout << "\ntestInsertWithStringId" << endl;
	BSONObj obj;
	std::string* id = uuid();
	obj.add("_id", id->c_str());
	obj.add("name", (const char*)"cross");
	delete id;
	controller->insert("dbtest", "sp1.customer", &obj);
}

TEST_F(TestDB, testInsertWithCharId)
{
	cout << "\ntestInsertWithCharId" << endl;
	BSONObj obj;
	std::string* id = uuid();
	obj.add("_id", id->c_str());
	obj.add("name", (const char*)"cross");
	delete id;
	controller->insert("dbtest", "sp1.customer", &obj);
}

TEST_F(TestDB, testInsertComplexBSON) {
	cout << "\ntestInsertComplexBSON" << endl;

	controller->dropNamespace("dbtest", "sp1.customercomplex");
	BSONObj obj;
	obj.add("int", 1);
	obj.add("double", 1.1);
	obj.add("char", "test");

	BSONObj inner;
	inner.add("int", (int)200000);
	inner.add("double", 1.1);
	inner.add("char", "testInner");
	obj.add("inner", inner);

	BSONArrayObj innerArray;
	BSONObj o1;
	o1.add("int", (int)1);
	innerArray.add(o1);

	obj.add("array", innerArray);

	controller->insert("dbtest", "sp1.customercomplex", &obj);

	BSONArrayObj* array = controller->find("dbtest", "sp1.customercomplex", "*", "$'int' == 1");
	EXPECT_TRUE(array->length() == 1);
	if (array->length() == 1) {
		BSONObj* res = *array->begin();
		EXPECT_TRUE(res != NULL);
		EXPECT_TRUE(res->has("_id"));
		EXPECT_TRUE(res->has("int"));
		if (res->has("int")) {
			cout << "\n\nint value: " << res->getInt("int") << endl << endl;
			EXPECT_TRUE(res->getInt("int") == 1);
		}
		EXPECT_TRUE(res->has("double"));
		if (res->has("double")) {
			EXPECT_TRUE(res->getDouble("double") == 1.1);
		}
		EXPECT_TRUE(res->getBSON("inner") != NULL);
		BSONObj* innerRes = res->getBSON("inner");
		EXPECT_TRUE(innerRes != NULL);
		EXPECT_TRUE(innerRes->has("char"));
		if (innerRes->has("char")) {
			EXPECT_TRUE(innerRes->getString("char").compare("testInner") == 0);
		}
		EXPECT_TRUE(innerRes->has("int"));
		if (innerRes->has("int")) {
			cout << "\n\ninner int value: " << innerRes->getInt("int") << endl << endl;
			EXPECT_TRUE(innerRes->getInt("int") == 200000);
		}

		cout << "testing array" << endl;
		EXPECT_TRUE(res->has("array"));
		if (res->has("array")) {
			BSONArrayObj* innerArrayRes = res->getBSONArray("array");
			EXPECT_TRUE(innerArrayRes->length() == 1);
			if (innerArrayRes->length() == 1) {
				BSONObj* oa1 = innerArrayRes->get(0);
				EXPECT_TRUE(oa1->has("int"));
				if (oa1->has("int")) {
					EXPECT_TRUE(oa1->getInt("int") == 1);
				}

			}
		}
	}
	delete array;
}

TEST_F(TestDB, testMassiveInsert)
{
	cout << "\ntestMassiveInsert" << endl;
	int inserts = 100;
	Logger* log = getLogger(NULL);

	log->startTimeRecord();

	FileOutputStream fos("temp.txt", "wb+");

	for (int x = 0; x < inserts; x++)
	{
		BSONObj* obj = new BSONObj();
		std::string* id = uuid();
		obj->add("_id", const_cast<char*>(id->c_str()));
		obj->add("name", "John");
		char temp[700];
		memset(temp, 0, 699);
		memset(temp, 'a', 700);
		obj->add("content", (char*)temp);
		obj->add("last", (char*)"Smith");
		testInsert(obj);

		int test = rand() % 10;
		if (test > 0)
		{
			__ids.push_back(*id);
			fos.writeString(id->c_str());
		}
		if ((x % 1000000) == 0)
		{
			cout<< "inserts " << x << endl;
		}
		delete obj;
		delete id;
	}
	fos.close();

	log->stopTimeRecord();

	int secs = log->recordedTime().totalSecs();

	cout<< "inserts " << inserts << ", secs: " << secs << endl;

	if (secs > 0)
	{
		// If throughtput is too small fail
		EXPECT_TRUE((inserts / secs) > 10000);
		cout << "\nThroughput: " << (inserts / secs) << " ops." << endl;
		cout << "\n------------------------------------------------------------" << endl;
	}
}

TEST_F(TestDB, testAntlrParser) {
	pANTLR3_INPUT_STREAM           input;
	pfilter_expressionLexer               lex;
	pANTLR3_COMMON_TOKEN_STREAM    tokens;
	pfilter_expressionParser              parser;

	char* filter = "a == 1";
	input  = antlr3NewAsciiStringInPlaceStream((pANTLR3_UINT8)filter, (ANTLR3_INT8)strlen(filter), (pANTLR3_UINT8)"name");
	lex    = filter_expressionLexerNew                (input);
	tokens = antlr3CommonTokenStreamSourceNew  (ANTLR3_SIZE_HINT, TOKENSOURCE(lex));
	parser = filter_expressionParserNew               (tokens);

	parser ->start_point(parser);

	// Must manually clean up
	//
	parser ->free(parser);
	tokens ->free(tokens);
	lex    ->free(lex);
	input  ->close(input);

}

TEST_F(TestDB, testFinds)
{
	cout << "\ntestFinds" << endl;

	Logger* log = getLogger(NULL);

	log->startTimeRecord();

	for (std::vector<string>::iterator i = __ids.begin(); i != __ids.end(); i++)
	{
		string id = *i;

		std::string filter = format("$\"_id\" == \"%s\"", id.c_str());

		BSONObj* res = controller->findFirst("dbtest", "sp1.customer", "*", filter.c_str());
		EXPECT_TRUE(res != NULL);
		if (res == NULL) {
			FAIL() << "res is null";
			return;
		}
		djondb::string id2 = res->getDJString("_id");
		if (id2.compare(id.c_str(), id.length()) != 0)
		{
			FAIL() << "id not found";
			return;
		}
		delete res;
	}

	log->stopTimeRecord();

	int secs = log->recordedTime().totalSecs();

	if (secs > 0)
	{
		EXPECT_TRUE((__ids.size() / secs) > 30);
		cout << "\nThroughput: " << (__ids.size() / secs) << " ops." << endl;
		cout << "\n------------------------------------------------------------" << endl;
	}
}

TEST_F(TestDB, testFindPartial) {
	controller->dropNamespace("testdb", "partial");

	std::string record("{ 'name': 'John', 'lastName': 'Smith', 'address': { 'phone': '555-12345', 'type': 'home'} }");

	BSONObj* obj = BSONParser::parse(record.c_str());

	controller->insert("testdb", "partial", obj);

	BSONArrayObj* result = controller->find("testdb", "partial", "*", "$\"name\", $\"lastName\"");

	EXPECT_TRUE(result != NULL);
	if (result != NULL) {
		BSONObj testObj;
		testObj.add("name", "John");
		testObj.add("lastName", "Smith");
		EXPECT_TRUE(result->length() == 1);
		if (result->length() > 0) {
			BSONObj* test = *result->begin();

			EXPECT_TRUE(*test == testObj);

			delete test;
		}
		delete result;
	}

	delete obj;
}

TEST_F(TestDB, testFindsFilterErrors) {
	// This will check for several filters with parsing errors
	cout << "\ntestFindsFilterErrors" << endl;


	std::string filter("a x n"); // bad constants
	try {
		controller->find("dbtest", "sp1.customer", "*", filter.c_str());
		FAIL() << "An error should occur";
	} catch (ParseException& e) {
		cout << "\nException --> " << e.what() << endl;
	}
}

TEST_F(TestDB, testFindsByFilter)
{
	cout << "\ntestFindsByFilter" << endl;
	// Insert some data
	//
	controller->dropNamespace("dbtest", "find.filter");
	controller->insert("dbtest", "find.filter", BSONParser::parse("{name: 'Juan', lastName:'Crossley'}"));
	controller->insert("dbtest", "find.filter", BSONParser::parse("{name: 'Pepe', lastName:'Crossley'}"));
	controller->insert("dbtest", "find.filter", BSONParser::parse("{name: 'Juan', lastName:'Smith'}"));
	controller->insert("dbtest", "find.filter", BSONParser::parse("{name: 'Juan', lastName:'Clark'}"));
	controller->insert("dbtest", "find.filter", BSONParser::parse("{name: 'Juan', lastName:'Crossley'}"));
	controller->insert("dbtest", "find.filter", BSONParser::parse("{name: 'Juan', lastName:'Crossley'}"));
	controller->insert("dbtest", "find.filter", BSONParser::parse("{name: 'Juan', lastName:'Crossley'}"));
	controller->insert("dbtest", "find.filter", BSONParser::parse("{name: 'Juan', lastName:'Last'}"));

	BSONObj* filter = BSONParser::parse("{lastName: 'Crossley'}");

	// Starting find by filter
	BSONArrayObj* found = controller->find("dbtest", "find.filter", "*", "$\"lastName\" == \"Crossley\"");
	EXPECT_TRUE(found->length() == 5); 
	delete found;
	delete filter;

	found = controller->find("dbtest", "find.filter", "*", "");
	EXPECT_TRUE(found->length() == 8); 
	delete found;

	found = controller->find("dbtest", "find.filter", "*", "$\"name\": \"Juan\"");
	EXPECT_TRUE(found->length() == 7); 
	delete found;

	found = controller->find("dbtest", "find.filter", "*", "$'name' == 'Juan' and $'lastName' == 'Smith'");
	EXPECT_TRUE(found->length() == 1); 
	delete found;

	found = controller->find("dbtest", "find.filter", "*", "$'name' == 'Juan' and $'lastName' == 'Last'");
	EXPECT_TRUE(found->length() == 1); 
	delete found;
}

TEST_F(TestDB, testFindsByTextFilter)
{
	cout << "\ntestFindsByTextFilter" << endl;
	// Insert some data
	//
	controller->dropNamespace("dbtest", "find.filter2");
	controller->insert("dbtest", "find.filter2", BSONParser::parse("{name: 'Juan', lastName:'Crossley', age: 38, inner: { x: 1}}"));
	controller->insert("dbtest", "find.filter2", BSONParser::parse("{name: 'Pepe', lastName:'Crossley', age: 15, inner: { x: 1}}"));
	controller->insert("dbtest", "find.filter2", BSONParser::parse("{name: 'Juan', lastName:'Smith', age: 45, inner: { x: 2}}"));
	controller->insert("dbtest", "find.filter2", BSONParser::parse("{name: 'Juan', lastName:'Clark', age: 38, inner: { x: 3}}"));

	std::string filter = "$'age' == 45";
	BSONArrayObj* found = controller->find("dbtest", "find.filter2","*", filter.c_str());
	EXPECT_TRUE(found->length() == 1); 
	djondb::string name = found->get(0)->getDJString("lastName");
	EXPECT_TRUE(name.compare("Smith", 5) == 0);

	filter = "";
	delete found;
	found = controller->find("dbtest", "find.filter2","*", filter.c_str());
	EXPECT_TRUE(found->length() == 4); 

	filter = "$'age' == 38";
	delete found;
	found = controller->find("dbtest", "find.filter2","*", filter.c_str());
	EXPECT_TRUE(found->length() == 2); 
	name = found->get(0)->getDJString("lastName");
	EXPECT_TRUE(name.compare("Crossley", 8) == 0);
	name = found->get(1)->getDJString("lastName");
	EXPECT_TRUE(name.compare("Clark", 5) == 0);
	delete found;

	found = controller->find("dbtest", "find.filter2", "*", "$'inner.x' == 1");
	EXPECT_TRUE(found->length() == 2); 
	delete found;
}

TEST_F(TestDB, testFindPrevious)
{
	cout << "\ntestFindPrevious" << endl;
	Logger* log = getLogger(NULL);

	FileInputStream fis("temp.txt", "rb");
	std::vector<std::string*> ids;
	while (!fis.eof())
	{
		ids.push_back(fis.readString());
	}
	fis.close();

	log->startTimeRecord();

	for (std::vector<string*>::iterator i = ids.begin(); i != ids.end(); i++)
	{
		string* id = *i;

		std::string filter = format("$\"_id\" == \"%s\"", id->c_str());
		BSONObj* res = controller->findFirst("dbtest", "sp1.customer", "*", filter.c_str());
		if (res == NULL)
		{
			FAIL() << "Looking for a previous id does not returned any match";
			break;
		}
		else
		{
			djondb::string id2 = res->getDJString("_id");
			//        cout << "\nLooking for: " << *id << endl;
			//        cout << "\nFound        " << *id2 << endl;
			if (strcmp(id2, id->c_str()) != 0)
			{
				FAIL() << "findFirst returned an incorrect result";
			}
			delete res;
		}
	}

	log->stopTimeRecord();

	int secs = log->recordedTime().totalSecs();

	if (secs > 0)
	{
		EXPECT_TRUE((ids.size() / secs) > 30);
		cout << "\nThroughput: " << (ids.size() / secs) << " ops." << endl;
		cout << "\n------------------------------------------------------------" << endl;
	}
}

/* 
TEST_F(TestDB, testManualIndex)
{
	cout << "\ntestManualIndex" << endl;
	std::set<std::string> keys;
	keys.insert("_id");
	std::auto_ptr<BPlusIndexP> tree(new BPlusIndexP(keys));

	Logger* log = getLogger(NULL);

	log->startTimeRecord();
	// Inserting
	int x = 0;
	char chr[100];
	do {
		cout << "Element: ";
		scanf("%s", chr);
		if (strncmp(chr, "end", 3) == 0) {
			break;
		}
		cout << "Number readed: " << chr << endl;
		BSONObj id;
		id.add("_id", const_cast<char*>(chr));
		tree->add(id, djondb::string(chr, strlen(chr)), 0, 0);
		tree->debug();
		getchar();
		x++;
	} while (strncmp(chr, "end", 3) != 0);
}
*/

TEST_F(TestDB, testIndexPage) {
	std::vector<std::string> ids;

	ids.push_back("1\0");
	ids.push_back("2\0");
	ids.push_back("3\0");
	ids.push_back("4\0");
	ids.push_back("5\0");

	testIndex(ids);

	ids.push_back("6\0");
	testIndex(ids);
	ids.push_back("7\0");
	ids.push_back("8\0");
	ids.push_back("9\0");
	ids.push_back("91\0");
	ids.push_back("92\0");
	testIndex(ids);

	ids.push_back("11\0");
	ids.push_back("12\0");
	ids.push_back("13\0");
	testIndex(ids);

	ids.clear();
	ids.push_back("1f0d2a02-e812-433c-8615-0db9dbad0eae\0");
	ids.push_back("803bbd79-b897-4a68-b8bb-ca54eb52b8ec\0");
	ids.push_back("9012e35d-b117-4669-a8f7-b3418033cebb\0");
	ids.push_back("c840c949-3509-46d8-8503-d203646913a4\0");
	ids.push_back("dd4339cd-d5d4-4e53-9165-e7410b4d42c5\0");
	ids.push_back("a9594d16-2358-4e38-a42c-2c61ddd88c36\0");
	testIndex(ids);

	ids.clear();
	Logger* log = getLogger(NULL);
	for (int x = 0; x < 100; x++) {
		std::string* guid = uuid();
		ids.push_back(*guid);
		log->debug(guid->c_str());
		delete guid;
	}
	testIndex(ids);
}

/*
TEST_F(TestDB, testMassiveInsertIndex)
{
	std::set<std::string> keys;
	keys.insert("_id");
	std::auto_ptr<BPlusIndexP> tree(new BPlusIndexP(keys));

	Logger* log = getLogger(NULL);

	// Inserting
	for (int n = 0; n < 100; n++)
	{
		log->startTimeRecord();
		for (int x = 0; x < 100000; x++) {
			BSONObj id;
			std::string* sid = uuid();

			id.add("_id", const_cast<char*>(sid->c_str()));

			tree->add(id, djondb::string(sid->c_str(), sid->length()), 0, 0);
			delete sid;

		}
		log->stopTimeRecord();
		DTime time = log->recordedTime();
		log->info("Inserted in: %d", time.totalSecs());
		log->info("Total inserted: %d", 100000 * (n + 1));
	}

}
*/

TEST_F(TestDB, testSimpleIndex)
{
	cout << "\ntestSimpleIndex" << endl;
	FileInputStream fis("simple.dat", "rb");
	std::vector<std::string> ids;
	while (!fis.eof())
	{
		std::string* s = fis.readString();
		ids.push_back(*s);
		delete s;
	}
	fis.close();
	testIndex(ids);
}

std::vector<std::string> generateGuids(int count) {
	std::vector<std::string> ids;
	for (int x = 0; x < count; x++)
	{
		std::string* guid = uuid();
		ids.push_back(*guid);
		delete guid;
	}
	return ids;
}

/* 
TEST_F(TestDB, testHighMemIndex)
{
	cout << "\ntestHighMemIndex" << endl;

	int x = 100000;
	cout << "testing " << x << " ids" << endl;
	std::vector<std::string> ids = generateGuids(x);
	testIndex(ids);
	
//		cout << "testing 100 ids" << endl;
//		ids = generateGuids(100);
//		testIndex(ids);
//		cout << "testing 1000 ids" << endl;
//		ids = generateGuids(1000);
//		testIndex(ids);
//		cout << "testing 1000000 ids" << endl;
//		ids = generateGuids(1000);
//		testIndex(ids);
}
*/

TEST_F(TestDB, testComplexIndex)
{
	cout << "\ntestComplexIndex" << endl;
	cout << "testing 10 ids" << endl;
	std::vector<std::string> ids = generateGuids(10);
	testIndex(ids);
	/*
		cout << "testing 100 ids" << endl;
		ids = generateGuids(100);
		testIndex(ids);
		cout << "testing 1000 ids" << endl;
		ids = generateGuids(1000);
		testIndex(ids);
		cout << "testing 1000000 ids" << endl;
		ids = generateGuids(1000);
		testIndex(ids);
		*/
}

TEST_F(TestDB, testIndexFactory) {
	cout << "\ntestIndexFactory" << endl;

	IndexAlgorithm* index = IndexFactory::indexFactory.index("dbtest", "ns.a", "_id");
	EXPECT_TRUE(index != NULL);

	// Let's check if the factory returns the same instance for the same key
	IndexAlgorithm* indexCompare = IndexFactory::indexFactory.index("dbtest", "ns.a", "_id");
	EXPECT_TRUE(index == indexCompare);

	// Let's change the keys and test if a new IndexAlgorithm will be returned
	IndexAlgorithm* indexCompare2 = IndexFactory::indexFactory.index("dbtest", "ns.a", "key");
	EXPECT_TRUE(index != indexCompare2);

	// Checking the contains method
	bool res = IndexFactory::indexFactory.containsIndex("dbtest", "ns.a", "_id");
	EXPECT_TRUE(res);

	bool res2 = IndexFactory::indexFactory.containsIndex("dbtest", "ns.a", "nkey");
	EXPECT_TRUE(!res2);
}

TEST_F(TestDB, testDropIndexes) {
	cout << "\ntestDropIndexes" << endl;

	IndexAlgorithm* index = IndexFactory::indexFactory.index("dbtest", "ns.a", "_id");
	EXPECT_TRUE(index != NULL);

	// Let's check if the factory returns the same instance for the same key
	IndexAlgorithm* indexCompare = IndexFactory::indexFactory.index("dbtest", "ns.a", "_id");
	EXPECT_TRUE(index == indexCompare);

	// Let's change the keys and test if a new IndexAlgorithm will be returned
	IndexAlgorithm* indexCompare2 = IndexFactory::indexFactory.index("dbtest", "ns.a", "key");
	EXPECT_TRUE(index != indexCompare2);

	// Checking the contains method
	bool res = IndexFactory::indexFactory.containsIndex("dbtest", "ns.a", "_id");
	EXPECT_TRUE(res);

	bool res2 = IndexFactory::indexFactory.containsIndex("dbtest", "ns.a", "nkey");
	EXPECT_TRUE(!res2);

	IndexFactory::indexFactory.dropIndex("dbtest", "ns.a", "_id");

	res = IndexFactory::indexFactory.containsIndex("dbtest", "ns.a", "_id");
	EXPECT_TRUE(!res);

	IndexFactory::indexFactory.index("dbtest", "ns.a", "test");
	IndexFactory::indexFactory.index("dbtest", "ns.a", "next");
	IndexFactory::indexFactory.dropIndexes("dbtest", "ns.a");
	res = IndexFactory::indexFactory.containsIndex("dbtest", "ns.a", "_id");
	EXPECT_TRUE(!res);
	res = IndexFactory::indexFactory.containsIndex("dbtest", "ns.a", "key");
	EXPECT_TRUE(!res);
	res = IndexFactory::indexFactory.containsIndex("dbtest", "ns.a", "test");
	EXPECT_TRUE(!res);
	res = IndexFactory::indexFactory.containsIndex("dbtest", "ns.a", "next");
	EXPECT_TRUE(!res);
}

TEST_F(TestDB, testDropnamespace)
{
	cout << "\ntestDropnamespace" << endl;
	BSONObj obj;
	obj.add("name", "Test");

	controller->insert("dbtest", "ns.drop", &obj);

	bool result = controller->dropNamespace("dbtest", "ns.drop");
	EXPECT_TRUE(result);

	BSONArrayObj* finds = controller->find("dbtest", "ns.drop", "*", "");

	EXPECT_TRUE(finds->length() == 0);

	delete finds;
}

TEST_F(TestDB, testErrorHandling) {
	// Test errors at filter expressions
	//
	// FilterParser* parser = FilterParser::parse("A = B");
	// EXPECT_TRUE(parser == NULL);
}

