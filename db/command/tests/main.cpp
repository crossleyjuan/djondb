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
#include "fileinputstream.h"
#include "fileoutputstream.h"
#include "bsonoutputstream.h"
#include "command.h"
#include "commandwriter.h"
#include "commandreader.h"
#include "insertcommand.h"
#include "updatecommand.h"
#include "removecommand.h"
#include "findcommand.h"
#include "dropnamespacecommand.h"
#include "showdbscommand.h"
#include "shownamespacescommand.h"
#include "util.h"
#include <cpptest.h>

#include <string>

class TestCommandSuite: public Test::Suite {
	public:
		TestCommandSuite() {
			TEST_ADD(TestCommandSuite::testInsertCommand);	
			TEST_ADD(TestCommandSuite::testUpdateCommand);	
			TEST_ADD(TestCommandSuite::testRemoveCommand);	
			TEST_ADD(TestCommandSuite::testFindCommand);	
			TEST_ADD(TestCommandSuite::testDropnamespaceCommand);	
			TEST_ADD(TestCommandSuite::testShownamespacesCommand);	
		}

		void testInsertCommand() {
			FileOutputStream* fos = new FileOutputStream("test.dat", "wb");

			CommandWriter* commandWriter = new CommandWriter(fos);
			InsertCommand cmd;
			cmd.setDB("testdb");
			cmd.setNameSpace("test.namespace.db");
			BSONObj obj;
			obj.add("name", std::string("Cross"));
			obj.add("age", 18);
			cmd.setBSON(obj);

			commandWriter->writeCommand(&cmd);

			fos->close();
			delete fos;
			delete commandWriter;

			FileInputStream* fis = new FileInputStream("test.dat", "rb");
			CommandReader* reader = new CommandReader(fis);
			InsertCommand* rdCmd = (InsertCommand*) reader->readCommand();
			TEST_ASSERT(rdCmd != NULL);
			TEST_ASSERT(rdCmd->nameSpace()->compare("test.namespace.db") == 0);
			TEST_ASSERT(rdCmd->DB()->compare("testdb") == 0);
			BSONObj* objResult = rdCmd->bson();
			TEST_ASSERT(objResult != NULL);
			TEST_ASSERT(objResult->has("name"));	
			TEST_ASSERT(objResult->getString("name").compare("Cross") == 0);
		}

		void testShowdbs() {
			FileOutputStream* fos = new FileOutputStream("test.dat", "wb");

			CommandWriter* writer = new CommandWriter(fos);
			ShowdbsCommand cmd;
			writer->writeCommand(&cmd);

			fos->close();
			delete fos;
			delete writer;

			FileInputStream* fis = new FileInputStream("test.dat", "rb");
			CommandReader* reader = new CommandReader(fis);

			Command* resCmd = reader->readCommand();
			TEST_ASSERT(resCmd->commandType() == SHOWDBS);

			fis->close();

			delete resCmd;
			delete fis;
			delete reader;
		}

		void testShownamespacesCommand() {
			FileOutputStream* fos = new FileOutputStream("test.dat", "wb");

			CommandWriter* writer = new CommandWriter(fos);
			ShownamespacesCommand cmd;
			cmd.setDB("testdb");
			writer->writeCommand(&cmd);

			fos->close();
			delete fos;
			delete writer;

			FileInputStream* fis = new FileInputStream("test.dat", "rb");
			CommandReader* reader = new CommandReader(fis);

			Command* resCmd = reader->readCommand();
			TEST_ASSERT(resCmd->commandType() == SHOWNAMESPACES);
			ShownamespacesCommand* sw = (ShownamespacesCommand*)resCmd;
			TEST_ASSERT(sw->DB()->compare("testdb") == 0);

			fis->close();

			delete resCmd;
			delete fis;
			delete reader;
		}

