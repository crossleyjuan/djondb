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
#include "linkedmap.hpp"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <cpptest.h>


using namespace std;

class TestUtilSuite : public Test::Suite
{
	public:
		TestUtilSuite() {
			TEST_ADD(TestUtilSuite::testDates);
			TEST_ADD(TestUtilSuite::testTimes);
			TEST_ADD(TestUtilSuite::testStrings);
			TEST_ADD(TestUtilSuite::testUUID);
			TEST_ADD(TestUtilSuite::testSettings);
			TEST_ADD(TestUtilSuite::testFileUtils);
			TEST_ADD(TestUtilSuite::testCircularQueue);
			TEST_ADD(TestUtilSuite::testLinkedMap);
		}

	private:
		void testDates()
		{
			DateTime dt = DateTime::today(true);

			DateTime dt2 = dt.addDays(1);

			TEST_ASSERT((dt.getDay() + 1) == dt2.getDay());

			// testing day of the week
			dt = DateTime(2011, 9, 12);
			TEST_ASSERT(dt.dayOfTheWeek() == 1);

			// testing day of the week "complex", 29 of febr
			dt = DateTime(2008, 2, 29);
			TEST_ASSERT(dt.dayOfTheWeek() == 5);

			// diff
			dt = DateTime(2011, 2, 10);
			dt2 = DateTime(2011, 3, 1);
			int diff = dt2.daysTo(dt);
			TEST_ASSERT(diff == 19);
		}

		static bool compare(int i1, int i2) {
			return i1 == i2;
		}

		static bool compareString(std::string i1, std::string i2) {
			return i1.compare(i2) == 0;
		}

		void testLinkedMap() {
			cout << "\ntestLinkedMap" << endl;
			LinkedMap<int, int> map(TestUtilSuite::compare);

			map.add(1, 10);
			map.add(0, 11);
			map.add(3, 12);

			LinkedMap<int, int>::iterator it = map.begin();
			int i = it->first;
			TEST_ASSERT(i == 1);
			it++;
			i = it->first;
			TEST_ASSERT(i == 0);
			it++;
			i = it->first;
			TEST_ASSERT(i == 3);

			map.erase(0);
			
			it = map.begin();
			i = it->first;
			TEST_ASSERT(i == 1);
			it++;
			i = it->first;
			TEST_ASSERT(i == 3);
			it++;
			TEST_ASSERT(it == map.end());


			// Test empty map
			LinkedMap<int, int> map2(TestUtilSuite::compare);
			TEST_ASSERT(map2.begin() == map2.end());

			LinkedMap<std::string, std::string> map3(TestUtilSuite::compareString);
			TEST_ASSERT(map3.begin() == map3.end());


			//Test unique elements
			LinkedMap<std::string, std::string> map4(TestUtilSuite::compareString);
			map4.add(std::string("test"), "test");
			map4.add(std::string("test2"), "test2");
			map4.add(std::string("test"), "ass");

			TEST_ASSERT(map4.size() == 2);
			TEST_ASSERT(map4["test"].compare(std::string("ass")) == 0);
			LinkedMap<std::string, std::string>::iterator it4 = map4.begin();
			TEST_ASSERT(it4->first.compare(std::string("test")) == 0);
			TEST_ASSERT(it4->second.compare("ass") == 0);
			it4++;
			TEST_ASSERT(it4->first.compare("test2") == 0);
			TEST_ASSERT(it4->second.compare("test2") == 0);
			it4++;
			TEST_ASSERT(it4 == map4.end());
		}

		void testCircularQueue() {
			CircularQueue<int> c(3);
			c.push_back(1);

			for (int x = 0; x < 3; x++) {
				TEST_ASSERT(c.next() == 1);
			}

			for (int x= 0; x < 3; x++) {
				TEST_ASSERT(c.previous() == 1);
			}

			c.push_back(2);

			TEST_ASSERT(c.current() == 2);
			TEST_ASSERT(c.next() == 1);
			TEST_ASSERT(c.next() == 2);

			c.push_back(3);
			TEST_ASSERT(c.current() == 3);
			TEST_ASSERT(c.previous() == 2);
			TEST_ASSERT(c.previous() == 1);
			TEST_ASSERT(c.previous() == 3);
			TEST_ASSERT(c.previous() == 2);

			c.push_back(4);
			TEST_ASSERT(c.current() == 4);
			TEST_ASSERT(c.previous() == 3);
			TEST_ASSERT(c.previous() == 2);
			TEST_ASSERT(c.previous() == 4);

			c.push_front(1);
			TEST_ASSERT(c.current() == 3);
			TEST_ASSERT(c.previous() == 2);
			TEST_ASSERT(c.previous() == 1);
			TEST_ASSERT(c.previous() == 3);

			TEST_ASSERT(c.pop_back() == 3);
			TEST_ASSERT(c.pop_front() == 1);
			TEST_ASSERT(c.current() == 2);
		}

