/*
 * =====================================================================================
 *
 *       Filename:  testperformance.cpp
 *
 *    Description:  This executes several performance tests to be measured and fixed
 *
 *        Version:  1.0
 *        Created:  07/30/2012 09:17:35 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>

#include "bson.h"
#include "util.h"
#include <iostream>

void testToChar() {
	BSONObj obj;
	obj.add("int", 10);
	obj.add("long", (__int64)10L);
	obj.add("string", (char*)"Test");
	obj.add("double", 3.14159);

	Logger* log = getLogger(NULL);
	log->startTimeRecord();	
	for (int x = 0; x < 10000000; x++) {
		char* f = obj.toChar();
		free(f);
	}
	log->stopTimeRecord();
	DTime time = log->recordedTime();
	if (time.totalSecs() > 0) {
		cout << "Performance over: " << time.totalSecs() << endl;
	} else {
		cout << "Total time is not enough to do some calculations" << endl;
	}

	delete log;
}

int main(int argc, char** args) {
	testToChar();
}
