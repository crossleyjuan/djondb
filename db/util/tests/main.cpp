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
#include "util.h"
#include "lock.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <gtest/gtest.h>
#ifdef WINDOWS
#include <Shlobj.h>
#endif


using namespace std;

TEST(testUtil, testDates)
{
	DateTime dt = DateTime::today(true);

	DateTime dt2 = dt.addDays(1);

	EXPECT_TRUE((dt.getDay() + 1) == dt2.getDay());

	// testing day of the week
	dt = DateTime(2011, 9, 12);
	EXPECT_TRUE(dt.dayOfTheWeek() == 1);

	// testing day of the week "complex", 29 of febr
	dt = DateTime(2008, 2, 29);
	EXPECT_TRUE(dt.dayOfTheWeek() == 5);

	// diff
	dt = DateTime(2011, 2, 10);
	dt2 = DateTime(2011, 3, 1);
	int diff = dt2.daysTo(dt);
	EXPECT_TRUE(diff == 19);
}

TEST(testUtil, testCircularQueue) {
	CircularQueue<int> c(3);
	c.push_back(1);

	for (int x = 0; x < 3; x++) {
		EXPECT_TRUE(c.next() == 1);
	}

	for (int x= 0; x < 3; x++) {
		EXPECT_TRUE(c.previous() == 1);
	}

	c.push_back(2);

	EXPECT_TRUE(c.current() == 2);
	EXPECT_TRUE(c.next() == 1);
	EXPECT_TRUE(c.next() == 2);

	c.push_back(3);
	EXPECT_TRUE(c.current() == 3);
	EXPECT_TRUE(c.previous() == 2);
	EXPECT_TRUE(c.previous() == 1);
	EXPECT_TRUE(c.previous() == 3);
	EXPECT_TRUE(c.previous() == 2);

	c.push_back(4);
	EXPECT_TRUE(c.current() == 4);
	EXPECT_TRUE(c.previous() == 3);
	EXPECT_TRUE(c.previous() == 2);
	EXPECT_TRUE(c.previous() == 4);

	c.push_front(1);
	EXPECT_TRUE(c.current() == 3);
	EXPECT_TRUE(c.previous() == 2);
	EXPECT_TRUE(c.previous() == 1);
	EXPECT_TRUE(c.previous() == 3);

	EXPECT_TRUE(c.pop_back() == 3);
	EXPECT_TRUE(c.pop_front() == 1);
	EXPECT_TRUE(c.current() == 2);
}

TEST(testUtil, testTimes)
{
	// Testing diff
	DTime t(15, 40, 0);
	DTime t2(16, 20, 0);
	DTime res = t2 - t;

	EXPECT_TRUE((res.hour() == 0) && (res.minutes() == 40) && (res.seconds() == 0));

	res = t2 - 200;
	EXPECT_TRUE((res.hour() == 16) && (res.minutes() == 16) && (res.seconds() == 40));
	// teting add
}

