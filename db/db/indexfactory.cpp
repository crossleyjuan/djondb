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

#include "indexfactory.h"

#include "bson.h"
#include "bplusindex.h"
//#include "bplusindexp.h"
#include <stdio.h>
#include <sstream>
#include <string>

IndexFactory IndexFactory::indexFactory;

std::string indexkey(const char* ns, const std::set<std::string>& keys) {
	char buffer[3000];
	memset(buffer, 0, 3000);
	sprintf(buffer, "ns:%s", ns);
	int pos = strlen(ns);
	for (std::set<std::string>::const_iterator it = keys.begin(); it != keys.end(); it++) {
		std::string s = *it;
		sprintf(buffer + pos, "%s:", s.c_str());
		pos += s.length();
	}

	std::string skey = std::string(buffer);
	return skey;
}

IndexFactory::IndexFactory()
{
    //ctor
}

IndexFactory::~IndexFactory()
{
	for (listByDbType::iterator idbs = _indexes.begin(); idbs != _indexes.end(); idbs++) {
		listByNSTypePtr ins = idbs->second;
		for (listByNSType::iterator i = ins->begin(); i != ins->end(); i++) {
			listAlgorithmsTypePtr indexes = i->second;
			for (listAlgorithmsType::iterator iindex = indexes->begin(); iindex != indexes->end(); iindex++) {
				IndexAlgorithm* alg = *iindex;
				delete alg;
			}
			indexes->clear();
			delete indexes;
		}
		ins->clear();
		delete ins;
	}
	_indexes.clear();
}

bool IndexFactory::containsIndex(const char* db, const char* ns, const std::string& key) {
	std::set<std::string> keys;
	keys.insert(key);
	return containsIndex(db, ns, keys);
}

IndexAlgorithm* IndexFactory::findIndex(const listAlgorithmsTypePtr& algs, const std::set<std::string>& keys) {
	IndexAlgorithm* result = NULL;

	for (listAlgorithmsType::const_iterator i = algs->begin(); i != algs->end(); i++) {
		IndexAlgorithm* impl = *i;
		std::set<std::string> implKeys = impl->keys();

		std::set<std::string>::const_iterator iKeys = keys.begin();
		std::set<std::string>::const_iterator implItKeys = implKeys.begin();

		bool found = true;
		while (true) {
			if ((iKeys != keys.end()) && (implItKeys != implKeys.end())) {
				std::string key1 = *iKeys;
				std::string key2 = *implItKeys;

				if (key1.compare(key2) != 0) {
					found = false;
					break;
				}
				iKeys++;
				implItKeys++;
				// get to the end of both sides and all the keys are equal
				if ((iKeys == keys.end()) && (implItKeys == implKeys.end())) {
					break;
				}
			} else {
				found = false;
				break;
			}
		}
		// If all the keys match the implementation keys then this is an exact index
		if (found) {
			result = impl;
			break;
		}
	}
	return result;
}

void IndexFactory::dropIndex(const listAlgorithmsTypePtr& algs, const std::set<std::string>& keys) {
	IndexAlgorithm* result = NULL;

	for (listAlgorithmsType::iterator i = algs->begin(); i != algs->end(); i++) {
		IndexAlgorithm* impl = *i;
		std::set<std::string> implKeys = impl->keys();

		std::set<std::string>::const_iterator iKeys = keys.begin();
		std::set<std::string>::const_iterator implItKeys = implKeys.begin();

		while (true) {
			if ((iKeys != keys.end()) && (implItKeys != implKeys.end())) {
				std::string key1 = *iKeys;
				std::string key2 = *implItKeys;

				if (key1.compare(key2) != 0) {
					break;
				}
				iKeys++;
				implItKeys++;
				// get to the end of both sides and all the keys are equal
				if ((iKeys == keys.end()) && (implItKeys == implKeys.end())) {
					algs->erase(i);
					delete impl;
					break;
				}
			}
		}
	}
}