		void testTimes()
		{
			// Testing diff
			DTime t(15, 40, 0);
			DTime t2(16, 20, 0);
			DTime res = t2 - t;

			TEST_ASSERT((res.hour() == 0) && (res.minutes() == 40) && (res.seconds() == 0));

			res = t2 - 200;
			TEST_ASSERT((res.hour() == 16) && (res.minutes() == 16) && (res.seconds() == 40));
			// teting add
		}

		void testStrings()
		{
			// Copy chars
			char* test = "Hello world!";
			char* res = strcpy(test, strlen(test));

			TEST_ASSERT(strcmp(test, res) == 0);
			TEST_ASSERT(test != res);
			free(res);

			// test offset
			char* testOffSet = "Hello world!";
			char* res2 = strcpy(testOffSet, 6, 5);
			TEST_ASSERT(strcmp("world", res2) == 0);
			TEST_ASSERT(testOffSet != res2);
			free(res2);

			//copy string
			std::string s = "Hello world!";
			res = strcpy(s);
			TEST_ASSERT(s.compare(res) == 0);

			// ends with
			bool com = endsWith("test.tex", ".tex");
			TEST_ASSERT(com);
			com = endsWith("test.ss", "test");
			TEST_ASSERT(!com);
			TEST_ASSERT(endsWith("test/", "/"));

			bool start = startsWith("testing the starts with", "testing");
			TEST_ASSERT(start);
			start = startsWith("testing", "testing this");
			TEST_ASSERT(!start);
			start = startsWith("testing", "testing");
			TEST_ASSERT(start);

			// tokenizer
			std::vector<std::string*>* token = tokenizer("test,other,and 1 more", ",");
			TEST_ASSERT(token->size() == 3);
			TEST_ASSERT(token->at(0)->compare("test") == 0);
			TEST_ASSERT(token->at(1)->compare("other") == 0);
			TEST_ASSERT(token->at(2)->compare("and 1 more") == 0);
			delete(token);

			// format
			std::string sformat = format("test %d %s %5.4f", 10, "Hello World!", 3.14159);
			TEST_ASSERT(sformat.compare("test 10 Hello World! 3.1416") == 0);

			// toString
			std::string s2 = toString(10.1);
			TEST_ASSERT(s2.compare("10.1")== 0);

			std::string s3 = toString(3.14159, 2);
			TEST_ASSERT(s3.compare("3.14")== 0);

			std::string s4 = toString(3);
			TEST_ASSERT(s4.compare("3")== 0);

			// split
			std::vector<std::string> sp = split("test,other,and 1 more", ",");
			TEST_ASSERT(sp.size() == 3);
			TEST_ASSERT(sp.at(0).compare("test") == 0);
			TEST_ASSERT(sp.at(1).compare("other") == 0);
			TEST_ASSERT(sp.at(2).compare("and 1 more") == 0);

			//Count char
			long c = countChar("testing.this.component.!", '.');
			TEST_ASSERT(c == 3);

			std::vector<std::string> spl = splitLines("test1\r\ntest2");
			TEST_ASSERT(spl.size() == 2);
			std::string t1 = spl[1];
			TEST_ASSERT(spl[0].compare("test1") == 0);
			TEST_ASSERT(spl[1].compare("test2") == 0);


			// Test concat
			std::string sc1 = "Hello ";
			std::string sc2 = "World!";
			std::string resultconcat = concatStrings(sc1, sc2);
			TEST_ASSERT(resultconcat.compare("Hello World!") == 0);

			// case insensitive comparation
			const char* sci1 = "TEst";
			const char* sci2 = "teSt";
			TEST_ASSERT(compareInsensitive(sci1, sci2));

			char* untrimmed = " Hello";
			char* trimExpected = "Hello";
			char* result = trim(untrimmed, strlen(untrimmed));
			TEST_ASSERT(strcmp(trimExpected, result) == 0);
		}

