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
#include <fileinputstream.h>
#include <mmapinputstream.h>
#include <mmapinputoutputstream.h>
#include <fileoutputstream.h>
#include <fileinputoutputstream.h>
#include <memorystream.h>
#include "bsonoutputstream.h"
#include "bsoninputstream.h"
#include "bsonbufferedobj.h"
#include "bsonbufferedarrayobj.h"
#include "bson.h"
#include <string.h>
#include <memory>
#include <gtest/gtest.h>
#include <limits.h>

using namespace std;

TEST(testFilesystem, testFileStreams)
{
	FileOutputStream streamo("test.txt", "wb");
	char* test = (char*)malloc(200001);
	memset(test, 0, 200001);
	memset(test, 'a', 200000);
	streamo.writeChars("Hello World!", 12);
	streamo.writeShortInt(2);
	streamo.writeShortInt(127);
	streamo.writeInt(200000);
	streamo.writeLong(LONG_MAX);
	//streamo.writeChars(test, strlen(test));
	streamo.close();

	FileInputStream streami("test.txt", "rb");
	char* text = streami.readChars();
	EXPECT_TRUE(strcmp(text, "Hello World!") == 0);
	int i1 = streami.readShortInt();
	EXPECT_TRUE(i1 == 2);
	int si1 = streami.readShortInt();
	EXPECT_TRUE(si1 == 127);
	int i2 = streami.readInt();
	EXPECT_TRUE(i2 == 200000);
	long l = streami.readLong();
	EXPECT_TRUE(l == LONG_MAX);

	//char* tchar = streami.readChars();
	//EXPECT_TRUE(strcmp(test, tchar) == 0);
	streami.close();
	free(test);
}

TEST(testFilesystem, testMMap)
{
	cout << "\ntestMMap\n" << endl;
	FileOutputStream streamo("test.txt", "wb");
	char* test = (char*)malloc(200001);
	memset(test, 0, 200001);
	memset(test, 'a', 200000);
	streamo.writeChars("Hello World!", 12);
	streamo.writeShortInt(2);
	streamo.writeShortInt(127);
	streamo.writeInt(200000);
	streamo.writeLong(LONG_MAX);
	//streamo.writeChars(test, strlen(test));
	streamo.close();

	MMapInputStream streami("test.txt", 0);
	char* text = streami.readChars();
	EXPECT_TRUE(strcmp(text, "Hello World!") == 0);
	int i1 = streami.readShortInt();
	EXPECT_TRUE(i1 == 2);
	int si1 = streami.readShortInt();
	EXPECT_TRUE(si1 == 127);
	int i2 = streami.readInt();
	EXPECT_TRUE(i2 == 200000);
	long l = streami.readLong();
	EXPECT_TRUE(l == LONG_MAX);

	//char* tchar = streami.readChars();
	//EXPECT_TRUE(strcmp(test, tchar) == 0);
	streami.close();
	free(test);
}

TEST(testFilesystem, testMMapIO)
{
	cout << "\ntestMMapIO\n" << endl;
	char* fileName;
#ifndef WINDOWS
	fileName = "/tmp/testmmap.dat";
#else
	makeDir("C:\\temp");
	fileName = "C:\\temp\\testmmap.dat";
#endif
	MMapInputOutputStream streamo(fileName, 0, 4);
	streamo.writeChars("Hello World!", 12);
	streamo.writeShortInt(2);
	streamo.writeShortInt(127);
	streamo.writeInt(200000);
	streamo.writeLong(LONG_MAX);

	streamo.seek(0);

	char* text = streamo.readChars();
	EXPECT_TRUE(strcmp(text, "Hello World!") == 0);
	int i1 = streamo.readShortInt();
	EXPECT_TRUE(i1 == 2);
	int si1 = streamo.readShortInt();
	EXPECT_TRUE(si1 == 127);
	int i2 = streamo.readInt();
	EXPECT_TRUE(i2 == 200000);
	long l = streamo.readLong();
	EXPECT_TRUE(l == LONG_MAX);

	//char* tchar = streamo.readChars();
	//EXPECT_TRUE(strcmp(test, tchar) == 0);
	streamo.close();
}

