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
#include <gtest/gtest.h>
#include <limits.h>

using namespace std;

TEST(testBSON, testBSON)
{
	cout << "testBSON" << endl;
	BSONObj* obj = new BSONObj();
	// Add in
	obj->add("int", 1);
	obj->add("string", (const char*)"test");
	obj->add("string2", (const char*)"t");
	obj->add("long", (__int64) 10000000000L);
	obj->add("double", 1.1);

	BSONObj rel;
	rel.add("innertext", (char*)"inner text");
	obj->add("rel1", rel);

	BSONArrayObj array;
	BSONObj b1;
	b1.add("b1", "test");
	array.add(b1);
	BSONObj b2;
	b2.add("b1", "test2");
	array.add(b2);
	obj->add("array", array);

	EXPECT_TRUE(obj->has("int"));
	EXPECT_TRUE(obj->getInt("int") == 1);

	EXPECT_TRUE(strcmp(obj->getString("string").c_str(), "test") == 0);
	EXPECT_TRUE(obj->getDJString("string").compare(djondb::string("test", 4)) == 0);
	EXPECT_TRUE(obj->getDJString("string2").compare(djondb::string("e", 1)) != 0);

	EXPECT_TRUE(obj->has("long"));
	cout << "long: " << obj->getLong("long") << endl;
	EXPECT_TRUE(obj->getLong("long") == 10000000000L);

	EXPECT_TRUE(obj->has("double"));
	EXPECT_TRUE(obj->getDouble("double") == 1.1);

	EXPECT_TRUE(obj->has("rel1"));
	EXPECT_TRUE(strcmp(obj->getBSON("rel1")->getString("innertext").c_str(), "inner text") == 0);

	EXPECT_TRUE(obj->has("array"));
	BSONArrayObj* arrayR = obj->getBSONArray("array");
	EXPECT_TRUE(arrayR != NULL);
	EXPECT_TRUE(arrayR->length() == 2);

	BSONObj* el1 = arrayR->get(0);
	EXPECT_TRUE(el1 != NULL);

	BSONObj* el2 = arrayR->get(1);
	EXPECT_TRUE(el2 != NULL);

	// test a non existant attribute
	obj->getLong("xx");
	EXPECT_THROW("The getLong should throw an exception", BSONException);

	obj->getString("xxx");
	EXPECT_THROW("The getString should throw an exception", BSONException);
	delete obj;
}

TEST(testBSON, testEquals) {
	cout << "\ntestEquals" << endl;
	BSONObj obj1;
	obj1.add("int", 1);
	obj1.add("double", 1.2);
	obj1.add("string", (char*)"Test");

	BSONObj obj2;
	obj2.add("int", 1);
	obj2.add("double", 1.2);
	obj2.add("string", (char*)"Test");

	EXPECT_TRUE(obj1 == obj2);

	obj2.add("string2", (char*)"Test");
	EXPECT_TRUE(obj1 != obj2);
	obj1.add("string2", (char*)"Test");
	EXPECT_TRUE(obj1 == obj2);

	obj1.add("test", 1);
	obj2.add("other", 2);
	EXPECT_TRUE(obj1 != obj2);
}
TEST(testBSON, testBigBSON)
{
	cout << "testBigBSON" << endl;
	BSONObj* obj = new BSONObj();

	int chars = 1000;
	// Add in
	obj->add("int", 1);
	obj->add("long", (__int64)LONG_MAX);
	obj->add("long long", (__int64)LLONG_MAX);
	char* temp = (char*)malloc(chars+1);
	memset(temp, 0, chars+1);
	memset(temp, 'a', chars);
	obj->add("char*", temp);

	BSONObj rel;
	rel.add("innertext", temp);
	obj->add("rel1", rel);

	char* json = obj->toChar();

	BSONObj* obj2 = BSONParser::parse(json);
	free(json);

	EXPECT_TRUE(obj->has("int"));
	EXPECT_TRUE(obj->getInt("int") == 1);

	EXPECT_TRUE(obj->has("long"));
	EXPECT_TRUE(obj->getLong("long") == LONG_MAX);

	EXPECT_TRUE(obj->has("long long"));
	EXPECT_TRUE(obj->getLong("long long") == LLONG_MAX);

	EXPECT_TRUE(strcmp(obj->getString("char*").c_str(), temp) == 0);

	EXPECT_TRUE(obj->has("rel1"));
	EXPECT_TRUE(strcmp(obj->getBSON("rel1")->getString("innertext").c_str(), temp) == 0);

	delete obj;
	delete obj2;
	free(temp);
}

