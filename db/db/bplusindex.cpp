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

#include "bplusindex.h"

#include "bson.h"
#include "util.h"
#include "prioritycache.h"
#include "filterparser.h"
#include "expressionresult.h"
#include <string.h>
#include <iostream>
#include <sstream>
#include <boost/crc.hpp>
#include <boost/shared_ptr.hpp>
#include <vector>

bool compareIndex(INDEXPOINTERTYPE key1, INDEXPOINTERTYPE key2) {
	return strcmp(key1, key2);
}

BPlusIndex::BPlusIndex(std::set<std::string> keys)
	: IndexAlgorithm(keys)
{
    _head = new IndexPage();
}


BPlusIndex::~BPlusIndex()
{
    if (_head)
    {
    }
}

void BPlusIndex::add(const BSONObj& elem, const std::string documentId, long filePos, long indexPos)
{
	Index* index = new Index();
	index->key = new BSONObj(elem);
	index->documentId = documentId;
	index->posData = filePos;
	index->indexPos = indexPos;
	insertIndexElement(_head, index);
}

Index* BPlusIndex::find(BSONObj* const elem)
{
	INDEXPOINTERTYPE key = elem->toChar();
	IndexPage* page = findIndexPage(_head, key);

	Index* result = NULL;
	for (int x = 0; x < page->size; x++) {
		Index* current = page->elements[x];
		INDEXPOINTERTYPE testKey = current->key->toChar();
		int res;
		if ((res = strcmp(testKey, key)) == 0) {
			result = current;
			break;
		}
	}
	free(key);
	return result;
}

void BPlusIndex::remove(const BSONObj& elem)
{
}


void BPlusIndex::debug() {
	_head->debug();
}

bool IndexPage::isLeaf() const {
	return _leaf;
}

bool IndexPage::isFull() const {
	return size == BUCKET_MAX_ELEMENTS;
}

IndexPage* BPlusIndex::findIndexPage(IndexPage* start, INDEXPOINTERTYPE key) const {
	Logger* log = getLogger(NULL);
	if (start->isLeaf()) {
		return start;
	} else {
		for (int x = 0; x < start->size; x++) {
			Index* current = start->elements[x];
			INDEXPOINTERTYPE testKey = current->key->toChar();
			int result = strcmp(key, testKey);
			free(testKey);

			if ((result < 0) && (start->pointers[x] != NULL)) {
				return findIndexPage(start->pointers[x], key);
			}
		}
		if (start->pointers[start->size] != NULL) {
			return findIndexPage(start->pointers[start->size], key);
		}
	}
}

void BPlusIndex::insertIndexElement(IndexPage* page, Index* index) {
	Logger* log = getLogger(NULL);

	IndexPage* pageFound = findIndexPage(_head, index->key->toChar());

	pageFound->add(index);
	checkPage(pageFound);
}

IndexPage::IndexPage() {
	parentElement = NULL;
	size = 0;
	_leaf = true;
	elements = (Index**)malloc(BUCKET_MAX_ELEMENTS * sizeof(Index*));
	for (int x = 0; x < BUCKET_MAX_ELEMENTS; x++) {
		elements[x] = NULL;
	}
	pointers = (IndexPage**)malloc((BUCKET_MAX_ELEMENTS + 1) * sizeof(IndexPage*));
	for (int x = 0; x < BUCKET_MAX_ELEMENTS + 1; x++) {
		pointers[x] = NULL;
	}
}

int IndexPage::add(Index* index) {
	Logger* log = getLogger(NULL);
	int indexPositionResult = 0;
	if (size == 0) {
		elements[0] = index;
	} else {
		int res;
		int x = 0;
		INDEXPOINTERTYPE key = index->key->toChar();
		for (int x = 0; x < BUCKET_MAX_ELEMENTS; x++) {
			Index* current = elements[x];
			indexPositionResult = x;
			if (current == NULL) {
				elements[x] = index;
				break;
			} else {
				INDEXPOINTERTYPE currentKey = current->key->toChar();
				int res = strcmp(key, currentKey); 
				if (res < 0) {
					// Moves the elements to the right side before inserting the new element
					for (int i = size; i > x; i--) {
						elements[i] = elements[i-1];
						pointers[i + 1] = pointers[i];
						pointers[i] = pointers[i - 1];
					}
					elements[x] = index;
					pointers[x] = NULL;
					break;
				}
			}
		}
	}
	size++;
	return indexPositionResult;
}

void BPlusIndex::checkPage(IndexPage* page) {
	Logger* log = getLogger(NULL);
	if (page->isFull()) {
		IndexPage* rightPage = new IndexPage();
		int midPoint = (int) BUCKET_MAX_ELEMENTS / 2;
		int i = 0;
		for (int x = midPoint + 1; x < page->size; x++) {
			rightPage->elements[i] = page->elements[x];
			rightPage->pointers[i] = page->pointers[x];
			rightPage->pointers[i + 1] = page->pointers[x + 1];
			rightPage->size++;
			i++;
		}
		Index* midElement = page->elements[midPoint];
		for (int x = midPoint; x < page->size; x++) {
			page->elements[x] = NULL;
			page->pointers[x +1] = NULL;
		}
		page->size -= (midPoint + 1);

		IndexPage* parentElement = page->parentElement;
		if (parentElement == NULL) {
			// Move all the elements to the right leaf
			IndexPage* rootPage = new IndexPage();
			_head = rootPage;
			_head->pointers[0] = page;
			page->parentElement = _head;
			_head->pointers[1] = rightPage;
			rightPage->parentElement = _head;
			_head->elements[0] = midElement;
			_head->_leaf = false;
			_head->size = 1;
		} else {
			int indexInserted = parentElement->add(midElement);
			parentElement->pointers[indexInserted + 1] = rightPage;
			parentElement->pointers[indexInserted] = page;
			rightPage->parentElement = parentElement;
			page->parentElement = parentElement;

			checkPage(parentElement);
		}
	}
}

std::list<Index*> BPlusIndex::find(FilterParser* parser) {
	return std::list<Index*>();
}

void IndexPage::debug() const {
	Logger* log = getLogger(NULL);

	log->debug("Page: %d", this);
	std::stringstream ss;
	for (int x = 0; x < size; x++) {
		if (pointers[x] == NULL) {
			ss << " (NULL) ";
		} else {
			ss << " (" << (long)pointers[x] << ") ";
		}
		ss << elements[x]->key->getString("_id");
	}
	if (pointers[size] != NULL) {
		ss << " (" << (long)pointers[size] << ") ";
	} else {
		ss << " (NULL) ";
	}
	std::string s = ss.str();
	log->debug("%s", s.c_str());

	for (int x = 0; x <= size; x++) {
		if (pointers[x] != NULL)
			pointers[x]->debug();
	}
}

