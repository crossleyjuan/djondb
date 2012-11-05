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
#include "bson.h"
#include "bsonutil.h"
#include <string>
#include <string.h>
#include <cpptest.h>
#include <limits.h>

using namespace std;

class TestBSONSuite : public Test::Suite
{
	public:
		TestBSONSuite()
		{
			TEST_ADD(TestBSONSuite::testBigBSON);
			TEST_ADD(TestBSONSuite::testBSON);
			TEST_ADD(TestBSONSuite::testEquals);
			TEST_ADD(TestBSONSuite::testCopyBSON);
			TEST_ADD(TestBSONSuite::testParserTrivial);
			TEST_ADD(TestBSONSuite::testParserSimple);
			TEST_ADD(TestBSONSuite::testToChar);
			TEST_ADD(TestBSONSuite::testParserRelation);
			TEST_ADD(TestBSONSuite::testParserCollection);
			TEST_ADD(TestBSONSuite::testParserArray);
			TEST_ADD(TestBSONSuite::testParserDoubleRelation);
			TEST_ADD(TestBSONSuite::testComparison);
			TEST_ADD(TestBSONSuite::testAutocasting);
			TEST_ADD(TestBSONSuite::testXPath);
			TEST_ADD(TestBSONSuite::testBSONUtil);
			TEST_ADD(TestBSONSuite::testBSONSelect);
		}

	private:
		void testBSON()
		{
			cout << "testBSON" << endl;
			BSONObj* obj = new BSONObj();
			// Add in
			obj->add("int", 1);
			obj->add("string", std::string("test"));
			obj->add("long", (long long) 10000000000L);
			obj->add("double", 1.1);

			BSONObj rel;
			rel.add("innertext", std::string("inner text"));
			obj->add("rel1", rel);

			BSONArrayObj array;
			BSONObj b1;
			b1.add("b1", "test");
			array.add(b1);
			BSONObj b2;
			b2.add("b1", "test2");
			array.add(b2);
			obj->add("array", array);

			TEST_ASSERT(obj->has("int"));
			TEST_ASSERT(obj->getInt("int") == 1);

			TEST_ASSERT(obj->getString("string").compare("test") == 0);

			TEST_ASSERT(obj->has("long"));
			cout << "long: " << obj->getLong64("long") << endl;
			TEST_ASSERT(obj->getLong64("long") == 10000000000L);

			TEST_ASSERT(obj->has("double"));
			TEST_ASSERT(obj->getDouble("double") == 1.1);

			TEST_ASSERT(obj->has("rel1"));
			TEST_ASSERT(obj->getBSON("rel1")->getString("innertext").compare("inner text") == 0);
			
			TEST_ASSERT(obj->has("array"));
			BSONArrayObj* arrayR = obj->getBSONArray("array");
			TEST_ASSERT(arrayR != NULL);
			TEST_ASSERT(arrayR->length() == 2);

			BSONObj* el1 = arrayR->get(0);
			TEST_ASSERT(el1 != NULL);
			
			BSONObj* el2 = arrayR->get(1);
			TEST_ASSERT(el2 != NULL);

			// test a non existant attribute
			try {
				obj->getLong("xx");
				TEST_FAIL("The getLong should throw an exception");
			} catch (BSONException e) {
			}

			try {
				obj->getString("xxx");
				TEST_FAIL("The getString should throw an exception");
			} catch (BSONException e) {
			}
			delete obj;
		}

