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
#include "bplusindex.h"
#include "filterparser.h"
#include "indexfactory.h"
#include "math.h"
#include <cpptest.h>
#include "filter_expressionLexer.h"
#include "filter_expressionParser.h"
#include "constantexpression.h"
#include "unaryexpression.h"
#include "simpleexpression.h"
#include "binaryexpression.h"
#include "expressionresult.h"

#include    <antlr3treeparser.h>
#include    <antlr3defs.h>

using namespace std;

DBController* controller;

class TestDBSuite: public Test::Suite
{
	public:
		TestDBSuite()
		{
			remove("sp1.customer.dat");
			remove("sp1.customer.idx");
			remove("find.filter.dat");
			remove("find.filter.idx");

			FileOutputStream fos("simple.dat", "wb");
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
			fos.writeString(std::string("12"));
			fos.writeString(std::string("16"));
			fos.writeString(std::string("12"));
			fos.writeString(std::string("13"));
			fos.close();

			TEST_ADD(TestDBSuite::testInsertComplexBSON);
			TEST_ADD(TestDBSuite::testSimpleIndex);
			TEST_ADD(TestDBSuite::testComplexIndex);
			TEST_ADD(TestDBSuite::testIndexFactory);
			TEST_ADD(TestDBSuite::testExpressions);
			TEST_ADD(TestDBSuite::testFilterExpressionParser);
			TEST_ADD(TestDBSuite::testFilterExpressionParserEquals);
			TEST_ADD(TestDBSuite::testInsertWithStringId);
			TEST_ADD(TestDBSuite::testInsertWithCharId);

			// TEST_ADD(TestDBSuite::testFindPrevious);
			TEST_ADD(TestDBSuite::testMassiveInsert);
			TEST_ADD(TestDBSuite::testFinds);
			TEST_ADD(TestDBSuite::testFindsFilterErrors);
			TEST_ADD(TestDBSuite::testFindsByFilter);
			TEST_ADD(TestDBSuite::testFindsByTextFilter);
			TEST_ADD(TestDBSuite::testFindPartial);
			TEST_ADD(TestDBSuite::testDropnamespace);
			TEST_ADD(TestDBSuite::testDbs);
			TEST_ADD(TestDBSuite::testNamespaces);
			TEST_ADD(TestDBSuite::testErrorHandling);
		}
	private:

		std::vector<std::string*> __ids;

	private:
		void testInsert(BSONObj* o)
		{
			BSONObj* res = controller->insert("dbtest", "sp1.customer", o);
			delete res;
		}

		void testExpressions() {
			cout << "\ntestExpressions" << endl;
			BSONObj dummy;
			ConstantExpression exp(35);
			ExpressionResult* result = exp.eval(dummy);

			TEST_ASSERT(result->type() == ExpressionResult::RT_INT);
			int* i = (int*)result->value();
			TEST_ASSERT(*i == 35);

			ConstantExpression exp2(3.324);
			ExpressionResult* result2 = exp2.eval(dummy);
			TEST_ASSERT(result2->type() == ExpressionResult::RT_DOUBLE);
			double* d = (double*)result2->value();
			TEST_ASSERT(*d == 3.324);

			ConstantExpression exp3("Test");
			ExpressionResult* result3 = exp3.eval(dummy);
			TEST_ASSERT(result3->type() == ExpressionResult::RT_STRINGDB);
			std::string* s = (std::string*)result3->value();
			TEST_ASSERT(s != NULL);
			TEST_ASSERT(s->compare("Test") == 0);

			BSONObj obj;
			obj.add("age", 35);
			obj.add("name", "John");
			BSONObj inner;
			inner.add("i", 100);
			obj.add("child", inner);

			SimpleExpression exp4("$'age'");
			ExpressionResult* result4 = exp4.eval(obj);
			TEST_ASSERT(result4->type() == ExpressionResult::RT_INT);
			int* i2 = (int*)result4->value();
			TEST_ASSERT(i2 != NULL);
			TEST_ASSERT(*i2 == 35);

			SimpleExpression exp5("$'name'");
			ExpressionResult* result5 = exp5.eval(obj);
			std::string* s2 = (std::string*)result5->value();
			TEST_ASSERT(s2 != NULL);
			TEST_ASSERT(s2->compare("John") == 0);
			delete result5;

			SimpleExpression exp6("$'child.i'");
			ExpressionResult* result6 = exp6.eval(obj);
			int* i3 = (int*)result6->value();
			TEST_ASSERT(i3 != NULL);
			TEST_ASSERT(*i3 == 100);

			BinaryExpression exp7(FO_EQUALS);
			exp7.push(new SimpleExpression("$'age'"));
			exp7.push(new ConstantExpression(35));
			ExpressionResult* result7 = exp7.eval(obj);
			TEST_ASSERT(result7->type() == ExpressionResult::RT_BOOLEAN);
			bool* bresult7 = (bool*)result7->value();
			TEST_ASSERT(*bresult7 == true);

			BinaryExpression exp8(FO_GREATERTHAN);
			exp8.push(new SimpleExpression("$'age'"));
			exp8.push(new ConstantExpression(30));
			ExpressionResult* result8 = exp8.eval(obj);
			TEST_ASSERT(result8->type() == ExpressionResult::RT_BOOLEAN);
			bool* bresult8 = (bool*)result8->value();
			TEST_ASSERT(*bresult8 == true);
		}

