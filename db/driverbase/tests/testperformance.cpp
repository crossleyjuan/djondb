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
#include "djondb_client.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

using namespace std; 
using namespace djondb;

class TestPerfomance {
	public:

		TestPerfomance() {
		}

		void testPerfomance(int port, int top = 10000000) {
			DjondbConnection* conn = DjondbConnectionManager::getConnection("localhost", port);

			if (!conn->open()) {
				cout << "Not connected" << endl;
				exit(0);
			}

			// 1k inserts
			//
			Logger* log = getLogger(NULL);
			log->startTimeRecord();
			int tests[] = { 10, 100, 1000, 10000, 100000, 1000000, 10000000};
			for (int i = 0; i < 7; i++) {
				if (tests[i] > top) {
					break;
				}
				cout << "Testing performance over: " << tests[i] << " inserts" << endl;
				for (int x = 0; x < tests[i]; x++) {
					std::string* uid = uuid();
					BSONObj obj;
					char* text = (char*)malloc(1001);
					memset(text, 0, 1001);
					memset(text, 'a', 1000);

					obj.add("text", text);

					obj.add("_id", *uid);
					delete uid;
					conn->insert("db", "test.performance", obj);
					free(text);
				}

				log->stopTimeRecord();

				DTime time = log->recordedTime();

				cout << "Total secs: " << time.totalSecs() << endl;
				if (time.totalSecs() > 0) {
					cout << "Performance over: " << tests[i] << " inserts:" << (tests[i] / time.totalSecs()) << endl;
				} else {
					cout << "Total time is not enough to do some calculations" << endl;
				}
				if ((time.totalSecs() > 0) && ((tests[i] / time.totalSecs()) < 16000))  {
					cout << "Performance is not good enough" << endl;
				}
			}

			conn->shutdown();

			conn->close();

			delete log;
		}
};

int main(int argc, char** arg) {
	int c;
	int port = 1243;
	int top = -1;
	while ((c = getopt(argc, arg, "p:t:")) != -1) {
		switch (c) {
			case 'p': {
							 char* cport = optarg;
							 port = atoi(cport);
							 break;
						 }
			case 't': {
							 char* ctop = optarg;
							 top = atoi(ctop);
							 break;
						 }
			case '?': {
							 printf("Usage: -p PORT t TOP\n");
							 exit(0);
							 break;
						 }
		}
	}
	TestPerfomance p;
	if (top  <= 0) {
		p.testPerfomance(port);
	} else {
		p.testPerfomance(port, top);
	}

	exit(0);
}