TEST(testBSON, testCopyBSON)
{
	cout << "testCopyBSON" << endl;

	BSONObj* objOrig = new BSONObj();
	// Add in
	objOrig->add("int", 1);
	objOrig->add("string", (char*)"test");
	objOrig->add("long", (__int64)1L);
	objOrig->add("double", 1.1);

	BSONObj rel;
	rel.add("innertext", (char*)"inner text");
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

	EXPECT_TRUE(obj->has("int"));
	EXPECT_TRUE(obj->getInt("int") == 1);

	EXPECT_TRUE(strcmp(obj->getString("string").c_str(), "test") == 0);

	EXPECT_TRUE(obj->has("long"));
	EXPECT_TRUE(obj->getLong("long") == 1L);

	EXPECT_TRUE(obj->has("double"));
	EXPECT_TRUE(obj->getDouble("double") == 1.1);

	BSONObj* temp = obj->getBSON("rel1");
	EXPECT_TRUE(temp != NULL);
	EXPECT_TRUE(strcmp(obj->getBSON("rel1")->getString("innertext").c_str(), "inner text") == 0);

	EXPECT_TRUE(obj->getBSONArray("array") != NULL);
	BSONArrayObj* arrayR = obj->getBSONArray("array");
	EXPECT_TRUE(arrayR != NULL);
	EXPECT_TRUE(arrayR->length() == 2);

	BSONObj* el1 = arrayR->get(0);
	EXPECT_TRUE(el1 != NULL);

	BSONObj* el2 = arrayR->get(1);
	EXPECT_TRUE(el2 != NULL);
	delete obj;
}

TEST(testBSON, testToChar)
{
	cout << "testToChar" << endl;

	BSONObj obj;
	obj.add("int", 1);
	obj.add("string", (char*)"test");
	obj.add("char*", (char*)"char*");
	obj.add("long", (__int64)1L);
	obj.add("double", 1.1);

	char* json = obj.toChar();
	int res = strcmp(json, "{ \"char*\" : \"char*\", \"double\" : 1.100000, \"int\" : 1, \"long\" : 1, \"string\" : \"test\"}");
	EXPECT_TRUE(res == 0);
	if (res != 0) {
		cout << "\nResult: " << json << endl;
	}

	free(json);

	BSONObj inner;
	inner.add("int", 1);
	inner.add("string", (char*)"test");
	inner.add("char*", (char*)"char*");
	inner.add("long", (__int64)1L);
	inner.add("double", 1.1);
	obj.add("inner", inner);

	json = obj.toChar();
	cout << "\nResult: " << json << endl;
	free(json);
}

TEST(testBSON, testParserSimple)
{
	cout << "testParserSimple" << endl;

	BSONObj* testEmpty = BSONParser::parse("{}");
	EXPECT_TRUE(testEmpty->length() == 0);

	BSONObj* obj = BSONParser::parse("{age: 1, name: 'John:test\\'test2\\'', salary: 3500.25, lnumber: 100000000000, values: {}, other: 1}");
	EXPECT_TRUE(obj->has("age"));
	EXPECT_TRUE(obj->getInt("age") == 1);
	EXPECT_TRUE(strcmp(obj->getString("name").c_str(), "John:test\\'test2\\'") == 0);

	EXPECT_TRUE(obj->has("salary"));
	EXPECT_TRUE(obj->getDouble("salary") == 3500.25);

	EXPECT_TRUE(obj->has("lnumber"));
	EXPECT_TRUE(obj->getLong("lnumber") == 100000000000);

	EXPECT_TRUE(obj->has("values"));

	EXPECT_TRUE(obj->has("other"));
	EXPECT_TRUE(obj->getInt("other") == 1);

	delete obj;
	delete testEmpty;
}

TEST(testBSON, testParserTrivial)
{
	cout << "testParserTrivial" << endl;

	BSONObj* obj = BSONParser::parse("{age: '1'}");
	EXPECT_TRUE(obj->has("age"));
	EXPECT_TRUE(strcmp(obj->getString("age").c_str(), "1") == 0);

	delete obj;

	BSONObj* obj2 = BSONParser::parse("{\"type\":\"2\",\"category\":\"1\",\"title\":\"test\",\"price\":\"asdf\",\"place\":\"asdf\",\"description\":\"asdf\"}");
	EXPECT_TRUE(obj2->has("type"));
	delete obj2;
}