		void testEquals() {
			cout << "\ntestEquals" << endl;
			BSONObj obj1;
			obj1.add("int", 1);
			obj1.add("double", 1.2);
			obj1.add("string", std::string("Test"));

			BSONObj obj2;
			obj2.add("int", 1);
			obj2.add("double", 1.2);
			obj2.add("string", std::string("Test"));

			TEST_ASSERT(obj1 == obj2);

			obj2.add("string2", std::string("Test"));
			TEST_ASSERT(obj1 != obj2);
			obj1.add("string2", std::string("Test"));
			TEST_ASSERT(obj1 == obj2);

			obj1.add("test", 1);
			obj2.add("other", 2);
			TEST_ASSERT(obj1 != obj2);
		}
		void testBigBSON()
		{
			cout << "testBigBSON" << endl;
			BSONObj* obj = new BSONObj();

			int chars = 1000;
			// Add in
			obj->add("int", 1);
			obj->add("long", LONG_MAX);
			obj->add("long long", LLONG_MAX);
			obj->add("string", std::string(chars, 'a'));
			char* temp = (char*)malloc(chars+1);
			memset(temp, 0, chars+1);
			memset(temp, 'a', chars);
			obj->add("char*", temp);

			BSONObj rel;
			rel.add("innertext", temp);
			obj->add("rel1", rel);
			free(temp);

			char* json = obj->toChar();

			BSONObj* obj2 = BSONParser::parse(json);
			free(json);

			TEST_ASSERT(obj->has("int"));
			TEST_ASSERT(obj->getInt("int") == 1);

			TEST_ASSERT(obj->has("long"));
			TEST_ASSERT(obj->getLong("long") == LONG_MAX);

			TEST_ASSERT(obj->has("long long"));
			TEST_ASSERT(obj->getLong64("long long") == LLONG_MAX);

			TEST_ASSERT(obj->getString("string").compare(std::string(chars, 'a')) == 0);

			TEST_ASSERT(obj->has("rel1"));
			TEST_ASSERT(obj->getBSON("rel1")->getString("innertext").compare(std::string(chars, 'a')) == 0);

			delete obj;
			delete obj2;
		}

		void testCopyBSON()
		{
			cout << "testCopyBSON" << endl;

			BSONObj* objOrig = new BSONObj();
			// Add in
			objOrig->add("int", 1);
			objOrig->add("string", std::string("test"));
			objOrig->add("long", 1L);
			objOrig->add("double", 1.1);

			BSONObj rel;
			rel.add("innertext", std::string("inner text"));
			objOrig->add("rel1", rel);

			BSONArrayObj array;
			BSONObj b1;
			b1.add("b1", "test");
			array.add(b1);
			BSONObj b2;
			b2.add("b1", "test2");
			array.add(b2);
			objOrig->add("array", array);

			BSONObj* obj = new BSONObj(*objOrig);
			delete objOrig;
			objOrig = NULL;

			TEST_ASSERT(obj->has("int"));
			TEST_ASSERT(obj->getInt("int") == 1);

			TEST_ASSERT(obj->getString("string").compare("test") == 0);

			TEST_ASSERT(obj->has("long"));
			TEST_ASSERT(obj->getLong("long") == 1L);

			TEST_ASSERT(obj->has("double"));
			TEST_ASSERT(obj->getDouble("double") == 1.1);

			BSONObj* temp = obj->getBSON("rel1");
			TEST_ASSERT(temp != NULL);
			TEST_ASSERT(obj->getBSON("rel1")->getString("innertext").compare("inner text") == 0);

			TEST_ASSERT(obj->getBSONArray("array") != NULL);
			BSONArrayObj* arrayR = obj->getBSONArray("array");
			TEST_ASSERT(arrayR != NULL);
			TEST_ASSERT(arrayR->length() == 2);

			BSONObj* el1 = arrayR->get(0);
			TEST_ASSERT(el1 != NULL);

			BSONObj* el2 = arrayR->get(1);
			TEST_ASSERT(el2 != NULL);
			delete obj;
		}

		void testToChar()
		{
			cout << "testToChar" << endl;

			BSONObj obj;
			obj.add("int", 1);
			obj.add("string", std::string("test"));
			obj.add("char*", (char*)"char*");
			obj.add("long", 1L);
			obj.add("double", 1.1);

			char* json = obj.toChar();
			int res = strcmp(json, "{ \"char*\" : \"char*\", \"double\" : 1.100000, \"int\" : 1, \"long\" : 1, \"string\" : \"test\"}");
			TEST_ASSERT(res == 0);
			if (res != 0) {
				cout << "\nResult: " << json << endl;
			}

			free(json);

			BSONObj inner;
			inner.add("int", 1);
			inner.add("string", std::string("test"));
			inner.add("char*", (char*)"char*");
			inner.add("long", 1L);
			inner.add("double", 1.1);
			obj.add("inner", inner);

			json = obj.toChar();
			cout << "\nResult: " << json << endl;
			free(json);
		}

