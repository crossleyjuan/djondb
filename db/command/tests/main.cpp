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
#include <gtest/gtest.h>

#include <string>

TEST(TestCommand, testInsertCommand) {
	cout << "testInsertCommand" << endl;
	FileOutputStream* fos = new FileOutputStream("test.dat", "wb");

	CommandWriter* commandWriter = new CommandWriter(fos);
	InsertCommand cmd;
	cmd.setDB("testdb");
	cmd.setNameSpace("test.namespace.db");
	BSONObj obj;
	obj.add("name", "Cross");
	obj.add("age", 18);
	cmd.setBSON(obj);

	commandWriter->writeCommand(&cmd);

	fos->close();
	delete fos;
	delete commandWriter;

	FileInputStream* fis = new FileInputStream("test.dat", "rb");
	CommandReader* reader = new CommandReader(fis);
	InsertCommand* rdCmd = (InsertCommand*) reader->readCommand();
	EXPECT_TRUE(rdCmd != NULL);
	EXPECT_TRUE(rdCmd->nameSpace()->compare("test.namespace.db") == 0);
	EXPECT_TRUE(rdCmd->DB()->compare("testdb") == 0);
	BSONObj* objResult = rdCmd->bson();
	EXPECT_TRUE(objResult != NULL);
	EXPECT_TRUE(objResult->has("name"));	
	EXPECT_TRUE(objResult->getString("name").compare("Cross") == 0);
}

TEST(TestCommand, testOptionsCommand) {
	cout << "testOptionsCommand" << endl;
	FileOutputStream* fos = new FileOutputStream("test.dat", "wb");

	CommandWriter* commandWriter = new CommandWriter(fos);

	BSONObj* options = new BSONObj();
	std::string* txId = uuid();
	options->add("_transactionId", const_cast<char*>(txId->c_str())); 
	delete txId;

	InsertCommand cmd;
	cmd.setDB("testdb");
	cmd.setNameSpace("test.namespace.db");
	cmd.setOptions(options);

	BSONObj obj;
	obj.add("name", "Cross");
	obj.add("age", 18);
	cmd.setBSON(obj);

	commandWriter->writeCommand(&cmd);

	fos->close();
	delete fos;
	delete commandWriter;

	FileInputStream* fis = new FileInputStream("test.dat", "rb");
	CommandReader* reader = new CommandReader(fis);
	InsertCommand* rdCmd = (InsertCommand*) reader->readCommand();
	EXPECT_TRUE(rdCmd != NULL);
	EXPECT_TRUE(rdCmd->nameSpace()->compare("test.namespace.db") == 0);
	EXPECT_TRUE(rdCmd->DB()->compare("testdb") == 0);
	BSONObj* objResult = rdCmd->bson();
	EXPECT_TRUE(objResult != NULL);
	EXPECT_TRUE(objResult->has("name"));	
	EXPECT_TRUE(objResult->getString("name").compare("Cross") == 0);

	EXPECT_TRUE(rdCmd->options() != NULL);
	if (rdCmd->options() != NULL) {
		const BSONObj* options = rdCmd->options();
		EXPECT_TRUE(options->has("_transactionId"));
	}
}

TEST(TestCommand, testShowdbs) {
	cout << "testShowdbs" << endl;
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
	EXPECT_TRUE(resCmd->commandType() == SHOWDBS);

	fis->close();

	delete resCmd;
	delete fis;
	delete reader;
}

TEST(TestCommand, testShownamespacesCommand) {
	cout << "testShownamespacesCommand" << endl;
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
	EXPECT_TRUE(resCmd->commandType() == SHOWNAMESPACES);
	ShownamespacesCommand* sw = (ShownamespacesCommand*)resCmd;
	EXPECT_TRUE(sw->DB()->compare("testdb") == 0);

	fis->close();

	delete resCmd;
	delete fis;
	delete reader;
}

TEST(TestCommand, testUpdateCommand) {
	cout << "testUpdateCommand" << endl;
	FileOutputStream* fos = new FileOutputStream("test.dat", "wb");

	CommandWriter* commandWriter = new CommandWriter(fos);
	UpdateCommand cmd;
	cmd.setDB("testdb");
	cmd.setNameSpace("test.namespace.db");
	BSONObj obj;
	std::string* uid = uuid();
	obj.add("_id", const_cast<char*>(uid->c_str()));
	delete uid;
	obj.add("name", "Cross");
	obj.add("age", 18);
	cmd.setBSON(obj);

	commandWriter->writeCommand(&cmd);

	fos->close();
	delete fos;
	delete commandWriter;

	FileInputStream* fis = new FileInputStream("test.dat", "rb");
	CommandReader* reader = new CommandReader(fis);
	UpdateCommand* rdCmd = (UpdateCommand*) reader->readCommand();
	EXPECT_TRUE(rdCmd != NULL);
	EXPECT_TRUE(rdCmd->nameSpace()->compare("test.namespace.db") == 0);
	EXPECT_TRUE(rdCmd->DB()->compare("testdb") == 0);
	BSONObj* objResult = rdCmd->bson();
	EXPECT_TRUE(objResult  != NULL);
	EXPECT_TRUE(objResult->has("name"));	
	EXPECT_TRUE(objResult->getString("name").compare("Cross") == 0);
}

TEST(TestCommand, testRemoveCommand) {
	cout << "testRemoveCommand" << endl;
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
	EXPECT_TRUE(rdCmd != NULL);
	EXPECT_TRUE(rdCmd->nameSpace()->compare("test.namespace.db") == 0);
	EXPECT_TRUE(rdCmd->DB()->compare("testdb") == 0);
	EXPECT_TRUE(rdCmd->id()->compare(*uid) == 0);
	EXPECT_TRUE(rdCmd->revision()->compare(*revision) == 0);

	delete fis;
	delete reader;
	delete rdCmd;
}

TEST(TestCommand, testFindCommand) {
	cout << "testFindCommand" << endl;
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
	EXPECT_TRUE(rdCmd != NULL);
	EXPECT_TRUE(rdCmd->select()->compare("*") == 0);
	EXPECT_TRUE(rdCmd->nameSpace()->compare("test.namespace.db") == 0);
	EXPECT_TRUE(rdCmd->DB()->compare("testdb") == 0);
	EXPECT_TRUE(rdCmd->filter()->compare("$'a.b.c' == 1") == 0);
}

TEST(TestCommand, testDropnamespaceCommand) {
	cout << "testDropnamespaceCommand" << endl;
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
	EXPECT_TRUE(rdCmd != NULL);
	EXPECT_TRUE(rdCmd->nameSpace()->compare("test.namespace.db") == 0);
	EXPECT_TRUE(rdCmd->DB()->compare("testdb") == 0);
}