TEST(testBSON, testParserRelation)
{
	cout << "testParserRelation" << endl;

	BSONObj* obj = BSONParser::parse("{age: 1, name: 'John', rel1: {innertext: 'inner text', salary: 150000, rent: 10000}}");
	EXPECT_TRUE(obj->has("age"));
	EXPECT_TRUE(obj->getInt("age") == 1);
	EXPECT_TRUE(obj->has("name"));
	EXPECT_TRUE(strcmp(obj->getString("name").c_str(), "John") == 0);

	__int32 salary = *obj->getXpath("rel1.salary");
	EXPECT_TRUE(salary == 150000);
	__int32 rent = *obj->getXpath("rel1.rent");
	EXPECT_TRUE(rent == 10000);

	EXPECT_TRUE(obj->getBSON("rel1") != NULL);
	EXPECT_TRUE(strcmp(obj->getBSON("rel1")->getString("innertext").c_str(), "inner text") == 0);

	delete obj;
}

TEST(testBSON, testParserArray)
{
	cout << "testParserArray" << endl;

	BSONArrayObj* array = BSONParser::parseArray("[{age: 1, name: 'John', salary: 3500.25, rel1: {innertext: 'inner text'}}, {age: 2, name: 'John2', salary: 23500.25, rel1: {innertext: 'inner text2'}}]");
	EXPECT_TRUE(array != NULL);
	EXPECT_TRUE(array->length() == 2);

	BSONObj* obj = array->get(0);
	EXPECT_TRUE(obj != NULL);
	EXPECT_TRUE(obj->has("age"));
	EXPECT_TRUE(obj->getInt("age") == 1);
	EXPECT_TRUE(obj->has("name"));
	EXPECT_TRUE(strcmp(obj->getString("name").c_str(), "John") == 0);

	EXPECT_TRUE(obj->has("salary"));
	EXPECT_TRUE(obj->getDouble("salary") == 3500.25);

	EXPECT_TRUE(obj->getBSON("rel1") != NULL);
	EXPECT_TRUE(strcmp(obj->getBSON("rel1")->getString("innertext").c_str(), "inner text") == 0);

	BSONArrayObj::iterator i = array->begin();
	EXPECT_TRUE(i != array->end());
	delete array;
}


TEST(testBSON, testParserCollection)
{
	cout << "testParserCollection" << endl;

	BSONObj* obj = BSONParser::parse("{age: 1, name: 'John', salary: 3500.25, rel1: [{innertext: 'inner text'}, {innertext: 'inner text'}, {innertext: 'inner text'}, {innertext: 'inner text'} ] }");
	EXPECT_TRUE(obj->has("age"));
	EXPECT_TRUE(obj->getInt("age") == 1);
	EXPECT_TRUE(obj->has("name"));
	EXPECT_TRUE(strcmp(obj->getString("name").c_str(), "John") == 0);

	EXPECT_TRUE(obj->has("salary"));
	EXPECT_TRUE(obj->getDouble("salary") == 3500.25);

	EXPECT_TRUE(obj->getBSONArray("rel1") != NULL);
	EXPECT_TRUE(obj->getBSONArray("rel1")->length() == 4);
	EXPECT_TRUE(obj->getBSONArray("rel1")->get(0)->has("innertext"));
	EXPECT_TRUE(strcmp(obj->getBSONArray("rel1")->get(0)->getString("innertext").c_str(), "inner text") == 0);

	delete obj;
}

TEST(testBSON, testParserDoubleRelation)
{
	cout << "testParserDoubleRelation" << endl;

	BSONObj* obj = BSONParser::parse("{age: 1, name: 'John', salary: 3500.25, rel1: {innertext: 'inner text', innerrel1: {innertext:'text2'}}}");
	EXPECT_TRUE(obj->has("age"));
	EXPECT_TRUE(obj->getInt("age") == 1);
	EXPECT_TRUE(obj->has("name"));
	EXPECT_TRUE(strcmp(obj->getString("name").c_str(), "John") == 0);

	EXPECT_TRUE(obj->has("salary"));
	EXPECT_TRUE(obj->getDouble("salary") == 3500.25);

	EXPECT_TRUE(obj->getBSON("rel1") != NULL);
	EXPECT_TRUE(strcmp(obj->getBSON("rel1")->getString("innertext").c_str(), "inner text") == 0);

	EXPECT_TRUE(obj->getBSON("rel1")->getBSON("innerrel1") != NULL);
	EXPECT_TRUE(strcmp(obj->getBSON("rel1")->getBSON("innerrel1")->getString("innertext").c_str(), "text2") == 0);
	delete obj;
}

TEST(testBSON, testComparison) {
	cout << "testComparison" << endl;

	// This method will test comparison from contents of two BSONObj

	BSONObj* obj1 = BSONParser::parse("{int: 1, double: 2, text: 'name'}");
	BSONObj* obj2 = BSONParser::parse("{int: 1, double: 2, text: 'name'}");

	EXPECT_TRUE(*obj1->getContent("int") == *obj2->getContent("int"));
	EXPECT_TRUE(*obj1->getContent("double") == *obj2->getContent("double"));
	EXPECT_TRUE(*obj1->getContent("text") == *obj2->getContent("text"));
	EXPECT_TRUE(!(*obj1->getContent("text") == *obj2->getContent("double")));

	delete obj1;
	delete obj2;
}