TEST(testFilesystem, testMMapIOOffset)
{
	cout << "\ntestMMapIOOffset\n" << endl;
	long ps = pageSize();

	__int64 offset = 4 * ps;
	char* fileName;
#ifndef WINDOWS
	fileName = "/tmp/testmmap.dat";
#else
	makeDir("C:\\temp");
	fileName = "C:\\temp\\testmmap.dat";
#endif
	MMapInputOutputStream streamo(fileName, offset, 10);
	streamo.writeChars("Hello World!", 12);
	streamo.writeShortInt(2);
	streamo.writeShortInt(127);
	streamo.writeInt(200000);
	streamo.writeLong(LONG_MAX);

	streamo.seek(0);

	char* text = streamo.readChars();
	EXPECT_TRUE(strcmp(text, "Hello World!") == 0);
	int i1 = streamo.readShortInt();
	EXPECT_TRUE(i1 == 2);
	int si1 = streamo.readShortInt();
	EXPECT_TRUE(si1 == 127);
	int i2 = streamo.readInt();
	EXPECT_TRUE(i2 == 200000);
	long l = streamo.readLong();
	EXPECT_TRUE(l == LONG_MAX);

	//char* tchar = streamo.readChars();
	//EXPECT_TRUE(strcmp(test, tchar) == 0);
	streamo.close();
}

TEST(testFilesystem, testMemoryStream) {
	cout << "\ntestMemoryStream\n" << endl;
	MemoryStream ms(10);
	char* text = "test1234567890darn0987654321";
	ms.writeChars(text, strlen(text));

	ms.seek(0);
	char* res = ms.readChars();

	EXPECT_TRUE(strcmp(res, text) == 0);

	MemoryStream ms2;
	ms2.writeInt(1);
	ms2.writeInt(2);
	int pos = ms2.currentPos();
	EXPECT_TRUE(pos == sizeof(__int32) * 2);
	ms2.seek(4);
	pos = ms2.currentPos();
	EXPECT_TRUE(pos == sizeof(__int32));
	ms2.readInt();
	pos = ms2.currentPos();
	EXPECT_TRUE(pos == sizeof(__int32) * 2);


	// testing with buffer initializer
	char* c = ms2.toChars();
	MemoryStream ms3(c, ms2.size());
	ms3.seek(0);
	ms3.readInt();
	ms3.readInt();
	pos = ms3.currentPos();
	EXPECT_TRUE(pos == sizeof(__int32) * 2);
	ms3.seek(4);
	pos = ms3.currentPos();
	EXPECT_TRUE(pos == sizeof(__int32));
	ms3.readInt();
	pos = ms3.currentPos();
	EXPECT_TRUE(pos == sizeof(__int32) * 2);

	free(c);
	free(res);
}

TEST(testFilesystem, testFileInputOutputStreams)
{
	FileInputOutputStream streamo("test2.txt", "w+b");
	streamo.writeInt(INT_MAX);
	char* test = (char*)malloc(200001);
	memset(test, 0, 200001);
	memset(test, 'a', 200000);
	streamo.writeChars("Hello World!", 12);
	streamo.writeShortInt(2);
	streamo.writeInt(2);
	streamo.writeLong((__int64)200000L);
	streamo.writeChars(test, strlen(test));

	streamo.seek(0);
	int i2 = streamo.readInt();
	EXPECT_TRUE(i2 == INT_MAX);
	char* text = streamo.readChars();
	EXPECT_TRUE(strcmp(text, "Hello World!") == 0);
	__int16 i16 = streamo.readShortInt();
	EXPECT_TRUE(i16 == 2);
	int i1 = streamo.readInt();
	EXPECT_TRUE(i1 == 2);
	__int64 l = streamo.readLong();
	EXPECT_TRUE(l == (__int64)200000L);
	char* tchar = streamo.readChars();
	EXPECT_TRUE(strcmp(test, tchar) == 0);
	free(test);
	streamo.close();
}