		void testDbs() {
			cout << "\ntestDbs" << endl;
			BSONObj* obj = BSONParser::parse("{ 'a': 'a'}");

			controller->insert("db1", "ns1", obj);
			controller->insert("db2", "ns1", obj);
			controller->insert("db3", "ns1", obj);

			std::vector<std::string>* dbs = controller->dbs();
			TEST_ASSERT(dbs->size() >= 3);

			delete dbs;
		}

		void testNamespaces() {
			cout << "\ntestNamespaces" << endl;
			BSONObj* obj = BSONParser::parse("{ 'a': 'a'}");

			controller->insert("testnamespacesdb", "ns1", obj);
			controller->insert("testnamespacesdb", "ns2", obj);
			controller->insert("testnamespacesdb", "ns3", obj);

			std::vector<std::string>* ns = controller->namespaces("testnamespacesdb");
			TEST_ASSERT(ns->size() == 3);
			TEST_ASSERT((*ns)[0].compare("ns1") == 0);
			TEST_ASSERT((*ns)[1].compare("ns2") == 0);
			TEST_ASSERT((*ns)[2].compare("ns3") == 0);
			delete ns;
		}

		void testFilterExpressionParser() {
			cout << "\ntestFilterExpressionParser" << endl;
			try {
				BSONObj obj;
				obj.add("age", 35);
				obj.add("state", 1);
				obj.add("name", "John");
				obj.add("llong", LLONG_MAX);

				FilterParser* parser = NULL;
				ExpressionResult* result = NULL;

				parser = FilterParser::parse("");
				result = parser->eval(obj);
				TEST_ASSERT(result->type() == ExpressionResult::RT_BOOLEAN);
				bool* bres = (bool*)result->value();
				TEST_ASSERT(*bres);

				parser = FilterParser::parse("$'age' == 35");
				result = parser->eval(obj);
				TEST_ASSERT(result->type() == ExpressionResult::RT_BOOLEAN);
				bres = (bool*)result->value();
				TEST_ASSERT(*bres);

				parser = FilterParser::parse("($'age' == 35 )");
				result = parser->eval(obj);
				TEST_ASSERT(result->type() == ExpressionResult::RT_BOOLEAN);
				bres = (bool*)result->value();
				TEST_ASSERT(*bres);

				parser = FilterParser::parse("(($'age' == 35 ) and ($'state' == 1 ))");
				result = parser->eval(obj);
				TEST_ASSERT(result->type() == ExpressionResult::RT_BOOLEAN);
				bres = (bool*)result->value();
				TEST_ASSERT(*bres);

				parser = FilterParser::parse("(($'age' == 36 ) and ($'state' == 1 ))");
				result = parser->eval(obj);
				TEST_ASSERT(result->type() == ExpressionResult::RT_BOOLEAN);
				bres = (bool*)result->value();
				TEST_ASSERT(!*bres);

				parser = FilterParser::parse("(($'age' == 35 ) and ($'state' == 2 ))");
				result = parser->eval(obj);
				TEST_ASSERT(result->type() == ExpressionResult::RT_BOOLEAN);
				bres = (bool*)result->value();
				TEST_ASSERT(!*bres);

				parser = FilterParser::parse("(($'age'==35) and ($'state'==1))");
				result = parser->eval(obj);
				TEST_ASSERT(result->type() == ExpressionResult::RT_BOOLEAN);
				bres = (bool*)result->value();
				TEST_ASSERT(*bres);

				parser = FilterParser::parse("(('John' == $'name') and ($'age'==35))");
				result = parser->eval(obj);
				TEST_ASSERT(result->type() == ExpressionResult::RT_BOOLEAN);
				bres = (bool*)result->value();
				TEST_ASSERT(*bres);

				parser = FilterParser::parse("(('John' == $'name') or ($'age'==36))");
				result = parser->eval(obj);
				TEST_ASSERT(result->type() == ExpressionResult::RT_BOOLEAN);
				bres = (bool*)result->value();
				TEST_ASSERT(*bres);

				parser = FilterParser::parse("(('Johnny' == $'name') or ($'age'==35))");
				result = parser->eval(obj);
				TEST_ASSERT(result->type() == ExpressionResult::RT_BOOLEAN);
				bres = (bool*)result->value();
				TEST_ASSERT(*bres);

				parser = FilterParser::parse("($'age' > 15)");
				result = parser->eval(obj);
				TEST_ASSERT(result->type() == ExpressionResult::RT_BOOLEAN);
				bres = (bool*)result->value();
				TEST_ASSERT(*bres);

				parser = FilterParser::parse("($'age' < 45)");
				result = parser->eval(obj);
				TEST_ASSERT(result->type() == ExpressionResult::RT_BOOLEAN);
				bres = (bool*)result->value();
				TEST_ASSERT(*bres);

				parser = FilterParser::parse("($'age' >= 15)");
				result = parser->eval(obj);
				TEST_ASSERT(result->type() == ExpressionResult::RT_BOOLEAN);
				bres = (bool*)result->value();
				TEST_ASSERT(*bres);

				parser = FilterParser::parse("($'age' >= 35)");
				result = parser->eval(obj);
				TEST_ASSERT(result->type() == ExpressionResult::RT_BOOLEAN);
				bres = (bool*)result->value();
				TEST_ASSERT(*bres);

				parser = FilterParser::parse("($'age' <= 45)");
				result = parser->eval(obj);
				TEST_ASSERT(result->type() == ExpressionResult::RT_BOOLEAN);
				bres = (bool*)result->value();
				TEST_ASSERT(*bres);

				parser = FilterParser::parse("($'age' <= 35)");
				result = parser->eval(obj);
				TEST_ASSERT(result->type() == ExpressionResult::RT_BOOLEAN);
				bres = (bool*)result->value();
				TEST_ASSERT(*bres);

				parser = FilterParser::parse("($'name' == \"John\")");
				result = parser->eval(obj);
				TEST_ASSERT(result->type() == ExpressionResult::RT_BOOLEAN);
				bres = (bool*)result->value();
				TEST_ASSERT(*bres);

				// Eval an attribute that does not exist
				parser = FilterParser::parse("($'nn' == \"John\")");
				result = parser->eval(obj);
				TEST_ASSERT(result->type() == ExpressionResult::RT_BOOLEAN);
				bres = (bool*)result->value();
				TEST_ASSERT(!*bres);

				char filter[1000];
				memset(filter, 0, 1000);
				sprintf(filter, "$'llong' == %lld", LLONG_MAX);
				parser = FilterParser::parse(filter);
				result = parser->eval(obj);
				TEST_ASSERT(result->type() == ExpressionResult::RT_BOOLEAN);
				bres = (bool*)result->value();
				TEST_ASSERT(*bres);

			} catch (ParseException& e) {
				TEST_FAIL(e.what());
			}
		}