TEST(testBSON, testAutocasting) {
	cout << "testAutocasting" << endl;

	BSONObj o;
	o.add("long", (__int64)1L);
	o.add("double", 2.0);
	o.add("int", 1);
	o.add("char*", (char*)"Test");

	BSONObj inner;
	inner.add("text", "text");
	o.add("inner", inner);

	EXPECT_TRUE(o.getContent("long") != NULL);
	EXPECT_TRUE((__int64)*o.getContent("long") == 1L);

	EXPECT_TRUE(o.getContent("double") != NULL);
	EXPECT_TRUE((double)*o.getContent("double") == 2.0);

	EXPECT_TRUE(o.getContent("int") != NULL);
	EXPECT_TRUE((int)*o.getContent("int") == 1);

	EXPECT_TRUE(o.getContent("inner") != NULL);
	BSONObj* obj = *o.getContent("inner");
	EXPECT_TRUE(strcmp(obj->getString("text").c_str(), "text") == 0);
}

TEST(testBSON, testXPath) {
	cout << "testXPath" << endl;

	BSONObj* obj1 = BSONParser::parse("{ name: 'John', age: 35, one: { data: 1 }, children: [ { name: 'Joshua', age: 15}, { name: 'Mary', age: 30}] }");

	__int32 age = *obj1->getXpath("age");

	EXPECT_TRUE(age == 35);

	BSONArrayObj* children = *obj1->getXpath("children");
	EXPECT_TRUE(children->length() == 2);

	__int32 data = *obj1->getXpath("one.data");
	EXPECT_TRUE(data == 1);

	delete obj1;
}

TEST(testBSON, testBSONUtil) {
	cout << "\ntestBSONUtil" << endl;

	char* selectsimple = "$\"test\"";
	std::set<std::string> result = bson_splitSelect("$\"test\"");
	EXPECT_TRUE(result.size() == 1);
	EXPECT_TRUE(result.find("test") != result.end());

	selectsimple = "$\"test\", $\"test2\"";
	result = bson_splitSelect("$\"test\"");
	EXPECT_TRUE(result.size() == 1);
	EXPECT_TRUE(result.find("test") != result.end());
	result = bson_splitSelect("$\"test2\"");
	EXPECT_TRUE(result.size() == 1);
	EXPECT_TRUE(result.find("test2") != result.end());

	selectsimple = "$\"test\", $\"test2.inner1\"";
	result = bson_splitSelect("$\"test\"");
	EXPECT_TRUE(result.size() == 1);
	EXPECT_TRUE(result.find("test") != result.end());
	result = bson_splitSelect("$\"test\", $\"test2\"");
	EXPECT_TRUE(result.size() == 2);
	EXPECT_TRUE(result.find("test") != result.end());
	EXPECT_TRUE(result.find("test2") != result.end());

	selectsimple = "$\"test2.inner\"";
	char* subresult = bson_subselect(selectsimple, "test2");
	char* expected = "$\"inner\"";
	EXPECT_TRUE(strcmp(subresult, expected) == 0);

	selectsimple = "$\"test1\", $\"test2.inner\", $\"test1.testinner\", $\"test2.inner2\", $\"test2.inner2.testii\"";
	subresult = bson_subselect(selectsimple, "test2");
	expected = "$\"inner\", $\"inner2\", $\"inner2.testii\"";
	EXPECT_TRUE(strcmp(subresult, expected) == 0);
}

TEST(testBSON, testBSONSelect) {
	cout << "\ntestBSONSelect()" << endl;

	char* selectsimple;
	BSONObj* obj;
	BSONObj* expected;
	BSONObj* result;

	obj = BSONParser::parse("{ name: 'John', age: 35, one: { data: 1 }, children: [ { name: 'Joshua', age: 15}, { name: 'Mary', age: 30}] }");
	selectsimple = "$\"name\", $\"one.data\"";
	result = obj->select(const_cast<const char*>(selectsimple));
	expected = BSONParser::parse("{ name: 'John', one: { data: 1 }}");
	EXPECT_TRUE(*result == *expected);
	delete expected;
	delete result;

	selectsimple = "*";
	result = obj->select(const_cast<const char*>(selectsimple));
	expected = new BSONObj(*obj);
	EXPECT_TRUE(*result == *expected);

	delete obj;
}