TEST(testFilesystem, testBSONStreams)
{
	cout << "\ntestBSONStreams\n" << endl;

	std::auto_ptr<FileOutputStream> fos(new FileOutputStream("bson.txt", "wb"));
	std::auto_ptr<BSONOutputStream> bsonOut(new BSONOutputStream(fos.get()));

	std::auto_ptr<BSONObj> obj(new BSONObj());
	// Add in
	obj->add("int", INT_MAX);
	obj->add("string", (char*)"test");
	obj->add("char*", (char*)"char*");
	obj->add("long", (__int64)100000000L);
	obj->add("long64", (__int64)LLONG_MAX);
	obj->add("double", 1.1);

	BSONObj inner;
	inner.add("name", "John");
	inner.add("long", (__int64)LONG_MAX);
	inner.add("int", (__int32)1);
	obj->add("inner", inner);

	bsonOut->writeBSON(*obj);

	fos->close();
	fos.release();

	obj.release();

	std::auto_ptr<FileInputStream> fis(new FileInputStream("bson.txt", "rb"));
	std::auto_ptr<BSONInputStream> bsonIn(new BSONInputStream(fis.get()));

	obj = std::auto_ptr<BSONObj>(bsonIn->readBSON());

	EXPECT_TRUE(obj->has("int"));
	EXPECT_TRUE(obj->getInt("int") == INT_MAX);

	EXPECT_TRUE(obj->getString("string").compare("test") == 0);

	EXPECT_TRUE(obj->getString("char*").compare("char*") == 0);

	EXPECT_TRUE(obj->has("long"));
	EXPECT_TRUE(obj->getLong("long") == 100000000L);

	EXPECT_TRUE(obj->has("long64"));
	EXPECT_TRUE(obj->getLong("long64") == LLONG_MAX);

	EXPECT_TRUE(obj->has("double"));
	EXPECT_TRUE(obj->getDouble("double") == 1.1);

	BSONObj* innerR = obj->getBSON("inner");
	EXPECT_TRUE(innerR != NULL);

	EXPECT_TRUE(innerR->has("name"));
	EXPECT_TRUE(innerR->getString("name").compare("John") == 0);

	EXPECT_TRUE(innerR->has("int"));
	EXPECT_TRUE(innerR->getInt("int") == 1);

	EXPECT_TRUE(innerR->has("long"));
	EXPECT_TRUE(innerR->getLong("long") == LONG_MAX);

	fis->close();
}

TEST(testFilesystem, testBSONSelect) {
	cout << "\ntestBSONSelect\n" << endl;
	SCOPED_TRACE("file-system");
	std::auto_ptr<BSONObj> objTest(BSONParser::parse("{age: 1, name: 'John', salary: 3500.25, simplerel: {test: 'inner value', test2: 'inner value2', test3: 3, test4: 3.5}, rel1: [{innertext: 'inner text', test2: 'text2'}, {innertext: 'inner text', test2: 'text333'}, {innertext: 'inner text', test2: 'text4'}, {innertext: 'inner text'} ] }"));
	MemoryStream ms;
	BSONOutputStream bos(&ms);
	bos.writeBSON(*objTest.get());

	ms.seek(0);

	BSONInputStream bis(&ms);
	BSONObj* result = bis.readBSON("*");

	EXPECT_TRUE(*result == *objTest.get());

	delete result;

	ms.seek(0);
	BSONObj expected;
	expected.add("age", 1);

	result = bis.readBSON("$\"age\"");

	EXPECT_TRUE(*result == expected);

	delete result;

	ms.seek(0);
	expected = BSONObj();
	expected.add("age", 1);
	expected.add("name", "John");

	result = bis.readBSON("$\"age\", $\"name\"" );

	EXPECT_TRUE(*result == expected);

	ms.seek(0);
	std::auto_ptr<BSONObj> test(BSONParser::parse("{ age: 1, simplerel: { test2: 'inner value2'}}"));

	result = bis.readBSON("$\"age\", $\"simplerel.test2\"" );

	EXPECT_TRUE(*result == *test.get());

	delete result;

	ms.seek(0);
	std::auto_ptr<BSONObj> testrel(BSONParser::parse("{age: 1, rel1: [{test2: 'text2'}, {test2: 'text333'}, {test2: 'text4'}, {} ] }"));

	result = bis.readBSON("$\"age\", $\"re1.test2\"" );

	EXPECT_TRUE(*result == *testrel.get());

	delete result;
}

TEST(testFilesystem, testInnerArrays) {
	// test bson inner arrays
	std::auto_ptr<BSONObj> objTest(BSONParser::parse("{age: 1, name: 'John', salary: 3500.25, rel1: [{innertext: 'inner text'}, {innertext: 'inner text'}, {innertext: 'inner text'}, {innertext: 'inner text'} ] }"));
	std::auto_ptr<FileOutputStream> fos(new FileOutputStream("bson.txt", "wb"));
	std::auto_ptr<BSONOutputStream> bsonOut(new BSONOutputStream(fos.get()));

	bsonOut->writeBSON(*objTest.get());

	fos->close();

	std::auto_ptr<FileInputStream> fis(new FileInputStream("bson.txt", "rb"));
	std::auto_ptr<BSONInputStream> bsonIn(new BSONInputStream(fis.get()));

	BSONObj* obj = bsonIn->readBSON();
	EXPECT_TRUE(obj->has("age"));
	EXPECT_TRUE(obj->getInt("age") == 1);
	EXPECT_TRUE(obj->getString("name").compare("John") == 0);

	EXPECT_TRUE(obj->has("salary"));
	EXPECT_TRUE(obj->getDouble("salary") == 3500.25);

	EXPECT_TRUE(obj->has("rel1"));
	EXPECT_TRUE(obj->getBSONArray("rel1")->length() == 4);
	EXPECT_TRUE(obj->getBSONArray("rel1")->get(0)->getString("innertext").compare("inner text") == 0);

	delete obj;

}

