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
#include "fileinputstream.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

using namespace std; 
using namespace djondb;

class TestPerfomance {
	public:

		TestPerfomance() {
		}

		std::vector<std::string>* generateNames(int number) {
			// this will avoid overriding previously generated names and keep consistent the results
				FileInputStream* fisNames = new FileInputStream("names.csv", "r");
				const char* fullNames = fisNames->readFull();
				FileInputStream* fisLastNames = new FileInputStream("last.csv", "r");
				const char* fullLast = fisLastNames->readFull();

				std::vector<std::string> names = split(fullNames, "\r");
				cout << names.size() << endl;
				std::vector<std::string> lastNames = split(fullLast, "\r");
				cout << lastNames.size() << endl;

				std::vector<std::string>* generated = new std::vector<std::string>();
				for (int x = 0; x < number; x++) {
					int i = rand() % names.size();
					std::string name = names.at(i);

					i = rand() % lastNames.size();
					std::string lastName = lastNames.at(i);

					std::string fullName = name + " " + lastName;

					generated->push_back(fullName);
				}

				fisNames->close();
				fisLastNames->close();

				delete fisNames;
				delete fisLastNames;
				return generated;
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
			std::vector<std::string>* names = generateNames(top);

			log->startTimeRecord();
			log->info("Testing performance over: %s inserts.", top);
			for (int x = 0; x < top; x++) {
				BSONObj obj;
				char* text = (char*)malloc(1001);
				memset(text, 0, 1001);
				memset(text, 'a', 1000);

				obj.add("t", x);

				obj.add("text", text);
				obj.add("name", const_cast<char*>(names->at(x).c_str()));

				conn->insert("db", "test.performance", obj);
				free(text);
				// every 10 % will print a message showing the progress
				if ((x % (top / 10)) == 0) {
					DTime timeTemp = log->recordedTime();
					int progress = (x * 100) / top;
					if (timeTemp.totalSecs() > 0) {
						log->info("Inserted %d: throughtput: %d per sec. %d comnpleted", x, (x / timeTemp.totalSecs()), progress);
					} else {
						log->info("Inserted :%d, throughtput too high to be measured. %d completed.", x, progress);
					}
				}
			}

			log->stopTimeRecord();

			DTime time = log->recordedTime();

			cout << "Total secs: " << time.totalSecs() << endl;
			if (time.totalSecs() > 0) {
				log->info("Inserted %d: throughtput: %d.", top, (top / time.totalSecs()));
			} else {
				log->info("Inserted :%d, throughtput too high to be measured´n", top);
			}
			if ((time.totalSecs() > 0) && ((top / time.totalSecs()) < 16000))  {
				log->info("Performance is not good enough");
			}

			//			conn->shutdown();

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
