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
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <iostream>
#include <netdb.h> 
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include "command.h"
#include <cpptest.h>

#ifdef WINDOWS
#include <Windows.h>
#endif
#include <math.h>

using namespace std;


bool __running;
void *startSocketListener(void* arg);


void *startSocketListener(void* arg) {
	NetworkInputStream* nis = (NetworkInputStream*)arg;

	BSONInputStream* bis = new BSONInputStream(nis);
	while (__running) {
//		std::auto_ptr<BSONObj> resObj(bis->readBSON());
//		TEST_ASSERT(resObj.get() != NULL);
// 	TEST_ASSERT(resObj->has("_id"));
	}
}

class TestNetworkSuite: public Test::Suite {
	private:
		char* _host;
		int _port;

	public:
		TestNetworkSuite() {
			_host = (char*)"localhost";
			_port = 1243;

			//TEST_ADD(TestNetworkSuite::testInsert);
			//TEST_ADD(TestNetworkSuite::testFinds);
		//	TEST_ADD(TestNetworkSuite::testUpdate);
		}

		void testInsert() {
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
				//        TEST_ASSERT(resObj.get() != NULL);
				//        TEST_ASSERT(resObj->has("_id"));
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

		void testFinds() {
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
				TEST_ASSERT(resObj.get() != NULL);
				TEST_ASSERT(resObj->has("_id"));
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

		void testUpdate() {
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
				TEST_ASSERT(resObj.get() != NULL);
				TEST_ASSERT(resObj->has("_id"));
				TEST_ASSERT(resObj->has("content"));

				char* temp = (char*)malloc(100);
					memset(temp, 0, 100);
				memset(temp, 'b', 99);
				TEST_ASSERT(resObj->getString("content").compare(temp) == 0);
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

/*
		int main(int argc, char* args[])
		{
			//    NetworkService service;
			//    service.start();
			//
			int inserts;
			bool insert = false;
			bool finds = false;
			bool updates = false;
			bool error = false;
			int maxfinds = -1;
			int maxupdates = -1;
			if (argc < 2) {
				cout << "No command specified" << endl;
				error = true;
			}
			for (int x = 1; x < argc; x++) {
				if (strncmp(args[x], "--insert", 8) == 0) {
					insert = true;
					char* val = strchr(args[x], '=');
					if (val == NULL) {
						cout << "Incorrect syntax, expected --insert=xxx" << endl;
						error = true;
					} else {
						inserts = atoi(val + 1);
					}
				} else if (strncmp(args[x], "--finds", 7) == 0) {
					finds = true;
					char* val = strchr(args[x], '=');
					if (val != NULL) {
						maxfinds = atoi(val+1);
					}
				} else if (strncmp(args[x], "--updates", 9) == 0) {
					updates = true;
					char* val = strchr(args[x], '=');
					if (val != NULL) {
						maxupdates = atoi(val+1);
					}
				} else {
					cout << "Incorrect command " << args[x] << endl;
					error = true;
				}
			}
			if (error) {
				cout << "Usage: test-network [--inserts=xxxx] [--finds[=maxfinds] ] [--update[=maxupdates] ]" << endl;
				return 1;
			}
			//    timespec t1;
			//    clock_gettime(CLOCK_REALTIME, &t1);
			//    sleep(2);
			//    timespec t2;
			//    clock_gettime(CLOCK_REALTIME, &t2);
			//
			//    timespec res = diff(t1, t2);
			//    cout << "res: " << res.tv_sec << ":" << res.tv_nsec << endl;
			if (insert) {
				cout << "Inserts " << inserts << endl;
				testMassiveInsert(inserts);
			}

			if (finds) {
				cout << "Finds " << endl;
				testFinds("local_host",1243, maxfinds);
			}

			if (updates) {
				cout << "Updates " << endl;
				testUpdate("local_host",1243, maxupdates);
			}
			//
			//    service.stop();

			return 0;
		}
*/

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
        ts.add(auto_ptr<Test::Suite>(new TestNetworkSuite));
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
