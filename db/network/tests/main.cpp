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
#include "networkservice.h"
#include "networkoutputstream.h"
#include "networkinputstream.h"
#include "fileinputstream.h"
#include "fileoutputstream.h"
#include "commandwriter.h"
#include "util.h"
#include "bsonoutputstream.h"
#include "bsoninputstream.h"
#include "insertcommand.h"
#include "findcommand.h"
#include "updatecommand.h"
#include "bson.h"
#include "defs.h"
#include <sys/types.h>
/*
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
*/
#include <string.h>
#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include "command.h"
#include <gtest/gtest.h>
#include <memory>

#ifdef WINDOWS
#include <Windows.h>
#endif
#include <math.h>

using namespace std;


bool __running;

char* _host = "localhost";
int _port = 1243;

TEST(TestNetwork, testInsert) {
	int inserts = 1;

	Logger* log = getLogger(NULL);

	cout << "Starting " << endl;

	log->startTimeRecord();
	__running = true;
	std::auto_ptr<NetworkOutputStream> out(new NetworkOutputStream());
	int socket = out->open(_host, _port);
	std::auto_ptr<NetworkInputStream> nis(new NetworkInputStream(socket));
	//    out->setNonblocking();
	out->disableNagle();
	//    Thread* receiveThread = new Thread(&startSocketListener);
	//    receiveThread->start(nis);
	std::auto_ptr<BSONInputStream> bis(new BSONInputStream(nis.get()));
	//    BSONOutputStream* bsonOut = new BSONOutputStream(out);
	std::auto_ptr<CommandWriter> writer(new CommandWriter(out.get()));

	std::vector<std::string> ids;
	for (int x = 0; x < inserts; x++) {

		std::auto_ptr<InsertCommand> cmd(new InsertCommand());

		BSONObj obj;
		std::auto_ptr<std::string> guid(uuid());
		obj.add("_id", guid->c_str());
		int test = rand() % 10;
		if (test > 0) {
			ids.push_back(*guid.get());
		}
		//        obj->add("name", "John");
		char* temp = (char*)malloc(2000);
		memset(temp, 0, 2000);
		memset(temp, 'a', 1999);
		int len = strlen(temp);
		obj.add("content", temp);
		free(temp);
		//obj->add("last", "Smith");
		cmd->setBSON(obj);
		std::string ns("myns");
		cmd->setNameSpace(ns);
		writer->writeCommand(cmd.get());
		//        std::auto_ptr<BSONObj> resObj(bis->readBSON());
		//        EXPECT_TRUE(resObj.get() != NULL);
		//        EXPECT_TRUE(resObj->has("_id"));
		if ((inserts > 9) && (x % (inserts / 10)) == 0) {
			cout << x << " Records sent" << endl;
		}
	}
	FileOutputStream* fosIds = new FileOutputStream((char*)"results.txt", (char*)"wb");
	fosIds->writeInt(ids.size());
	for (std::vector<std::string>::iterator i2 = ids.begin(); i2!= ids.end(); i2++) {
		std::string s = *i2;
		fosIds->writeString(s);
	}
	fosIds->close();
	cout << "Sending close connection command" << endl;
	out->writeString(std::string("1.2.3"));
	out->writeInt(CLOSECONNECTION);
	cout << "all sent" << endl;

	log->stopTimeRecord();

	DTime rec = log->recordedTime();

	int secs = rec.totalSecs();
	cout<< "inserts " << inserts << ", time: " << rec.toChar() << endl;

	if (secs > 0) {
		cout << "Throughput: " << (inserts / secs) << " ops." << endl;
	}
	cout << "------------------------------------------------------------" << endl;
	cout << "Ready to close the connection" << endl;
	getchar();
	__running = false;

	cout << "Closing the connection" << endl;
	out->closeStream();

	delete(log);
}

