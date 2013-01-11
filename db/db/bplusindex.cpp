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

void shiftRightArray(void** array, int startPoint, int count, int size) {
	for (int i = 0; i < count; i++) {
		for (int x = size - 2; x > startPoint - 1; x--) {
			array[x + 1] = array[x]; array[x] = NULL;
		}
	}
}

void shiftLeftArray(void** array, int startPoint, int count, int size) {
	for (int i = 0; i < count; i++) {
		for (int x = startPoint + 1; x < size; x++) {
			array[x - 1] = array[x];
			array[x] = NULL;
		}
	}
}

void insertArray(void** array, void* element, int pos, int length) {
	shiftRightArray(array, pos, 1, length);
	array[pos] = element;
}

int findInsertPositionArray(Index** elements, Index* index, int len, int size) {
	Logger* log = getLogger(NULL);
	int indexPositionResult = 0;
	if (len == 0) {
		return 0;
	} else {
		int res;
//		INDEXPOINTERTYPE key = index->key->toChar();
		djondb::string key = index->key->getDJString("_id");
		bool found = false;
		for (int x = 0; x < size; x++) {
			Index* current = elements[x];
			indexPositionResult = x;
			if (current == NULL) {
				found = true;
				break;
			} else {
	djondb::string currentKey = current->key->getDJString("_id");
	//			INDEXPOINTERTYPE currentKey = current->key->toChar();
				int res = currentKey.compare(key); 
				//free(currentKey);
				if (res < 0) {
					found = true;
					break;
				}
			}
		}
		if (!found) {
			indexPositionResult++;
		}
		//free(key);
	}
	return indexPositionResult;
}

void initializeArray(void** array, int size) {
	for (int x = 0; x < size; x++) {
		array[x] = NULL;
	}
}	

void copyArray(void** source, void** destination, int startIndex, int endIndex, int offset) {
	int i = offset;
	for (int x = startIndex; x <= endIndex; x++) {
		destination[i] = source[x];
		i++;
	}
}

void removeArray(void** source, int startIndex, int endIndex) {
	for (int x = startIndex; x <= endIndex; x++) {
		source[x] = NULL;
	}
}

Index::~Index() {
	delete key;
}

Index::Index(const Index& orig) {
	this->key = new BSONObj(*orig.key);
	this->documentId = orig.documentId;
	this->posData = orig.posData;
	this->indexPos = orig.indexPos;
}

BPlusIndex::BPlusIndex(std::set<std::string> keys): IndexAlgorithm(keys)
{
	_head = new IndexPage();
}

BPlusIndex::~BPlusIndex()
{
	if (_head)
	{
		delete _head;
	}
}

void BPlusIndex::add(const BSONObj& elem, const djondb::string documentId, long filePos, long indexPos)
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
	//INDEXPOINTERTYPE key = elem->toChar();
	djondb::string key = elem->getDJString("_id");
	Index* result = findIndex(_head, key);

	//free(key);
	return result;
}

void BPlusIndex::remove(const BSONObj& elem)
{
}

void BPlusIndex::debug() {
	_head->debug();
}

bool IndexPage::isLeaf() const {
	for (int x = 0; x <= size; x++) {
		if (pointers[x] != NULL) {
			return false;
		}
	}
	return true;
}

void IndexPage::movePointers(int startPoint, int count) {
	shiftRightArray((void**)pointers, startPoint, count, BUCKET_MAX_ELEMENTS + 1);
}

void IndexPage::moveElements(int startPoint, int count) {
	shiftRightArray((void**)elements, startPoint, count, BUCKET_MAX_ELEMENTS);
}

bool IndexPage::isFull() const {
	return size >= BUCKET_MAX_ELEMENTS;
}

Index* BPlusIndex::findIndex(IndexPage* start, djondb::string key) const {
	Logger* log = getLogger(NULL);
	for (int x = 0; x < start->size; x++) {
		Index* current = start->elements[x];
		//INDEXPOINTERTYPE testKey = current->key->toChar();
		djondb::string testKey = current->key->getDJString("_id");
		int result = testKey.compare(key);
		//free(testKey);

		if (result < 0) {
			if (start->pointers[x] != NULL) {
				return findIndex(start->pointers[x], key);
			} else {
				return NULL;
			}
		} if (result == 0) {
			return current;
		}
	}
	if (start->pointers[start->size] != NULL) {
		return findIndex(start->pointers[start->size], key);
	} else {
		return NULL;
	}
}