bool IndexFactory::containsIndex(const char* db, const char* ns, const std::set<std::string>& keys) {
	bool result = false;
	listByDbType::const_iterator itIndexes = _indexes.find(std::string(db));
	if (itIndexes != _indexes.end()) {	
		listByNSTypePtr byns = itIndexes->second;
		listByNSType::const_iterator itIndexesNS = byns->find(std::string(ns));
		if (itIndexesNS != byns->end()) {
			listAlgorithmsTypePtr algorithms = itIndexesNS->second;

			IndexAlgorithm* impl = findIndex(algorithms, keys);
			if (impl != NULL) {
				result = true;
			}
		}
	}
	return result;
}

IndexAlgorithm* IndexFactory::index(const char* db, const char* ns, const std::string& key) {
	std::set<std::string> skeys;
	skeys.insert(key);
	return index(db, ns, skeys);
}

listAlgorithmsTypePtr IndexFactory::findAlgorithms(const char* db, const char* ns) {
	listByDbType::iterator itIndexesByDB = _indexes.find(std::string(db));
	listByNSTypePtr indexesByNs;
	if (itIndexesByDB == _indexes.end()) {
		indexesByNs = new listByNSType();
		_indexes.insert(pair<std::string, listByNSTypePtr >(std::string(db), indexesByNs));
	} else {
		indexesByNs = itIndexesByDB->second;
	}

	listByNSType::iterator itNs = indexesByNs->find(std::string(ns));

	listAlgorithmsTypePtr algorithms;
	if (itNs == indexesByNs->end()) {
		algorithms = new listAlgorithmsType();
		indexesByNs->insert(pair<std::string, listAlgorithmsTypePtr >(std::string(ns), algorithms));
	} else {
		algorithms = itNs->second;
	}

	return algorithms;
}

IndexAlgorithm* IndexFactory::index(const char* db, const char* ns, const std::set<std::string>& keys) {
	listAlgorithmsTypePtr algorithms = findAlgorithms(db, ns);

	IndexAlgorithm* indexImpl = findIndex(algorithms, keys);
	if (indexImpl == NULL) {
		char* indexFileName = (char*)malloc(strlen(db) + strlen(ns) + 2);
		memset(indexFileName, 0, strlen(db) + strlen(ns) + 2);
		memcpy(indexFileName, db, strlen(db));
		memcpy(indexFileName + strlen(db), "/", 1);
		memcpy(indexFileName + strlen(db) + 1, ns, strlen(ns));
		indexImpl = new BPlusIndex(keys);
		/* 
		indexImpl = new BPlusIndex(indexFileName);
		indexImpl->setKeys(keys);
		*/
		algorithms->push_back(indexImpl);
		free(indexFileName);
	}

	return indexImpl;
}

void IndexFactory::dropIndex(const char* db, const char* ns, const std::string& key) {
	std::set<std::string> skeys;
	skeys.insert(key);
	dropIndex(db, ns, skeys);
}

void IndexFactory::dropIndex(const char* db, const char* ns, const std::set<std::string>& keys) {
	listAlgorithmsTypePtr algorithms = findAlgorithms(db, ns);

	dropIndex(algorithms, keys);
	
}


void IndexFactory::dropIndexes(const char* db, const char* ns) {

	listByDbType::iterator itIndexesByDB = _indexes.find(std::string(db));
	listByNSTypePtr indexesByNs;
	if (itIndexesByDB == _indexes.end()) {
		indexesByNs = new listByNSType();
		_indexes.insert(pair<std::string, listByNSTypePtr >(std::string(db), indexesByNs));
	} else {
		indexesByNs = itIndexesByDB->second;
	}

	listByNSType::iterator itNs = indexesByNs->find(std::string(ns));

	listAlgorithmsTypePtr algorithms;
	if (itNs != indexesByNs->end()) {
		algorithms = itNs->second;
	} else {
		return;
	}

	for (listAlgorithmsType::iterator i = algorithms->begin(); i != algorithms->end(); i++) {
		IndexAlgorithm* impl = *i;
		delete impl;
	}
	algorithms->clear();
	indexesByNs->erase(itNs);
}