		void testParserSimple()
		{
			cout << "testParserSimple" << endl;

			BSONObj* testEmpty = BSONParser::parse("{}");
			TEST_ASSERT(testEmpty->length() == 0);

			BSONObj* obj = BSONParser::parse("{age: 1, name: 'John:test\\'test2\\'', salary: 3500.25, lnumber: 100000000000}");
			TEST_ASSERT(obj->has("age"));
			TEST_ASSERT(obj->getInt("age") == 1);
			TEST_ASSERT(obj->getString("name").compare("John:test\\'test2\\'") == 0);

			TEST_ASSERT(obj->has("salary"));
			TEST_ASSERT(obj->getDouble("salary") == 3500.25);

			TEST_ASSERT(obj->has("lnumber"));
			TEST_ASSERT(obj->getLong64("lnumber") == 100000000000);

			delete obj;
			delete testEmpty;
		}

		void testParserTrivial()
		{
			cout << "testParserTrivial" << endl;

			BSONObj* obj = BSONParser::parse("{age: '1'}");
			TEST_ASSERT(obj->has("age"));
			TEST_ASSERT(obj->getString("age").compare("1") == 0);

			delete obj;

			BSONObj* obj2 = BSONParser::parse("{\"type\":\"2\",\"category\":\"1\",\"title\":\"test\",\"price\":\"asdf\",\"place\":\"asdf\",\"description\":\"asdf\"}");
			TEST_ASSERT(obj2->has("type"));
			delete obj2;
		}

		void testParserRelation()
		{
			cout << "testParserRelation" << endl;

			BSONObj* obj = BSONParser::parse("{age: 1, name: 'John', rel1: {innertext: 'inner text', salary: 150000, rent: 10000}}");
			TEST_ASSERT(obj->has("age"));
			TEST_ASSERT(obj->getInt("age") == 1);
			TEST_ASSERT(obj->has("name"));
			TEST_ASSERT(obj->getString("name").compare("John") == 0);

			int salary = obj->getXpath("rel1.salary");
			TEST_ASSERT(salary == 150000);
			int rent = obj->getXpath("rel1.rent");
			TEST_ASSERT(rent == 10000);

			TEST_ASSERT(obj->getBSON("rel1") != NULL);
			TEST_ASSERT(obj->getBSON("rel1")->getString("innertext").compare("inner text") == 0);

			delete obj;
		}

		void testParserArray()
		{
			cout << "testParserArray" << endl;

			BSONArrayObj* array = BSONParser::parseArray("[{age: 1, name: 'John', salary: 3500.25, rel1: {innertext: 'inner text'}}, {age: 2, name: 'John2', salary: 23500.25, rel1: {innertext: 'inner text2'}}]");
			TEST_ASSERT(array != NULL);
			TEST_ASSERT(array->length() == 2);

			BSONObj* obj = array->get(0);
			TEST_ASSERT(obj != NULL);
			TEST_ASSERT(obj->has("age"));
			TEST_ASSERT(obj->getInt("age") == 1);
			TEST_ASSERT(obj->has("name"));
			TEST_ASSERT(obj->getString("name").compare("John") == 0);

			TEST_ASSERT(obj->has("salary"));
			TEST_ASSERT(obj->getDouble("salary") == 3500.25);

			TEST_ASSERT(obj->getBSON("rel1") != NULL);
			TEST_ASSERT(obj->getBSON("rel1")->getString("innertext").compare("inner text") == 0);

			BSONArrayObj::iterator i = array->begin();
			TEST_ASSERT(i != array->end());
			delete array;
		}