IndexPage* BPlusIndex::findIndexPage(IndexPage* start, djondb::string key) const {
	Logger* log = getLogger(NULL);
	if (start->isLeaf()) {
		return start;
	} else {
		for (int x = 0; x < start->size; x++) {
			Index* current = start->elements[x];
			//INDEXPOINTERTYPE testKey = current->key->toChar();
			djondb::string testKey = current->key->getDJString("_id");
			int result = testKey.compare(key);
			//free(testKey);

			if (result < 0) {
				if (start->pointers[x] != NULL) {
					return findIndexPage(start->pointers[x], key);
				} else {
					return start;
				}
			}
		}
		if (start->pointers[start->size] != NULL) {
			return findIndexPage(start->pointers[start->size], key);
		} else {
			return start;
		}
	}
}


void refreshParentRelationship(IndexPage* page) {
	for (int x = 0; x < BUCKET_MAX_ELEMENTS + 1; x++) {
		if (page->pointers[x] != NULL) {
			IndexPage* child = page->pointers[x];
			child->parentElement = page;
		}
	}
}

void BPlusIndex::insertIndexElement(IndexPage* page, Index* index) {
	Logger* log = getLogger(NULL);

	djondb::string key = index->key->getDJString("_id");
	//char* key = index->key->toChar();
	IndexPage* pageFound = findIndexPage(_head, key);

	addElement(pageFound, index, NULL);
	//free(key);
}

IndexPage::IndexPage() {
	parentElement = NULL;
	size = 0;
	_leaf = true;
	leftSibling = NULL;
	rightSibling = NULL;
	elements = (Index**)malloc(BUCKET_MAX_ELEMENTS * sizeof(Index*));
	for (int x = 0; x < BUCKET_MAX_ELEMENTS; x++) {
		elements[x] = NULL;
	}
	pointers = (IndexPage**)malloc((BUCKET_MAX_ELEMENTS + 1) * sizeof(IndexPage*));
	for (int x = 0; x < BUCKET_MAX_ELEMENTS + 1; x++) {
		pointers[x] = NULL;
	}
}

IndexPage::~IndexPage() {
	Logger* log = getLogger(NULL);
	if (log->isDebug()) {
		log->debug("Deleting page: %d", (long)this);

		for (int x = 0; x < size; x++) {
			if (elements[x] != NULL) {
				log->debug("Deleting element: %d", (long)elements[x]);
				delete elements[x];
				elements[x] = NULL;
			}
		}
		for (int x = 0; x < size + 1; x++) {
			if (pointers[x] != NULL) {
				IndexPage* page = pointers[x];
				delete page;
				pointers[x] = NULL;
			}
		}
		free(elements);
		free(pointers);
	}
}

void BPlusIndex::splitAddLeaf(IndexPage* page, Index* index) {
	Logger* log = getLogger(NULL);

	//temporal arrays
	Index** tmpelements = (Index**)malloc(sizeof(Index*) * (BUCKET_MAX_ELEMENTS + 1));
	//IndexPage** tmppointers = (IndexPage**)malloc(sizeof(IndexPage*) * (BUCKET_MAX_ELEMENTS + 2));

	initializeArray((void**)tmpelements, BUCKET_MAX_ELEMENTS);
	copyArray((void**)page->elements, (void**)tmpelements, 0, BUCKET_MAX_ELEMENTS - 1, 0);

	int posToInsert = findInsertPositionArray(tmpelements, index, page->size, BUCKET_MAX_ELEMENTS);

	insertArray((void**)tmpelements, index, posToInsert, BUCKET_MAX_ELEMENTS + 1);

	// clean the previous "left"
	initializeArray((void**)page->elements, BUCKET_MAX_ELEMENTS);

	IndexPage* rightPage = new IndexPage();
	int midPoint = (BUCKET_MAX_ELEMENTS / 2);
	copyArray((void**)tmpelements, (void**)page->elements, 0, midPoint, 0);
	page->size = (BUCKET_MAX_ELEMENTS / 2) + 1;
	//copyArray((void**)tmppointers, (void**)page->pointers, 0, midPoint + 1, 0);
	copyArray((void**)tmpelements, (void**)rightPage->elements, midPoint + 1, BUCKET_MAX_ELEMENTS, 0);
	rightPage->size = (BUCKET_MAX_ELEMENTS / 2) + 1;
	refreshParentRelationship(rightPage);
	//copyArray((void**)tmppointers, (void**)rightPage->pointers, midPoint + 2, BUCKET_MAX_ELEMENTS + 2, 0);

	// Promotion
	IndexPage* parentElement = page->parentElement;
	Index* copyElement = new Index(*rightPage->elements[0]);
	if (parentElement == NULL) {
		createRoot(copyElement, page, rightPage);
		parentElement = _head;
	} else {
		int pos = addElement(parentElement, copyElement, rightPage);
	}

	free(tmpelements);
	refreshParentRelationship(parentElement);
}

