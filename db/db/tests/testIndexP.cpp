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
#include "fileinputstream.h"
#include "fileoutputstream.h"
#include <sstream>

#include <gtest/gtest.h>

/* 
TEST(testIndexP, generateNames) {
	FileInputStream* fisNames = new FileInputStream("names.csv", "r");
	const char* fullNames = fisNames->readFull();
	FileInputStream* fisLastNames = new FileInputStream("last.csv", "r");
	const char* fullLast = fisLastNames->readFull();

	std::vector<string> names = split(fullNames, "\r");
	cout << names.size() << endl;
	std::vector<string> lastNames = split(fullLast, "\r");
	cout << lastNames.size() << endl;

	FileOutputStream* fos = new FileOutputStream("names.txt", "w+");
	for (int x = 0; x < 1000000; x++) {
		int i = rand() % names.size();
		std::string name = names.at(i);
		
		i = rand() % lastNames.size();
		std::string lastName = lastNames.at(i);

		std::string fullName = name + " " + lastName;

		fos->writeString(fullName);
	}

	fos->close();
	fisNames->close();
	fisLastNames->close();

	delete fos;
	delete fisNames;
	delete fisLastNames;
}
*/

TEST(testIndexP, testSimple) {
	std::set<std::string> keys;
	keys.insert("_id");

	BPlusIndexP index("testIndex");
	index.setKeys(keys);

	for (int x = 0; x < 1000; x++) {
		BSONObj o;
		std::string* id = uuid();
		o.add("_id", id->c_str());
		char* temp = strcpy(const_cast<char*>(id->c_str()), id->length());
		index.add(o, djondb::string(temp, id->length()), 100, 10);
		delete id;
	}

}

TEST(testIndexP, testRecoverNames) {
	std::set<std::string> keys;
	keys.insert("_id");

	BPlusIndexP* index = new BPlusIndexP("testIndexNames");
	index->setKeys(keys);

	FileInputStream* fis = new FileInputStream("names.txt", "r");

	std::vector<std::string> names;
	while (!fis->eof()) {
		for (int x = 0; x < 10; x++) {
			BSONObj o;
			std::stringstream ss;
			ss << x;
			std::string* name = fis->readString();
			o.add("_id", name->c_str());
			char* temp = strcpy(const_cast<char*>(name->c_str()), name->length());
			index->add(o, djondb::string(temp, name->length()), 100, 10);
			index->debug();
			names.push_back(*name);
			delete name;
		}
	}

	index->debug();
	delete index;

	index = new BPlusIndexP("testIndexNames");
	index->setKeys(keys);
	index->debug();
	for (std::vector<std::string>::iterator i = names.begin(); i != names.end(); i++) {
		std::string name = *i;
		BSONObj o;
		o.add("_id", name.c_str());
		Index* idx = index->find(&o);
		ASSERT_TRUE(idx != NULL);
		ASSERT_TRUE(idx->key->has("_id"));
		EXPECT_TRUE(idx->key->getString("_id").compare(name) == 0);
	}
	delete index;
}

TEST(testIndexP, testRecoverRandom) {
	std::set<std::string> keys;
	keys.insert("_id");

	BPlusIndexP* index = new BPlusIndexP("testIndex2");
	index->setKeys(keys);

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
		index->debug();
	}

	index->debug();
	delete index;

	index = new BPlusIndexP("testIndex2");
	index->setKeys(keys);
	index->debug();
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