		void testParserCollection()
		{
			cout << "testParserCollection" << endl;

			BSONObj* obj = BSONParser::parse("{age: 1, name: 'John', salary: 3500.25, rel1: [{innertext: 'inner text'}, {innertext: 'inner text'}, {innertext: 'inner text'}, {innertext: 'inner text'} ] }");
			TEST_ASSERT(obj->has("age"));
			TEST_ASSERT(obj->getInt("age") == 1);
			TEST_ASSERT(obj->has("name"));
			TEST_ASSERT(obj->getString("name").compare("John") == 0);

			TEST_ASSERT(obj->has("salary"));
			TEST_ASSERT(obj->getDouble("salary") == 3500.25);

			TEST_ASSERT(obj->getBSONArray("rel1") != NULL);
			TEST_ASSERT(obj->getBSONArray("rel1")->length() == 4);
			TEST_ASSERT(obj->getBSONArray("rel1")->get(0)->has("innertext"));
			TEST_ASSERT(obj->getBSONArray("rel1")->get(0)->getString("innertext").compare("inner text") == 0);

			delete obj;
		}

		void testParserDoubleRelation()
		{
			cout << "testParserDoubleRelation" << endl;

			BSONObj* obj = BSONParser::parse("{age: 1, name: 'John', salary: 3500.25, rel1: {innertext: 'inner text', innerrel1: {innertext:'text2'}}}");
			TEST_ASSERT(obj->has("age"));
			TEST_ASSERT(obj->getInt("age") == 1);
			TEST_ASSERT(obj->has("name"));
			TEST_ASSERT(obj->getString("name").compare("John") == 0);

			TEST_ASSERT(obj->has("salary"));
			TEST_ASSERT(obj->getDouble("salary") == 3500.25);

			TEST_ASSERT(obj->getBSON("rel1") != NULL);
			TEST_ASSERT(obj->getBSON("rel1")->getString("innertext").compare("inner text") == 0);

			TEST_ASSERT(obj->getBSON("rel1")->getBSON("innerrel1") != NULL);
			TEST_ASSERT(obj->getBSON("rel1")->getBSON("innerrel1")->getString("innertext").compare("text2") == 0);
			delete obj;
		}

		void testComparison() {
			cout << "testComparison" << endl;

			// This method will test comparison from contents of two BSONObj

			BSONObj* obj1 = BSONParser::parse("{int: 1, double: 2, text: 'name'}");
			BSONObj* obj2 = BSONParser::parse("{int: 1, double: 2, text: 'name'}");

			TEST_ASSERT(*obj1->getContent("int") == *obj2->getContent("int"));
			TEST_ASSERT(*obj1->getContent("double") == *obj2->getContent("double"));
			TEST_ASSERT(*obj1->getContent("text") == *obj2->getContent("text"));
			TEST_ASSERT(!(*obj1->getContent("text") == *obj2->getContent("double")));

			delete obj1;
			delete obj2;
		}

		void testAutocasting() {
			cout << "testAutocasting" << endl;

			BSONObj o;
			o.add("long", 1L);
			o.add("double", 2.0);
			o.add("int", 1);
			o.add("char*", (char*)"Test");
			o.add("string", (std::string)"String");

			BSONObj inner;
			inner.add("text", "text");
			o.add("inner", inner);

			TEST_ASSERT(o.getContent("long") != NULL);
			TEST_ASSERT((long)*o.getContent("long") == 1L);

			TEST_ASSERT(o.getContent("double") != NULL);
			TEST_ASSERT((double)*o.getContent("double") == 2.0);

			TEST_ASSERT(o.getContent("int") != NULL);
			TEST_ASSERT((int)*o.getContent("int") == 1);

			TEST_ASSERT(o.getContent("char*") != NULL);
			TEST_ASSERT(((std::string)*o.getContent("char*")).compare("Test") == 0);

			TEST_ASSERT(o.getContent("string") != NULL);
			TEST_ASSERT(((std::string)*o.getContent("string")).compare("String") == 0);

			TEST_ASSERT(o.getContent("inner") != NULL);
			BSONObj obj = *o.getContent("inner");
			TEST_ASSERT(obj.getString("text").compare("text") == 0);
		}