void BPlusIndex::splitAddInner(IndexPage* page, Index* index, IndexPage* rightPage) {
	Logger* log = getLogger(NULL);

	//temporal arrays
	Index** tmpelements = (Index**)malloc(sizeof(Index*) * (BUCKET_MAX_ELEMENTS + 1));
	IndexPage** tmppointers = (IndexPage**)malloc(sizeof(IndexPage*) * (BUCKET_MAX_ELEMENTS + 2));

	initializeArray((void**)tmpelements, BUCKET_MAX_ELEMENTS);
	initializeArray((void**)tmppointers, BUCKET_MAX_ELEMENTS + 1);

	copyArray((void**)page->elements, (void**)tmpelements, 0, BUCKET_MAX_ELEMENTS - 1, 0);
	copyArray((void**)page->pointers, (void**)tmppointers, 0, BUCKET_MAX_ELEMENTS, 0);

	int posToInsert = findInsertPositionArray(tmpelements, index, page->size, BUCKET_MAX_ELEMENTS);

	insertArray((void**)tmpelements, index, posToInsert, BUCKET_MAX_ELEMENTS + 1);
	insertArray((void**)tmppointers, rightPage, posToInsert + 1, BUCKET_MAX_ELEMENTS + 2);

	// clean the previous "left"
	initializeArray((void**)page->elements, BUCKET_MAX_ELEMENTS);
	initializeArray((void**)page->pointers, BUCKET_MAX_ELEMENTS + 1);

	IndexPage* newRightPage = new IndexPage();
	int midPoint = (BUCKET_MAX_ELEMENTS / 2);
	copyArray((void**)tmpelements, (void**)page->elements, 0, midPoint, 0);
	copyArray((void**)tmppointers, (void**)page->pointers, 0, midPoint + 1, 0);

	page->size = (BUCKET_MAX_ELEMENTS / 2) + 1;
	copyArray((void**)tmpelements, (void**)newRightPage->elements, midPoint + 1, BUCKET_MAX_ELEMENTS, 0);
	copyArray((void**)tmppointers, (void**)newRightPage->pointers, midPoint + 2, BUCKET_MAX_ELEMENTS + 1, 1);

	newRightPage->size = (BUCKET_MAX_ELEMENTS / 2) + 1;
	refreshParentRelationship(newRightPage);

	// Promotion
	IndexPage* parentElement = page->parentElement;
	Index* element = newRightPage->elements[0];

	if (parentElement == NULL) {
		createRoot(element, page, newRightPage);
		parentElement = _head;
	} else {
		int pos = addElement(parentElement, element, newRightPage);
	}
	shiftLeftArray((void**)newRightPage->elements, 0, 1, BUCKET_MAX_ELEMENTS - 1);
	shiftLeftArray((void**)newRightPage->pointers, 0, 1, BUCKET_MAX_ELEMENTS);
	newRightPage->size--;

	refreshParentRelationship(parentElement);
	free(tmpelements);
	free(tmppointers);
}

void BPlusIndex::splitAdd(IndexPage* page, Index* index, IndexPage* rightPointer) {
	Logger* log = getLogger(NULL);

	if (page->isLeaf()) {
		assert(rightPointer == NULL);
		splitAddLeaf(page, index);
	} else {
		splitAddInner(page, index, rightPointer);
	}
}