		void testUUID()
		{
			std::string* u = uuid();
			TEST_ASSERT(u != NULL);
			delete u;
		}

		void testSettings() {
			std::string folder = getSetting("DATA_DIR");

			TEST_ASSERT(folder.compare("/var/djondb") == 0);
		}

		void testFileUtils() {
			//test mkdir

			makeDir("/tmp/test/test2");

			TEST_ASSERT(existDir("/tmp/test/test2"));

			char* home = getenv("HOME");
			TEST_ASSERT(checkFileCreation(home));
			// test directory fail
			TEST_ASSERT(!checkFileCreation("/"));
		}
};
//// Tests unconditional fail TEST_ASSERTs
////
//class FailTestSuite : public Test::Suite
//{
//public:
//    FailTestSuite()
//    {
//        TEST_ADD(FailTestSuite::success)
//        TEST_ADD(FailTestSuite::always_fail)
//
//    }
//
//private:
//    void success() {}
//
//    void always_fail()
//    {
//        // This will always fail
//        //
//        TEST_FAIL("unconditional fail");
//    }
//};
//
//// Tests compare TEST_ASSERTs
////
//class CompareTestSuite : public Test::Suite
//{
//public:
//    CompareTestSuite()
//    {
//        TEST_ADD(CompareTestSuite::success)
//        TEST_ADD(CompareTestSuite::compare)
//        TEST_ADD(CompareTestSuite::delta_compare)
//    }
//
//private:
//    void success() {}
//
//    void compare()
//    {
//        // Will succeed since the expression evaluates to true
//        //
//        TEST_ASSERT(1 + 1 == 2)
//
//        // Will fail since the expression evaluates to false
//        //
//        TEST_ASSERT(0 == 1);
//    }
//
//    void delta_compare()
//    {
//        // Will succeed since the expression evaluates to true
//        //
//        TEST_ASSERT_DELTA(0.5, 0.7, 0.3);
//
//        // Will fail since the expression evaluates to false
//        //
//        TEST_ASSERT_DELTA(0.5, 0.7, 0.1);
//    }
//};
//
//// Tests throw TEST_ASSERTs
////
//class ThrowTestSuite : public Test::Suite
//{
//public:
//    ThrowTestSuite()
//    {
//        TEST_ADD(ThrowTestSuite::success)
//        TEST_ADD(ThrowTestSuite::test_throw)
//    }
//
//private:
//    void success() {}
//
//    void test_throw()
//    {
//        // Will fail since the none of the functions throws anything
//        //
//        TEST_THROWS_MSG(func(), int, "func() does not throw, expected int exception")
//        TEST_THROWS_MSG(func_no_throw(), int, "func_no_throw() does not throw, expected int exception")
//        TEST_THROWS_ANYTHING_MSG(func(), "func() does not throw, expected any exception")
//        TEST_THROWS_ANYTHING_MSG(func_no_throw(), "func_no_throw() does not throw, expected any exception")
//
//        // Will succeed since none of the functions throws anything
//        //
//        TEST_THROWS_NOTHING(func())
//        TEST_THROWS_NOTHING(func_no_throw())
//
//        // Will succeed since func_throw_int() throws an int
//        //
//        TEST_THROWS(func_throw_int(), int)
//        TEST_THROWS_ANYTHING(func_throw_int())
//
//        // Will fail since func_throw_int() throws an int (not a float)
//        //
//        TEST_THROWS_MSG(func_throw_int(), float, "func_throw_int() throws an int, expected a float exception")
//        TEST_THROWS_NOTHING_MSG(func_throw_int(), "func_throw_int() throws an int, expected no exception at all")
//    }
//
//    void func() {}
//    void func_no_throw() throw() {}
//    void func_throw_int() throw(int)
//    {
//        throw 13;
//    }
//};

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
		ts.add(auto_ptr<Test::Suite>(new TestUtilSuite));
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