TEST(testUtil, testStrings)
{
	// Copy chars
	char* test = "Hello world!";
	char* res = strcpy(test, strlen(test));

	EXPECT_TRUE(strcmp(test, res) == 0);
	EXPECT_TRUE(test != res);
	free(res);

	// test offset
	char* testOffSet = "Hello world!";
	char* res2 = strcpy(testOffSet, 6, 5);
	EXPECT_TRUE(strcmp("world", res2) == 0);
	EXPECT_TRUE(testOffSet != res2);
	free(res2);

	//copy string
	std::string s = "Hello world!";
	res = strcpy(s);
	EXPECT_TRUE(s.compare(res) == 0);

	// ends with
	bool com = endsWith("test.tex", ".tex");
	EXPECT_TRUE(com);
	com = endsWith("test.ss", "test");
	EXPECT_TRUE(!com);
	EXPECT_TRUE(endsWith("test/", "/"));

	bool start = startsWith("testing the starts with", "testing");
	EXPECT_TRUE(start);
	start = startsWith("testing", "testing this");
	EXPECT_TRUE(!start);
	start = startsWith("testing", "testing");
	EXPECT_TRUE(start);

	// tokenizer
	std::vector<std::string*>* token = tokenizer("test,other,and 1 more", ",");
	EXPECT_TRUE(token->size() == 3);
	EXPECT_TRUE(token->at(0)->compare("test") == 0);
	EXPECT_TRUE(token->at(1)->compare("other") == 0);
	EXPECT_TRUE(token->at(2)->compare("and 1 more") == 0);
	delete(token);

	// format
	std::string sformat = format("test %d %s %5.4f", 10, "Hello World!", 3.14159);
	EXPECT_TRUE(sformat.compare("test 10 Hello World! 3.1416") == 0);

	// toString
	std::string s2 = toString(10.1);
	EXPECT_TRUE(s2.compare("10.1")== 0);

	std::string s3 = toString(3.14159, 2);
	EXPECT_TRUE(s3.compare("3.14")== 0);

	std::string s4 = toString(3);
	EXPECT_TRUE(s4.compare("3")== 0);

	// split
	std::vector<std::string> sp = split("test,other,and 1 more", ",");
	EXPECT_TRUE(sp.size() == 3);
	EXPECT_TRUE(sp.at(0).compare("test") == 0);
	EXPECT_TRUE(sp.at(1).compare("other") == 0);
	EXPECT_TRUE(sp.at(2).compare("and 1 more") == 0);

	//Count char
	long c = countChar("testing.this.component.!", '.');
	EXPECT_TRUE(c == 3);

	std::vector<std::string> spl = splitLines("test1\r\ntest2");
	EXPECT_TRUE(spl.size() == 2);
	std::string t1 = spl[1];
	EXPECT_TRUE(spl[0].compare("test1") == 0);
	EXPECT_TRUE(spl[1].compare("test2") == 0);


	// Test concat
	std::string sc1 = "Hello ";
	std::string sc2 = "World!";
	std::string resultconcat = concatStrings(sc1, sc2);
	EXPECT_TRUE(resultconcat.compare("Hello World!") == 0);

	// case insensitive comparation
	const char* sci1 = "TEst";
	const char* sci2 = "teSt";
	EXPECT_TRUE(compareInsensitive(sci1, sci2));

	char* untrimmed = " Hello";
	char* trimExpected = "Hello";
	char* result = trim(untrimmed, strlen(untrimmed));
	EXPECT_TRUE(strcmp(trimExpected, result) == 0);
}

TEST(testUtil, testUUID)
{
	std::string* u = uuid();
	EXPECT_TRUE(u != NULL);
	delete u;
}

TEST(testUtil, testSettings) {
	std::string folder = getSetting("DATA_DIR");

#ifndef WINDOWS
	EXPECT_TRUE(folder.compare("/var/djondb") == 0);
#else
	EXPECT_TRUE(folder.compare("") == 0);
#endif
}

TEST(testUtil, testFileUtils) {
	//test mkdir
#ifndef WINDOWS
	const char* tmpDir = "/tmp/test/test2";
	char* home = getenv("HOME");
#else
	const char* tmpDir = "c:\\temp\\tests2";
	WCHAR path[MAX_PATH];
	char* home = (char*)malloc(MAX_PATH);
	memset(home, 0, MAX_PATH);
	if (SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_PROFILE, NULL, 0, path))) {
		wcstombs(home, path, MAX_PATH);
	}
#endif
	makeDir(tmpDir);

	EXPECT_TRUE(existDir(tmpDir));

	EXPECT_TRUE(checkFileCreation(home));
	// test directory fail
	EXPECT_TRUE(!checkFileCreation("/"));


	cout << "\ntesting combine paths" << endl;

#ifndef WINDOWS
	const char* path1 = "/home/test";
	const char* path2 = "myfolder/myfile.txt";
	char* result = combinePath(path1, path2);
	EXPECT_TRUE(strcmp(result, "/home/test/myfolder/myfile.txt") == 0);
	free(result);

	const char* path3 = "/home/test/";
	const char* path4 = "myfolder/myfile.txt";
	char* result2 = combinePath(path3, path4);
	EXPECT_TRUE(strcmp(result2, "/home/test/myfolder/myfile.txt") == 0);
#else
	char* path1 = "c:\\test";
	char* path2 = "myfolder\\myfile.txt";
	char* result = combinePath(path1, path2);
	EXPECT_TRUE(strcmp(result, "c:\\test\\myfolder\\myfile.txt") == 0);

	char* path3 = "c:\\test\\";
	char* path4 = "myfolder\\myfile.txt";
	char* result2 = combinePath(path3, path4);
	EXPECT_TRUE(strcmp(result2, "c:\\test\\myfolder\\myfile.txt") == 0);
	free(home);
#endif
}