TEST(testFilesystem, testBSONStreamsComplex)
{
	std::auto_ptr<FileOutputStream> fos(new FileOutputStream("bson.txt", "wb"));
	std::auto_ptr<BSONOutputStream> bsonOut(new BSONOutputStream(fos.get()));

	std::auto_ptr<BSONObj> obj(new BSONObj());
	// Add in
	obj->add("int", 1);
	obj->add("string", "test");
	obj->add("char*", (char*)"char*");
	obj->add("long", (__int64)1L);
	obj->add("long64", (__int64)10000000000000L);
	obj->add("double", 1.1);

	BSONObj inner;
	inner.add("int", 1);
	inner.add("string", "test");
	inner.add("char*", (char*)"char*");
	inner.add("long", (__int64)1L);
	inner.add("long64", (__int64)10000000000000L);
	inner.add("double", 1.1);

	obj->add("inner", inner);

	bsonOut->writeBSON(*obj);

	fos->close();

	obj.release();

	std::auto_ptr<FileInputStream> fis(new FileInputStream("bson.txt", "rb"));
	std::auto_ptr<BSONInputStream> bsonIn(new BSONInputStream(fis.get()));

	obj = std::auto_ptr<BSONObj>(bsonIn->readBSON());

	EXPECT_TRUE(obj->has("int"));
	EXPECT_TRUE(obj->getInt("int") == 1);

	EXPECT_TRUE(obj->getString("string").compare("test") == 0);

	EXPECT_TRUE(obj->getString("char*").compare("char*") == 0);

	EXPECT_TRUE(obj->has("long"));
	EXPECT_TRUE(obj->getLong("long") == 1L);

	EXPECT_TRUE(obj->has("double"));
	EXPECT_TRUE(obj->getDouble("double") == 1.1);

	BSONObj* innerTest = obj->getBSON("inner");
	EXPECT_TRUE(innerTest != NULL);

	EXPECT_TRUE(innerTest->has("int"));
	EXPECT_TRUE(innerTest->getInt("int") == 1);

	EXPECT_TRUE(innerTest->getString("string").compare("test") == 0);

	EXPECT_TRUE(innerTest->getString("char*").compare("char*") == 0);

	EXPECT_TRUE(innerTest->has("long"));
	EXPECT_TRUE(innerTest->getLong("long") == 1L);

	EXPECT_TRUE(innerTest->has("long64"));
	EXPECT_TRUE(innerTest->getLong("long64") == 10000000000000L);

	EXPECT_TRUE(innerTest->has("double"));
	EXPECT_TRUE(innerTest->getDouble("double") == 1.1);

	fis->close();
}

TEST(testFilesystem, testBSONStreamsArray)
{
	std::auto_ptr<FileOutputStream> fos(new FileOutputStream("bson.txt", "wb"));
	std::auto_ptr<BSONOutputStream> bsonOut(new BSONOutputStream(fos.get()));

	std::vector<BSONObj*> elements;
	for (int x = 0; x < 10; x++) {
		BSONObj* obj = new BSONObj();
		// Add in
		obj->add("int", 1);
		obj->add("string", (char*)"test");
		obj->add("char*", (char*)"char*");
		obj->add("long", (__int64)1L);
		obj->add("double", 1.1);

		elements.push_back(obj);
	}

	bsonOut->writeBSONArray(elements);

	fos->close();

	std::auto_ptr<FileInputStream> fis(new FileInputStream("bson.txt", "rb"));
	std::auto_ptr<BSONInputStream> bsonIn(new BSONInputStream(fis.get()));

	BSONArrayObj* result = bsonIn->readBSONArray();

	EXPECT_TRUE(result->length() == 10);
	for (BSONArrayObj::iterator i = result->begin(); i != result->end(); i++) {
		BSONObj* obj = *i;
		EXPECT_TRUE(obj->has("int"));
		EXPECT_TRUE(obj->getInt("int") == 1);

		EXPECT_TRUE(obj->getString("string").compare("test") == 0);

		EXPECT_TRUE(obj->getString("char*").compare("char*") == 0);

		EXPECT_TRUE(obj->has("long"));
		EXPECT_TRUE(obj->getLong("long") == 1L);

		EXPECT_TRUE(obj->has("double"));
		EXPECT_TRUE(obj->getDouble("double") == 1.1);

	}

	fis->close();
	delete result;
}