int BPlusIndex::addElement(IndexPage* page, Index* index, IndexPage* rightPointer) {
	if (!page->isFull()) {
		int pos = page->findInsertPosition(index);
		page->moveElements(pos, 1);
		page->movePointers(pos + 1, 1);
		page->elements[pos] = index;
		page->pointers[pos + 1] = rightPointer;
		page->size++;
		if (rightPointer != NULL) {
			rightPointer->parentElement = page;
		}
	} else {
		splitAdd(page, index, rightPointer);
	}
}

int IndexPage::findInsertPosition(Index* index) const {
	return findInsertPositionArray(elements, index, size, BUCKET_MAX_ELEMENTS);
}

void BPlusIndex::moveElements(IndexPage* source, IndexPage* destination, int startIndex, int endIndex) {
	copyElements(source, destination, startIndex, endIndex);
	removeElements(source, startIndex, endIndex);
}

void BPlusIndex::copyElements(IndexPage* source, IndexPage* destination, int startIndex, int endIndex) {
	copyArray((void**)source->elements, (void**)destination->elements, startIndex, endIndex, 0);
	copyArray((void**)source->pointers, (void**)destination->pointers, startIndex, endIndex, 0);
	destination->size += (endIndex - startIndex);
}

void BPlusIndex::removeElements(IndexPage* source, int startIndex, int endIndex) {
	removeArray((void**)source, startIndex, endIndex);
	source->size -= (endIndex - startIndex);
}

void BPlusIndex::createRoot(Index* element, IndexPage* left, IndexPage* right) {
	// Move all the elements to the right leaf
	IndexPage* rootPage = new IndexPage();
	_head = rootPage;
	_head->pointers[0] = left;
	left->parentElement = _head;
	_head->pointers[1] = right;
	right->parentElement = _head;
	_head->elements[0] = element;
	_head->_leaf = false;
	_head->size = 1;
}

std::list<Index*> BPlusIndex::find(FilterParser* parser) {
	std::list<Index*> result;

	if (_head != NULL) {
		std::list<Index*> partial = _head->find(parser);
		result.insert(result.begin(), partial.begin(), partial.end());
	}

	return result;
}

void IndexPage::debugElements() const {
	Logger* log = getLogger(NULL);
	if (log->isDebug()) {

		std::stringstream ss;
		for (int x = 0; x < size; x++) {
			if (pointers[x] == NULL) {
				ss << " (NULL) ";
			} else {
				ss << " (" << (long)pointers[x] << ") ";
			}
			if (elements[x] != NULL) {
				ss << " <<" << (long) elements[x] << ">> " << (const char*)elements[x]->key->getDJString("_id");
			} else {
				ss << " << NULL >> ";
			}
		}
		if (pointers[size] != NULL) {
			ss << " (" << (long)pointers[size] << ") ";
		} else {
			ss << " (NULL) ";
		}
		std::string s = ss.str();
		log->debug("%s", s.c_str());
	}
}

void IndexPage::debug() const {
	Logger* log = getLogger(NULL);
	if (log->isDebug()) {

		if (parentElement != NULL) {
			log->debug("Page: %d, parentPage: %d", this, parentElement);
		} else {
			log->debug("Page: %d", this);
		}

		debugElements();

		for (int x = 0; x <= size; x++) {
			if (pointers[x] != NULL)
				pointers[x]->debug();
		}
	}
}


std::list<Index*> IndexPage::find(FilterParser* parser) const {
	std::list<Index*> result;
	for (int x = 0; x < size; x++) {
		BSONObj* key = elements[x]->key;
		bool match = false;
		ExpressionResult* expresult = parser->eval(*key);
		if (expresult->type() == ExpressionResult::RT_BOOLEAN) {
			match = *expresult;
		}
		delete expresult;
		if (match) {
			result.push_back(elements[x]);
		}
	}
	for (int x = 0; x <= size; x++) {
		IndexPage* innerPage = pointers[x];
		if (innerPage != NULL) {
			std::list<Index*> inner = innerPage->find(parser);
			result.insert(result.begin(), inner.begin(), inner.end());
		}
	}
	return result;
}

