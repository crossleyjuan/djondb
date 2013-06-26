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

			log->info("Testing performance over: %d inserts.", top);
			std::vector<std::string>* names = generateNames(top);
			std::vector<std::string*>* ids = new std::vector<std::string*>();
			log->startTimeRecord();
			for (int x = 0; x < top; x++) {
				BSONObj obj;
				char* text = (char*)malloc(1001);
				memset(text, 0, 1001);
				memset(text, 'a', 1000);

				std::string* id = uuid();
				obj.add("_id", id->c_str());
				int test = rand() % 100;
				if (test > 30) {
					ids->push_back(id);
				} else {
					delete id;
				}
				obj.add("t", x);

				obj.add("text", text);
				obj.add("name", const_cast<char*>(names->at(x).c_str()));

				conn->insert("db", "testperformance", obj);
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
				log->info("Inserted %d, throughtput too high to be measured", top);
			}
			if ((time.totalSecs() > 0) && ((top / time.totalSecs()) < 16000))  {
				log->info("Performance is not good enough");
			}


			conn->close();

			delete log;
		}
		
		void testCommand(int port, int top = 10000000) {
			DjondbConnection* conn = DjondbConnectionManager::getConnection("localhost", port);

			if (!conn->open()) {
				cout << "Not connected" << endl;
				exit(0);
			}

			Logger* log = getLogger(NULL);

			log->startTimeRecord();
			log->info("Testing command performance over: %d executions.", top);
			for (int x = 0; x < top; x++) {
				std::vector<std::string>* dbs = conn->dbs();
				if (dbs == NULL) {
					log->info("Test command failed and returned NULL");
					exit(1);
				}
				if (dbs->size() == 0) {
					log->info("Test command failed and returned 0 elements");
					exit(1);
				}
				delete dbs;
			}
			log->stopTimeRecord();

			DTime time = log->recordedTime();

			cout << "Total secs: " << time.totalSecs() << endl;
			if (time.totalSecs() > 0) {
				log->info("Executed %d: throughtput: %d.", top, (top / time.totalSecs()));
			} else {
				log->info("Executed %d, throughtput too high to be measured", top);
			}
			if ((time.totalSecs() > 0) && ((top / time.totalSecs()) < 5000))  {
				log->info("Performance is not good enough");
			}
		}

		void testFind(int port, int top = 10000000) {
			DjondbConnection* conn = DjondbConnectionManager::getConnection("localhost", port);

			if (!conn->open()) {
				cout << "Not connected" << endl;
				exit(0);
			}

			Logger* log = getLogger(NULL);

			log->startTimeRecord();
			log->info("Testing find performance over: %d finds.", top);
			for (int x = 0; x < top; x++) {
				BSONArrayObj* arr = conn->executeQuery("select top 1 * from db:testperformance");
				if (arr->length() == 0) {
					log->info("Error an id was not found");
					exit(1);
				}
				delete arr;
			}
			log->stopTimeRecord();

			DTime time = log->recordedTime();

			cout << "Total find secs: " << time.totalSecs() << endl;
			if (time.totalSecs() > 0) {
				log->info("Found %d: throughtput: %d.", top, (top / time.totalSecs()));
			} else {
				log->info("Found :%d, throughtput too high to be measured", top);
			}
			if ((time.totalSecs() > 0) && ((top / time.totalSecs()) < 16000))  {
				log->info("Performance is not good enough");
			}
		}
};

int main(int argc, char** arg) {
	int c;
	int port = 1243;
	int top = -1;
	bool finds = false;
	bool command = false;
	while ((c = getopt(argc, arg, "p:t:fc")) != -1) {
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
							 printf("Usage: -p PORT t TOP c f\n");
							 exit(0);
							 break;
						 }
			case 'c': {
							 command = true;
							 break;
						 }
			case 'f': {
							 finds = true;
							 break;
						 }
		}
	}
	TestPerfomance p;
	if (!finds && !command) {
		if (top  <= 0) {
			p.testPerfomance(port);
		} else {
			p.testPerfomance(port, top);
		}
	} else if (finds) {
		p.testFind(port, top);
	} else if (command) {
		p.testCommand(port, top);
	}

	exit(0);
}