TEST(testFilesystem, testBSONBufferedArray) {
	cout << "\ntestBSONBufferedArray" << endl;

	MemoryStream ms;
	BSONArrayObj array;
	BSONObj* o1 = BSONParser::parse("{ 'a': 1}");
	array.add(*o1);
	BSONObj* o2 = BSONParser::parse("{ 'a': 2}");
	array.add(*o1);

	BSONOutputStream bos(&ms);
	bos.writeBSONArray(&array);

	char* c = ms.toChars();

	BSONBufferedArrayObj bufferedarray(c, ms.size());

	EXPECT_TRUE(bufferedarray.length() == 2);

}

TEST(testFilesystem, testBSONBuffered) {
	cout << "\ntestBSONBuffered\n" << endl;

	MemoryStream* ms = new MemoryStream();
	BSONObj o;
	o.add("int", 10);
	o.add("long", (__int64)1000000);
	o.add("char", "Test Char");
	o.add("double", 2.14159);

	BSONObj inner;
	inner.add("name", "John");
	inner.add("int", 1);
	o.add("inner", inner);

	BSONArrayObj array;
	BSONObj* o1 = BSONParser::parse("{ 'a': 1}");
	array.add(*o1);
	BSONObj* o2 = BSONParser::parse("{ 'a': 2}");
	array.add(*o2);
	o.add("array", array);

	delete o1;
	delete o2;

	BSONOutputStream bos(ms);
	bos.writeBSON(o);

	char* c = ms->toChars();

	BSONBufferedObj* buffered = new BSONBufferedObj(c, ms->size());

	EXPECT_TRUE(buffered->getInt("int") == 10);
	EXPECT_TRUE(buffered->getLong("long") == (__int64)1000000);
	EXPECT_TRUE(buffered->getString("char").compare("Test Char") == 0);
	EXPECT_TRUE(buffered->getDouble("double") == (double)2.14159);
	BSONContent* innerResult = buffered->getXpath("inner.int");
	BSONObj* or2 = buffered->select("$'int', $'long'");
	if (innerResult != NULL) {
		__int32 inneri = *innerResult;
		EXPECT_TRUE(inneri == 1);
	}
	EXPECT_TRUE(or2->getInt("int") == 10);
	EXPECT_TRUE(or2->getLong("long") == (__int64)1000000);

	BSONBufferedArrayObj* arrayR = (BSONBufferedArrayObj*)buffered->getBSONArray("array");
	EXPECT_TRUE(arrayR->length() == 2);
	BSONBufferedObj* obj1 = (BSONBufferedObj*)arrayR->get(0);
	EXPECT_TRUE(obj1->getInt("a") == 1);
	BSONBufferedObj* obj2 = (BSONBufferedObj*)arrayR->get(1);
	EXPECT_TRUE(obj2->getInt("a") == 2);

	delete buffered;
	delete or2;
	delete ms;
	free(c);
}

TEST(testFilesystem, testFileSeek) {
	Logger* log = getLogger(NULL);
	log->info("testFileSeek");

	if (existFile("testseek.dat")) {
		removeFile("testseek.dat");
	}

	FileInputOutputStream* fos = new FileInputOutputStream("testseek.dat", "wb+");

	fos->writeChars("Test", 4);

	__int32 pos = fos->currentPos();
	fos->seek(0);
	char* test = fos->readChars();
	EXPECT_TRUE(strcmp(test, "Test") == 0);
	fos->seek(0, FROMEND_SEEK);

	fos->writeChars("Test2", 5);
	fos->seek(0);
	char* test2 = fos->readChars();
	EXPECT_TRUE(strcmp(test2, "Test") == 0);
	char* test3 = fos->readChars();
	EXPECT_TRUE(strcmp(test3, "Test2") == 0);
	fos->close();

}
