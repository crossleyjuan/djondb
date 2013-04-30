// =====================================================================================
// 
//  @file:  testIndexP.cpp
// 
//  @brief:  Test implementation for Persistent B+ tree
// 
//  @version:  1.0
//  @date:     04/28/2013 08:54:09 PM
//  Compiler:  g++
// 
//  @author:  Juan Pablo Crossley (Cross), crossleyjuan@gmail.com
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
// =====================================================================================
#include "bplusindexp.h"
#include "bson.h"
#include <sstream>

#include <gtest/gtest.h>

TEST(testIndexP, testSimple) {
	std::set<std::string> keys;
	keys.insert("_id");

	BPlusIndexP index(keys, "testIndex");

	for (int x = 0; x < 1000; x++) {
		BSONObj o;
		std::string* id = uuid();
		o.add("_id", id->c_str());
		char* temp = strcpy(const_cast<char*>(id->c_str()), id->length());
		index.add(o, djondb::string(temp, id->length()), 100, 10);
		delete id;
	}

}

TEST(testIndexP, testRecover) {
	std::set<std::string> keys;
	keys.insert("_id");

	BPlusIndexP* index = new BPlusIndexP(keys, "testIndex2");

	std::vector<std::string> ids;
	for (int x = 0; x < 10; x++) {
		BSONObj o;
		std::stringstream ss;
		ss << x;
		std::string id = ss.str();
		o.add("_id", id.c_str());
		int n = rand() % 100;
		if (n > 0) {
			ids.push_back(id);
		}
		char* temp = strcpy(const_cast<char*>(id.c_str()), id.length());
		index->add(o, djondb::string(temp, id.length()), 100, 10);
	}

	delete index;

	index = new BPlusIndexP(keys, "testIndex2");
	for (std::vector<std::string>::iterator i = ids.begin(); i != ids.end(); i++) {
		std::string guid = *i;
		BSONObj o;
		o.add("_id", guid.c_str());
		Index* idx = index->find(&o);
		ASSERT_TRUE(idx != NULL);
		ASSERT_TRUE(idx->key->has("_id"));
		EXPECT_TRUE(idx->key->getString("_id").compare(guid) == 0);
	}
	delete index;
}