		void testFilterExpressionParserEquals() {
			cout << "\ntestFilterExpressionParserEquals" << endl;
			BSONObj obj;
			obj.add("age", 35);

			FilterParser* parser = FilterParser::parse("$'age' == $'age'");
			/* 
				BSONContent* content = (BSONContent*)parser->eval(obj);

				int* test = *content;

				TEST_ASSERT(test != NULL);
				TEST_ASSERT(*test == 35);
				*/
			delete parser;

		}

		void testInsertWithStringId()
		{
			cout << "\ntestInsertWithStringId" << endl;
			BSONObj obj;
			std::string* id = uuid();
			obj.add("_id", *id);
			obj.add("name", "cross");
			delete id;
			BSONObj* res = controller->insert("dbtest", "sp1.customer", &obj);
			if (res != NULL)
			{
				delete res;
			}
		}

		void testInsertWithCharId()
		{
			cout << "\ntestInsertWithCharId" << endl;
			BSONObj obj;
			std::string* id = uuid();
			obj.add("_id", id->c_str());
			obj.add("name", "cross");
			delete id;
			BSONObj* res = controller->insert("dbtest", "sp1.customer", &obj);
			if (res != NULL)
			{
				delete res;
			}
		}

		void testInsertComplexBSON() {
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

			controller->insert("dbtest", "sp1.customercomplex", &obj);

			std::vector<BSONObj*>* array = controller->find("dbtest", "sp1.customercomplex", "*", "$'int' == 1");
			TEST_ASSERT(array->size() == 1);
			if (array->size() == 1) {
				BSONObj* res = *array->begin();
				TEST_ASSERT(res != NULL);
				TEST_ASSERT(res->has("_id"));
				TEST_ASSERT(res->has("int"));
				if (res->has("int")) {
					cout << "\n\nint value: " << res->getInt("int") << endl << endl;
					TEST_ASSERT(res->getInt("int") == 1);
				}
				TEST_ASSERT(res->has("double"));
				if (res->has("double")) {
					TEST_ASSERT(res->getDouble("double") == 1.1);
				}
				TEST_ASSERT(res->getBSON("inner") != NULL);
				BSONObj* innerRes = res->getBSON("inner");
				TEST_ASSERT(innerRes != NULL);
				TEST_ASSERT(innerRes->has("char"));
				if (innerRes->has("char")) {
					TEST_ASSERT(innerRes->getString("char").compare("testInner") == 0);
				}
				TEST_ASSERT(innerRes->has("int"));
				if (innerRes->has("int")) {
					cout << "\n\ninner int value: " << innerRes->getInt("int") << endl << endl;
					TEST_ASSERT(innerRes->getInt("int") == 200000);
				}
				delete res;
			}
			delete array;
		}