		void testUpdateCommand() {
			FileOutputStream* fos = new FileOutputStream("test.dat", "wb");

			CommandWriter* commandWriter = new CommandWriter(fos);
			UpdateCommand cmd;
			cmd.setDB("testdb");
			cmd.setNameSpace("test.namespace.db");
			BSONObj obj;
			std::string* uid = uuid();
			obj.add("_id", *uid);
			delete uid;
			obj.add("name", std::string("Cross"));
			obj.add("age", 18);
			cmd.setBSON(obj);

			commandWriter->writeCommand(&cmd);

			fos->close();
			delete fos;
			delete commandWriter;

			FileInputStream* fis = new FileInputStream("test.dat", "rb");
			CommandReader* reader = new CommandReader(fis);
			UpdateCommand* rdCmd = (UpdateCommand*) reader->readCommand();
			TEST_ASSERT(rdCmd != NULL);
			TEST_ASSERT(rdCmd->nameSpace()->compare("test.namespace.db") == 0);
			TEST_ASSERT(rdCmd->DB()->compare("testdb") == 0);
			BSONObj* objResult = rdCmd->bson();
			TEST_ASSERT(objResult  != NULL);
			TEST_ASSERT(objResult->has("name"));	
			TEST_ASSERT(objResult->getString("name").compare("Cross") == 0);
		}

		void testRemoveCommand() {
			FileOutputStream* fos = new FileOutputStream("test.dat", "wb");

			CommandWriter* commandWriter = new CommandWriter(fos);
			RemoveCommand cmd;
			cmd.setDB("testdb");
			cmd.setNameSpace("test.namespace.db");
			std::string* uid = uuid();
			cmd.setId(*uid);
			std::string* revision = uuid();
			cmd.setRevision(*revision);

			commandWriter->writeCommand(&cmd);

			fos->close();
			delete fos;
			delete commandWriter;

			FileInputStream* fis = new FileInputStream("test.dat", "rb");
			CommandReader* reader = new CommandReader(fis);
			RemoveCommand* rdCmd = (RemoveCommand*) reader->readCommand();
			TEST_ASSERT(rdCmd != NULL);
			TEST_ASSERT(rdCmd->nameSpace()->compare("test.namespace.db") == 0);
			TEST_ASSERT(rdCmd->DB()->compare("testdb") == 0);
			TEST_ASSERT(rdCmd->id()->compare(*uid) == 0);
			TEST_ASSERT(rdCmd->revision()->compare(*revision) == 0);

			delete fis;
			delete reader;
			delete rdCmd;
		}

		void testFindCommand() {
			FileOutputStream* fos = new FileOutputStream("test.dat", "wb");

			CommandWriter* commandWriter = new CommandWriter(fos);
			FindCommand cmd;
			cmd.setDB("testdb");
			cmd.setNameSpace("test.namespace.db");
			cmd.setSelect("*");
			cmd.setFilter("$'a.b.c' == 1");

			commandWriter->writeCommand(&cmd);

			fos->close();
			delete fos;
			delete commandWriter;

			FileInputStream* fis = new FileInputStream("test.dat", "rb");
			CommandReader* reader = new CommandReader(fis);
			FindCommand* rdCmd = (FindCommand*) reader->readCommand();
			TEST_ASSERT(rdCmd != NULL);
			TEST_ASSERT(rdCmd->select()->compare("*") == 0);
			TEST_ASSERT(rdCmd->nameSpace()->compare("test.namespace.db") == 0);
			TEST_ASSERT(rdCmd->DB()->compare("testdb") == 0);
			TEST_ASSERT(rdCmd->filter()->compare("$'a.b.c' == 1") == 0);
		}
		
		void testDropnamespaceCommand() {
			FileOutputStream* fos = new FileOutputStream("test.dat", "wb");

			CommandWriter* commandWriter = new CommandWriter(fos);
			DropnamespaceCommand cmd;
			cmd.setDB("testdb");
			cmd.setNameSpace("test.namespace.db");

			commandWriter->writeCommand(&cmd);

			fos->close();
			delete fos;
			delete commandWriter;

			FileInputStream* fis = new FileInputStream("test.dat", "rb");
			CommandReader* reader = new CommandReader(fis);
			DropnamespaceCommand* rdCmd = (DropnamespaceCommand*) reader->readCommand();
			TEST_ASSERT(rdCmd != NULL);
			TEST_ASSERT(rdCmd->nameSpace()->compare("test.namespace.db") == 0);
			TEST_ASSERT(rdCmd->DB()->compare("testdb") == 0);
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
		ts.add(auto_ptr<Test::Suite>(new TestCommandSuite));
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