TEST(TestNetwork, testFinds) {
	int maxfinds = 1;
	Logger* log = getLogger(NULL);

	cout << "Starting " << endl;

	log->startTimeRecord();
	__running = true;
	NetworkOutputStream* out = new NetworkOutputStream();
	int socket = out->open(_host, _port);
	NetworkInputStream* nis = new NetworkInputStream(socket);
	// nis->setNonblocking();
	//    Thread* receiveThread = new Thread(&startSocketListener);
	//    receiveThread->start(nis);
	BSONInputStream* bis = new BSONInputStream(nis);
	//    BSONOutputStream* bsonOut = new BSONOutputStream(out);
	std::auto_ptr<CommandWriter> writer(new CommandWriter(out));
	FileInputStream* fisIds = new FileInputStream("results.txt", "rb");
	int x = 0;
	int count = fisIds->readInt();
	if ((maxfinds > -1) && (count > maxfinds)) {
		count = maxfinds;
	}
	cout << "Records to find: " << count << endl;
	for (x =0; x < count; x++) {
		std::string* guid = fisIds->readString();
		std::auto_ptr<FindCommand> cmd(new FindCommand());

		BSONObj obj;
		obj.add("_id", guid->c_str());
		//obj->add("last", "Smith");
		cmd->setFilter("$'_id' == '" + *guid + "'");
		std::string ns("myns");
		cmd->setNameSpace(ns);
		writer->writeCommand(cmd.get());
		std::auto_ptr<BSONObj> resObj(bis->readBSON());
		EXPECT_TRUE(resObj.get() != NULL);
		EXPECT_TRUE(resObj->has("_id"));
		if ((count > 9) && (x % (count / 10)) == 0) {
			cout << x << " Records received" << endl;
		}
		delete guid;
	}
	cout << "Sending close connection command" << endl;
	out->writeString(std::string("1.2.3"));
	out->writeInt(CLOSECONNECTION);
	cout << "all sent" << endl;

	log->stopTimeRecord();

	DTime rec = log->recordedTime();

	int secs = rec.totalSecs();
	cout<< "finds " << count << ", time: " << rec.toChar() << endl;

	if (secs > 0) {
		cout << "Throughput: " << (count / secs) << " ops." << endl;
	}
	cout << "------------------------------------------------------------" << endl;
	cout << "Ready to close the connection" << endl;
	getchar();
	__running = false;

	cout << "Closing the connection" << endl;
	out->closeStream();

	delete(log);
}

TEST(TestNetwork, testUpdate) {
	int maxupdates =1;
	Logger* log = getLogger(NULL);

	cout << "Starting " << endl;

	log->startTimeRecord();
	__running = true;
	NetworkOutputStream* out = new NetworkOutputStream();
	int socket = out->open(_host, _port);
	NetworkInputStream* nis = new NetworkInputStream(socket);
	// nis->setNonblocking();
	//    Thread* receiveThread = new Thread(&startSocketListener);
	//    receiveThread->start(nis);
	BSONInputStream* bis = new BSONInputStream(nis);
	//    BSONOutputStream* bsonOut = new BSONOutputStream(out);
	std::auto_ptr<CommandWriter> writer(new CommandWriter(out));
	FileInputStream* fisIds = new FileInputStream("results.txt", "rb");
	int x = 0;
	int count = fisIds->readInt();
	if ((maxupdates > -1) && (count > maxupdates)) {
		count = maxupdates;
	}
	cout << "Records to update: " << count << endl;

	std::vector<std::string> idsUpdated;
	for (x =0; x < count; x++) {
		std::auto_ptr<std::string> guid(fisIds->readString());
		std::auto_ptr<UpdateCommand> cmd(new UpdateCommand());

		BSONObj obj;
		obj.add("_id", guid->c_str());

		idsUpdated.push_back(*guid.get());
		char* temp = (char*)malloc(100);
		memset(temp, 0, 100);
		memset(temp, 'b', 99);
		int len = strlen(temp);
		obj.add("content", temp);
		free(temp);
		//obj->add("last", "Smith");
		cmd->setBSON(obj);
		std::string ns("myns");
		cmd->setNameSpace(ns);
		writer->writeCommand(cmd.get());

		if ((count > 9) && (x % (count / 10)) == 0) {
			cout << x << " Records received" << endl;
		}
	}

	log->stopTimeRecord();

	cout << "Executing a verification" << endl;

	for (std::vector<std::string>::iterator i = idsUpdated.begin(); i != idsUpdated.end(); i++) {
		std::string guid = *i;

		std::auto_ptr<FindCommand> cmd (new FindCommand());

		BSONObj obj;
		obj.add("_id", guid.c_str());
		cmd->setFilter("$'_id' == '" + guid + "'");
		cmd->setNameSpace("myns");
		writer->writeCommand(cmd.get());

		std::auto_ptr<BSONObj> resObj(bis->readBSON());
		EXPECT_TRUE(resObj.get() != NULL);
		EXPECT_TRUE(resObj->has("_id"));
		EXPECT_TRUE(resObj->has("content"));

		char* temp = (char*)malloc(100);
		memset(temp, 0, 100);
		memset(temp, 'b', 99);
		EXPECT_TRUE(resObj->getString("content").compare(temp) == 0);
		free(temp);
	}
	DTime rec = log->recordedTime();

	int secs = rec.totalSecs();
	cout<< "finds " << count << ", time: " << rec.toChar() << endl;

	if (secs > 0) {
		cout << "Throughput: " << (count / secs) << " ops." << endl;
	}
	cout << "------------------------------------------------------------" << endl;
	cout << "Ready to close the connection" << endl;
	getchar();
	__running = false;

	cout << "Closing the connection" << endl;
	out->closeStream();

	delete(log);
}