		void testMassiveInsert()
		{
			cout << "\ntestMassiveInsert" << endl;
			int inserts = 1000;
			std::auto_ptr<Logger> log(getLogger(NULL));

			log->startTimeRecord();

			FileOutputStream fos("temp.txt", "wb+");

			for (int x = 0; x < inserts; x++)
			{
				BSONObj* obj = new BSONObj();
				obj->add("name", "John");
				char temp[700];
				memset(temp, 0, 699);
				memset(temp, 'a', 700);
				obj->add("content", std::string(temp));
				obj->add("last", std::string("Smith"));
				testInsert(obj);

				int test = rand() % 10;
				if (test > 0)
				{
					__ids.push_back(new std::string(obj->getString("_id").c_str()));
					fos.writeString(obj->getString("_id"));
				}
				if ((x % 1000000) == 0)
				{

					cout<< "inserts " << x << endl;
				}
				delete obj;
			}
			fos.close();

			log->stopTimeRecord();

			int secs = log->recordedTime().totalSecs();

			cout<< "inserts " << inserts << ", secs: " << secs << endl;

			if (secs > 0)
			{
				// If throughtput is too small fail
				TEST_ASSERT((inserts / secs) > 10000);
				cout << "\nThroughput: " << (inserts / secs) << " ops." << endl;
				cout << "\n------------------------------------------------------------" << endl;
			}
		}