		void testXPath() {
			cout << "testXPath" << endl;

			BSONObj* obj1 = BSONParser::parse("{ name: 'John', age: 35, one: { data: 1 }, children: [ { name: 'Joshua', age: 15}, { name: 'Mary', age: 30}] }");

			int age = obj1->getXpath("age");

			TEST_ASSERT(age == 35);

			BSONArrayObj children = obj1->getXpath("children");
			TEST_ASSERT(children.length() == 2);

			int data = obj1->getXpath("one.data");
			TEST_ASSERT(data == 1);

			delete obj1;
		}

		void testBSONUtil() {
			cout << "\ntestBSONUtil" << endl;

			char* selectsimple = "$\"test\"";
			std::set<std::string> result = bson_splitSelect("$\"test\"");
			TEST_ASSERT(result.size() == 1);
			TEST_ASSERT(result.find("test") != result.end());

			selectsimple = "$\"test\", $\"test2\"";
			result = bson_splitSelect("$\"test\"");
			TEST_ASSERT(result.size() == 1);
			TEST_ASSERT(result.find("test") != result.end());
			result = bson_splitSelect("$\"test2\"");
			TEST_ASSERT(result.size() == 1);
			TEST_ASSERT(result.find("test2") != result.end());

			selectsimple = "$\"test\", $\"test2.inner1\"";
			result = bson_splitSelect("$\"test\"");
			TEST_ASSERT(result.size() == 1);
			TEST_ASSERT(result.find("test") != result.end());
			result = bson_splitSelect("$\"test\", $\"test2\"");
			TEST_ASSERT(result.size() == 2);
			TEST_ASSERT(result.find("test") != result.end());
			TEST_ASSERT(result.find("test2") != result.end());

			selectsimple = "$\"test2.inner\"";
			char* subresult = bson_subselect(selectsimple, "test2");
			char* expected = "$\"inner\"";
			TEST_ASSERT(strcmp(subresult, expected) == 0);

			selectsimple = "$\"test1\", $\"test2.inner\", $\"test1.testinner\", $\"test2.inner2\", $\"test2.inner2.testii\"";
			subresult = bson_subselect(selectsimple, "test2");
			expected = "$\"inner\", $\"inner2\", $\"inner2.testii\"";
			TEST_ASSERT(strcmp(subresult, expected) == 0);
		}

		void testBSONSelect() {
			cout << "\ntestBSONSelect()" << endl;

			char* selectsimple;
			BSONObj* obj;
			BSONObj* expected;
			BSONObj* result;

			obj = BSONParser::parse("{ name: 'John', age: 35, one: { data: 1 }, children: [ { name: 'Joshua', age: 15}, { name: 'Mary', age: 30}] }");
			selectsimple = "$\"name\", $\"one.data\"";
			result = obj->select(const_cast<const char*>(selectsimple));
			expected = BSONParser::parse("{ name: 'John', one: { data: 1 }}");
			TEST_ASSERT(*result == *expected);
			delete expected;
			delete result;

			selectsimple = "*";
			result = obj->select(const_cast<const char*>(selectsimple));
			expected = new BSONObj(*obj);
			TEST_ASSERT(*result == *expected);

			delete obj;
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
	cout << "usage: mytest [MODE]\n"
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
			cout << "invalid commandline argument: " << arg << endl;
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
		ts.add(auto_ptr<Test::Suite>(new TestBSONSuite));
		//        ts.add(auto_ptr<Test::Suite>(new CompareTestSuite));
		//        ts.add(auto_ptr<Test::Suite>(new ThrowTestSuite));

		// Run the tests
		//
		auto_ptr<Test::Output> output(cmdline(argc, argv));
		ts.run(*output, true);

		Test::HtmlOutput* const html = dynamic_cast<Test::HtmlOutput*>(output.get());
		if (html)
			html->generate(cout, true, "MyTest");
	}
	catch (...)
	{
		cout << "unexpected exception encountered\n";
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
