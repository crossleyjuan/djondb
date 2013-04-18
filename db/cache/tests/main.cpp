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

#include "cachemanager.h"
#include "prioritycache.h"
#include <assert.h>
#include <gtest/gtest.h>

using namespace std;

TEST(testCache, testCache)
{
	StructureCache* cache = new StructureCache();
	Structure* str = new Structure();
	str->add("name");
	str->add("lastName");
	long id = str->crc();
	CacheManager::structuresCache()->add(id, str);

	Structure* str2 = new Structure();
	str2->add("name");
	str2->add("lastName");
	long id2 = str->crc();

	CacheManager::structuresCache()->add(id2, str);

	Structure* s2 = (*CacheManager::structuresCache())[id];
	EXPECT_TRUE (s2 == str);

	EXPECT_TRUE (s2 != str2);

	int looped = 0;
	for (StructureCache::iterator i = CacheManager::structuresCache()->begin(); i != CacheManager::structuresCache()->end(); i++) {
		long k = i->first;
		Structure* str = i->second; 
		looped++;
	}
	// if the iterator worked then the looped should contain 2
	EXPECT_TRUE(looped == 1);

	// test erase
	CacheManager::structuresCache()->clear();
	EXPECT_TRUE(CacheManager::structuresCache()->size() == 0);


	CacheManager::structuresCache()->add(1, str2);
	StructureCache::iterator i = CacheManager::structuresCache()->begin();
	CacheManager::structuresCache()->erase(i);
	EXPECT_TRUE(CacheManager::structuresCache()->size() == 0);

	// Test several inserts
	CacheManager::objectCache()->add("ABC", new BSONObj());
	CacheManager::objectCache()->add("ABD", new BSONObj());
	CacheManager::objectCache()->add("ABE", new BSONObj());
	CacheManager::objectCache()->add("ABF", new BSONObj());
	CacheManager::objectCache()->add("ABG", new BSONObj());
	CacheManager::objectCache()->add("ABH", new BSONObj());


	EXPECT_TRUE(CacheManager::objectCache()->size() == 6);
}


TEST(testCache, testPriorityCache) {
	PriorityCache<int, char*> cache(3);

	cache.add(1, "a");
	cache.add(2, "b");
	cache.add(3, "c");

	PriorityCache<int, char*>::iterator i = cache[1];
	EXPECT_TRUE(i != cache.end());
	if (i != cache.end()) {
		EXPECT_TRUE(strcmp(i->second, "a") == 0);
	}

	// Test remove
	cache.erase(1);
	PriorityCache<int, char*>::iterator i2 = cache[1];
	EXPECT_TRUE(i2 == cache.end());

	// Test replace
	cache.add(2, "x");
	PriorityCache<int, char*>::iterator i3 = cache[2];
	EXPECT_TRUE(i3 != cache.end());
	if (i3 != cache.end()) {
		EXPECT_TRUE(strcmp(i3->second, "x") == 0);
	}

	// test top
	cache.add(4, "d");
	cache[3]; // pops ups the 3 priority
	cache.add(5, "e");
	i = cache[4];
	EXPECT_TRUE(i != cache.end());
	if (i != cache.end()) {
		EXPECT_TRUE(strcmp(i->second, "d") == 0);
	}
	i = cache[2];
	EXPECT_TRUE(i == cache.end());
}