		void testAntlrParser() {
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

		void testFinds()
		{
			cout << "\ntestFinds" << endl;

			std::auto_ptr<Logger> log(getLogger(NULL));

			log->startTimeRecord();

			for (std::vector<string*>::iterator i = __ids.begin(); i != __ids.end(); i++)
			{
				string* id = *i;

				std::string filter = format("$\"_id\" == \"%s\"", id->c_str());

				BSONObj* res = controller->findFirst("dbtest", "sp1.customer", "*", filter.c_str());
				TEST_ASSERT(res != NULL);
				if (res == NULL) {
					TEST_FAIL("res is null");
					return;
				}
				std::string id2 = res->getString("_id");
				if (id2.compare(*id) != 0)
				{
					TEST_FAIL("id not found");
					return;
				}
				delete res;
			}

			log->stopTimeRecord();

			int secs = log->recordedTime().totalSecs();

			if (secs > 0)
			{
				TEST_ASSERT((__ids.size() / secs) > 30);
				cout << "\nThroughput: " << (__ids.size() / secs) << " ops." << endl;
				cout << "\n------------------------------------------------------------" << endl;
			}
		}

		void testFindPartial() {
			controller->dropNamespace("testdb", "partial");

			std::string record("{ 'name': 'John', 'lastName': 'Smith', 'address': { 'phone': '555-12345', 'type': 'home'} }");

			BSONObj* obj = BSONParser::parse(record.c_str());

			controller->insert("testdb", "partial", obj);

			std::vector<BSONObj*>* result = controller->find("testdb", "partial", "*", "$\"name\", $\"lastName\"");

			TEST_ASSERT(result != NULL);
			if (result != NULL) {
				BSONObj testObj;
				testObj.add("name", "John");
				testObj.add("lastName", "Smith");
				TEST_ASSERT(result->size() == 1);
				if (result->size() > 0) {
					BSONObj* test = *result->begin();

					TEST_ASSERT(*test == testObj);

					delete test;
				}
				delete result;
			}

			delete obj;
		}

		void testFindsFilterErrors() {
			// This will check for several filters with parsing errors
			cout << "\ntestFindsFilterErrors" << endl;


			std::string filter("a x n"); // bad constants
			try {
				controller->find("dbtest", "sp1.customer", "*", filter.c_str());
				TEST_FAIL("An error should occur");
			} catch (ParseException& e) {
				cout << "\nException --> " << e.what() << endl;
			}
		}

		void testFindsByFilter()
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
			std::vector<BSONObj*>* found = controller->find("dbtest", "find.filter", "*", "$\"lastName\" == \"Crossley\"");
			TEST_ASSERT(found->size() == 5); 
			delete found;
			delete filter;

			found = controller->find("dbtest", "find.filter", "*", "");
			TEST_ASSERT(found->size() == 8); 
			delete found;

			found = controller->find("dbtest", "find.filter", "*", "$\"name\": \"Juan\"");
			TEST_ASSERT(found->size() == 7); 
			delete found;

			found = controller->find("dbtest", "find.filter", "*", "$'name' == 'Juan' and $'lastName' == 'Smith'");
			TEST_ASSERT(found->size() == 1); 
			delete found;

			found = controller->find("dbtest", "find.filter", "*", "$'name' == 'Juan' and $'lastName' == 'Last'");
			TEST_ASSERT(found->size() == 1); 
			delete found;
		}

		void testFindsByTextFilter()
		{
			cout << "\ntestFindsByTextFilter" << endl;
			// Insert some data
			//
			controller->dropNamespace("dbtest", "find.filter2");
			controller->insert("dbtest", "find.filter2", BSONParser::parse("{name: 'Juan', lastName:'Crossley', age: 38}"));
			controller->insert("dbtest", "find.filter2", BSONParser::parse("{name: 'Pepe', lastName:'Crossley', age: 15}"));
			controller->insert("dbtest", "find.filter2", BSONParser::parse("{name: 'Juan', lastName:'Smith', age: 45}"));
			controller->insert("dbtest", "find.filter2", BSONParser::parse("{name: 'Juan', lastName:'Clark', age: 38}"));

			std::string filter = "$'age' == 45";
			std::vector<BSONObj*>* found = controller->find("dbtest", "find.filter2","*", filter.c_str());
			TEST_ASSERT(found->size() == 1); 
			std::string name = found->at(0)->getString("lastName");
			TEST_ASSERT(name.compare("Smith") == 0);

			filter = "";
			delete found;
			found = controller->find("dbtest", "find.filter2","*", filter.c_str());
			TEST_ASSERT(found->size() == 4); 

			filter = "$'age' == 38";
			delete found;
			found = controller->find("dbtest", "find.filter2","*", filter.c_str());
			TEST_ASSERT(found->size() == 2); 
			name = found->at(0)->getString("lastName");
			TEST_ASSERT(name.compare("Crossley") == 0);
			name = found->at(1)->getString("lastName");
			TEST_ASSERT(name.compare("Clark") == 0);
			delete found;
		}