void testFunction(const char* x) {
	djondb::string s("test", 4);
	EXPECT_TRUE(strcmp(s.c_str(), x) == 0);
}

const char* testFunctionWithReturn(djondb::string s) {
	djondb::string s2 = s;
	return s2.c_str();
}

TEST(testUtil, testDjonStrings) {
	djondb::string s("test", 4);

	const char* c = s.c_str();
	__int32 l = s.length();
	EXPECT_TRUE(strcmp(c, "test") == 0);
	EXPECT_TRUE(l == 4);

	djondb::string s2("test", 4);
	EXPECT_TRUE(s == s2);

	djondb::string s3("other", 5);
	EXPECT_TRUE(s != s3);


	// Testing copy smart references
	char* cx = strcpy("Testing", 7);
	djondb::string s4(cx, 7);
	djondb::string s5(s4);
	const char* c4 = s4.c_str();
	const char* c5 = s5.c_str();
	// Compare the references, they should be the same
	EXPECT_TRUE(c4 == c5);

	// testing simple copying
	djondb::string s6(strcpy("Otra", 4), 4);
	djondb::string* s7 = new djondb::string(s6);
	delete s7;
	const char* c6 = s6.c_str();
	EXPECT_TRUE(strcmp(c6, "Otra") == 0);


	djondb::string s8(strcpy("Otra", 4), 4);
	{
		djondb::string s9 = s8;
		const char* c9 = s9.c_str();
		EXPECT_TRUE(strcmp(s8.c_str(), c9) == 0);
	};
	EXPECT_TRUE(strcmp(s8.c_str(), "Otra") == 0);

	// Test null strings
	djondb::string s10;
	djondb::string s11(s10);


	djondb::string s12("test", 4);
	const char* cs12 = testFunctionWithReturn(s12);
	testFunction(s12.c_str());
	EXPECT_TRUE(strcmp(s12.c_str(), cs12) == 0);


	djondb::string s13("aaaa", 4);
	djondb::string s14("bbbb", 4);
	EXPECT_TRUE(s13.compare(s14) < 0);                 
	EXPECT_TRUE(s13.compare(s14.c_str(), 4) < 0);                 

	djondb::string s15("aa", 2);
	djondb::string s16("aabb", 4);
	EXPECT_TRUE(s15.compare(s16) < 0);                 
}

static void* testMethodProduce(void* val) {
	cout << "\nProduce method started" << endl;
	Lock* lock = (Lock*)val;

	cout << "Produce: Sleeping for 3 segs" << endl;
	Thread::sleep(3000);

	lock->notify();
	cout << "Produce: notification sent" << endl;

	return NULL;
}

static void* testMethodConsume(void* val) {
	cout << "\nConsume method started" << endl;
	Lock* lock = (Lock*)val;

	cout << "Consume method waiting" << endl;
	lock->wait();

	cout << "Consume received notification" << endl;

	return NULL;
}

static void* threadMethodTest(void* val) {
	cout << "\ntestMethodTest" << endl;

	Thread::sleep(10000);
	cout << "\ntestMethodTest finished" << endl;
	return NULL;
}

static void* threadMethodTestLock(void* val) {
	cout << "\ntestMethodTestLock" << endl;

	Lock* lock = (Lock*)val;
	cout << "Locking" << endl;
	lock->lock();
	Thread::sleep(5000);
	lock->unlock();
	cout << "Unlocking" << endl;
	cout << "\ntestMethodTestLock finished" << endl;

	return NULL;
}

TEST(testUtil, testThreads) {
	cout << "\ntestThreads" << endl;
	Thread* thread = new Thread(&threadMethodTest);
	thread->start(NULL);

	Thread* thread2 = new Thread(&threadMethodTest);
	thread2->start(NULL);

	thread->join();
	thread2->join();

	delete thread;
	delete thread2;

	cout << "\nTesting locks" << endl;

	Thread* t1 = new Thread(threadMethodTestLock);
	Thread* t2 = new Thread(threadMethodTestLock);

	Lock* lock = new Lock();
	t1->start(lock);
	t2->start(lock);

	t1->join();
	t2->join();

	delete lock;
	delete t1;
	delete t2;

	Lock* lwait = new Lock();
	Thread* t3 = new Thread(&testMethodProduce);
	Thread* t4 = new Thread(&testMethodConsume);

	t3->start(lwait);
	t4->start(lwait);

	t3->join();
	t4->join();
}

