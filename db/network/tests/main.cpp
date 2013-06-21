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
#include "networkserver.h"
#include "fileinputstream.h"
#include "fileoutputstream.h"
#include "commandwriter.h"
#include "commandreader.h"
#include "util.h"
#include "bsonoutputstream.h"
#include "bsoninputstream.h"
#include "insertcommand.h"
#include "showdbscommand.h"
#include "shutdowncommand.h"
#include "findcommand.h"
#include "updatecommand.h"
#include "bson.h"
#include "defs.h"
#include <sys/types.h>
#include <list>
/* #include <sys/socket.h> #include <netinet/in.h>
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

const char* _host = "localhost";
int _port = 1243;

int MAX_INSERT = 1000;

int __insertCount = 0;

struct Element {
	NetworkOutputStream* nos;
	NetworkInputStream* nis;
};

void* producer(void* arg) {
	NetworkOutputStream* nos = new NetworkOutputStream();
	int socket = nos->open("localhost", _port);

	printf("Producer started\n");
	Logger* log = getLogger(NULL);
	log->info("Producer starter");
	log->startTimeRecord();
	if (socket > 0) {
		NetworkInputStream* nis = new NetworkInputStream(socket);

		std::auto_ptr<CommandWriter> writer(new CommandWriter(nos));

		for (int x = 0; x < MAX_INSERT; x++) {
			std::auto_ptr<InsertCommand> cmd(new InsertCommand());

			BSONObj* obj = new BSONObj();
			std::auto_ptr<std::string> guid(uuid());
			obj->add("_id", guid->c_str());
			char* temp = (char*)malloc(2000);
			memset(temp, 0, 2000);
			memset(temp, 'a', 1999);
			int len = strlen(temp);
			obj->add("content", temp);
			free(temp);
			cmd->setBSON(obj);
			std::string db("mydb");
			cmd->setDB(db);
			std::string ns("myns");
			cmd->setNameSpace(ns);
			cmd->setOptions(new BSONObj());
			writer->writeCommand(cmd.get());

			int result = nis->readInt();
			if (result != 1) {
				break;
			}
		}
		nis->close();
	} else {
		printf("Socket is 0");
	}
	log->info("Producer end");
	log->stopTimeRecord();

	DTime time = log->recordedTime();
	log->info("Producer time: %d", time.toChar());
}

int callBack(void* objCallback, NetworkInputStream* nis, NetworkOutputStream* nos) {
   printf("callback\n");
	CommandReader* reader = new CommandReader(const_cast<NetworkInputStream*>(nis));

	getLogger(NULL)->info("Consumer starter");
	while (__insertCount < MAX_INSERT) {
		Command* cmd = reader->readCommand();

		nos->writeInt(1); // ok
		__insertCount++;
		delete cmd;
	}
	nis->close();
	getLogger(NULL)->info("Consumer end");
}


int callbackServer(void* instance, NetworkInputStream* const nis, NetworkOutputStream* const nos) {
	char* stat;
	Logger* log = getLogger(NULL);

	stat = nis->readChars();
	log->info("server: command received: %s -", stat);
	if (strcmp(stat, "stop") == 0) {
		return -1;
	}

	if (strcmp(stat, "say") == 0) {
		log->info("server: Received say, waiting data");
		char* data = nis->readChars();
		char* response = (char*)malloc(200);
		memset(response, 0, 200);
		memcpy(response, "Hello ", 6);
		memcpy(response + 6, data, strlen(data));
		log->info("server: Sending greeting");
		nos->writeChars(response, strlen(response));
		free(data);
		free(response);
	}
	if (strcmp(stat, "execute") == 0) {
		log->info("server: Received execute, waiting data");
		char* data = nis->readChars();
		EXPECT_TRUE(strcmp(data, "command") == 0);
		free(data);
		log->info("server: Data received");
	}
	if (nis->available() > 0) {
		log->info("---------------------- server: Available");
	}
	free(stat);
	stat = NULL;
}

int callbackCommandServer(void* instance, NetworkInputStream* const nis, NetworkOutputStream* const nos) {
	CommandReader* reader = new CommandReader(nis);
	Logger* log = getLogger(NULL);
	log->info("server: waiting command");
	Command* cmd = reader->readCommand();
	switch (cmd->commandType()) {
		case INSERT:
			log->info("server: received insert");
			break;
		case SHOWDBS:
			log->info("server: received showdbs preparing fixed answer");
			nos->writeInt(3);
			nos->writeString(std::string("db1"));
			nos->writeString(std::string("db2"));
			nos->writeString(std::string("db3"));
			break;
		case SHUTDOWN:
			log->info("server: received shutdown");
			break;
	}
	if (cmd->commandType() == SHUTDOWN) {
		delete cmd;
		return -1;
	}
	delete cmd;
	delete reader;
}


// this method is executed in a thread to simulate multiple clients
void* clients(void* arg) {
	NetworkOutputStream* nos = new NetworkOutputStream();
	int socket = nos->open("localhost", _port);
	NetworkInputStream* nis = new NetworkInputStream(socket);
	Logger* log = getLogger(NULL);

	for (int x = 0; x < 10; x++) {
		log->info("client: Sending say");
		nos->writeChars("say", 3);
		nos->writeChars("Name", 4);
		log->info("client: Waiting greeting");
		char* greeting = nis->readChars();
		EXPECT_TRUE(strcmp(greeting, "Hello Name") == 0);
		free(greeting);
		log->info("client: Received greeting");
		Thread::sleep(100);

		log->info("client: Sending execute");
		nos->writeChars("execute", 7);
		nos->writeChars("command", 7);
		Thread::sleep(100);

		log->info("client: Sending say 2");
		nos->writeChars("say", 3);
		nos->writeChars("Name", 4);
		log->info("client: Waiting greeting 2");
		greeting = nis->readChars();
		EXPECT_TRUE(strcmp(greeting, "Hello Name") == 0);
		free(greeting);
		Thread::sleep(100);
	}
	// Send bye bye
	nos->writeChars("stop", 4);
	nis->close();
}

TEST(TestNetwork, testClients) {
	Logger* log = getLogger(NULL);
	NetworkServer* server = new NetworkServer(_port);
	server->listen(NULL, &callbackServer);

	Thread::sleep(100);
	std::list<Thread*> threads;
	for (int x= 0; x < 1; x++) {
		Thread* t = new Thread(clients);
		threads.push_back(t);
		t->start(NULL);
	}

	for (std::list<Thread*>::iterator it = threads.begin(); it != threads.end(); it++) {
		Thread* t = *it;
		t->join();
	}

	server->stop();
	delete server;
}

// this method is executed in a thread to simulate multiple clients
void* commandClients(void* arg) {
	NetworkOutputStream* nos = new NetworkOutputStream();
	int socket = nos->open("localhost", _port);
	NetworkInputStream* nis = new NetworkInputStream(socket);
	Logger* log = getLogger(NULL);

	CommandWriter* writer = new CommandWriter(nos);
	for (int x = 0; x < 10; x++) {
		log->info("client: preparing insert");
		InsertCommand* cmd = new InsertCommand();
		cmd->setDB("db1");
		cmd->setNameSpace("ns");
		BSONObj* o = new BSONObj();
		std::string* id = uuid();
		o->add("_id", id->c_str());
		delete id;
		std::string* rev = uuid();
		o->add("_revision", rev->c_str());
		delete rev;
		o->add("name", "John");
		cmd->setBSON(o);
		BSONObj* options = new BSONObj();
		cmd->setOptions(options);

		log->info("client: writing insert command");
		writer->writeCommand(cmd);
		log->info("client: insert command sent");
		delete cmd;

		log->info("client: preparing showdbs command");
		ShowdbsCommand* showCmd = new ShowdbsCommand();
		BSONObj* options2 = new BSONObj();
		showCmd->setOptions(options2);
		log->info("client: sending showCmd");
	  	writer->writeCommand(showCmd);	

		log->info("client: waiting showDbs answer");
		int dbs = nis->readInt();
		EXPECT_EQ(dbs, 3);
		char* db1 = nis->readChars();
		EXPECT_TRUE(strcmp(db1, "db1") == 0);
		char* db2 = nis->readChars();
		EXPECT_TRUE(strcmp(db2, "db2") == 0);
		char* db3 = nis->readChars();
		EXPECT_TRUE(strcmp(db3, "db3") == 0);

		log->info("client: showDbs received");
		free(db1);
		free(db2);
		free(db3);
	}

	log->info("client: sending ShutdownCommand");

	ShutdownCommand* shut = new ShutdownCommand();
	writer->writeCommand(shut);

	log->info("client: shutdown sent");
	nis->close();
}

TEST(TestNetwork, testCommands) {
	Logger* log = getLogger(NULL);
	NetworkServer* server = new NetworkServer(_port);
	server->listen(NULL, &callbackCommandServer);

	Thread::sleep(100);
	std::list<Thread*> threads;
	for (int x= 0; x < 1; x++) {
		Thread* t = new Thread(&commandClients);
		threads.push_back(t);
		t->start(NULL);
	}

	for (std::list<Thread*>::iterator it = threads.begin(); it != threads.end(); it++) {
		Thread* t = *it;
		t->join();
	}

	server->stop();
	delete server;
}

TEST(TestNetwork, testPerformance) {
	int inserts = 1;

	Logger* log = getLogger(NULL);

	cout << "Starting " << endl;

	log->startTimeRecord();

	NetworkServer* server = new NetworkServer(_port);
	server->listen(NULL, callBack);

	Thread::sleep(10000);

	cout << "Starting producer thread" << endl;
	Thread* threadProducer = new Thread(producer);
	threadProducer->start(NULL);

	while (__insertCount < MAX_INSERT) {
		Thread::sleep(100);
	}

	cout << "Closing the connection" << endl;

	threadProducer->join();

	server->stop();
	delete server;

	delete(log);
}

/* 

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

*/