		void testFindPrevious()
		{
			cout << "\ntestFindPrevious" << endl;
			std::auto_ptr<Logger> log(getLogger(NULL));

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
					TEST_FAIL("Looking for a previous id does not returned any match");
					break;
				}
				else
				{
					std::string id2 = res->getString("_id");
					//        cout << "\nLooking for: " << *id << endl;
					//        cout << "\nFound        " << *id2 << endl;
					if (id2.compare(*id) != 0)
					{
						TEST_FAIL("findFirst returned an incorrect result");
					}
					delete res;
				}
			}

			log->stopTimeRecord();

			int secs = log->recordedTime().totalSecs();

			if (secs > 0)
			{
				TEST_ASSERT((ids.size() / secs) > 30);
				cout << "\nThroughput: " << (ids.size() / secs) << " ops." << endl;
				cout << "\n------------------------------------------------------------" << endl;
			}
		}

		void testIndex(std::vector<std::string> ids)
		{
			std::set<std::string> keys;
			keys.insert("_id");
			std::auto_ptr<BPlusIndex> tree(new BPlusIndex(keys));

			std::auto_ptr<Logger> log(getLogger(NULL));

			log->startTimeRecord();
			// Inserting
			int x = 0;
			for (std::vector<std::string>::iterator i = ids.begin(); i != ids.end(); i++)
			{
				BSONObj id;
				std::string sid = *i;
				id.add("_id", sid);
				tree->add(id, sid, 0, 0);
				//tree->debug();
				x++;
			}
			log->stopTimeRecord();
			DTime time = log->recordedTime();

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
				id.add("_id", guid);
				Index* index = tree->find(id);
				TEST_ASSERT_MSG(index != NULL, ("guid not found: " + guid).c_str());
				if (index != NULL) {
					BSONObj* key = index->key;
					TEST_ASSERT(key != NULL);
					TEST_ASSERT(key->getString("_id").compare(guid) == 0);
				}

				ids.erase(i);
			}
			log->stopTimeRecord();
			time = log->recordedTime();
		}

		void testSimpleIndex()
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

		void testComplexIndex()
		{
			cout << "\ntestComplexIndex" << endl;
			FileInputStream fis("guids.txt", "rb");
			std::vector<std::string> ids;
			while (!fis.eof())
			{
				std::string* s= fis.readString();
				ids.push_back(*s);
				delete s;
			}
			fis.close();
			testIndex(ids);
		}

		void testIndexFactory() {
			cout << "\ntestIndexFactory" << endl;

			IndexAlgorithm* index = IndexFactory::indexFactory.index("dbtest", "ns.a", "_id");
			TEST_ASSERT(index != NULL);

			// Let's check if the factory returns the same instance for the same key
			IndexAlgorithm* indexCompare = IndexFactory::indexFactory.index("dbtest", "ns.a", "_id");
			TEST_ASSERT(index == indexCompare);

			// Let's change the keys and test if a new IndexAlgorithm will be returned
			IndexAlgorithm* indexCompare2 = IndexFactory::indexFactory.index("dbtest", "ns.a", "key");
			TEST_ASSERT(index != indexCompare2);

			// Checking the contains method
			bool res = IndexFactory::indexFactory.containsIndex("dbtest", "ns.a", "_id");
			TEST_ASSERT(res);

			bool res2 = IndexFactory::indexFactory.containsIndex("dbtest", "ns.a", "nkey");
			TEST_ASSERT(!res2);
		}

		void testDropnamespace()
		{
			cout << "\ntestDropnamespace" << endl;
			BSONObj obj;
			obj.add("name", "Test");

			BSONObj* res = controller->insert("dbtest", "ns.drop", &obj);

			bool result = controller->dropNamespace("dbtest", "ns.drop");
			TEST_ASSERT(result);

			std::vector<BSONObj*>* finds = controller->find("dbtest", "ns.drop", "*", "");

			TEST_ASSERT(finds->size() == 0);

			delete finds;
			delete res;
		}

		void testErrorHandling() {
			// Test errors at filter expressions
			//
			// FilterParser* parser = FilterParser::parse("A = B");
			// TEST_ASSERT(parser == NULL);
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
		controller = new DBController();
		controller->initialize();
		ts.add(auto_ptr<Test::Suite>(new TestDBSuite));
		//        ts.add(auto_ptr<Test::Suite>(new CompareTestSuite));
		//        ts.add(auto_ptr<Test::Suite>(new ThrowTestSuite));

		// Run the tests
		//
		auto_ptr<Test::Output> output(cmdline(argc, argv));
		ts.run(*output, true);

		Test::HtmlOutput* const html = dynamic_cast<Test::HtmlOutput*>(output.get());
		if (html)
			html->generate(cout, true, "MyTest");
		controller->shutdown();
	}
	catch (...)
	{
		cout << "\nunexpected exception encountered\n";
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
