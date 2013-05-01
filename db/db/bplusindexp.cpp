// =====================================================================================
// 
//  @file:  bplusindexp.cpp
// 
//  @brief:  Implementation of persistent BPlusIndex
// 
//  @version:  1.0
//  @date:     04/27/2013 09:09:47 PM
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
#include "util.h"
#include "prioritycache.h"
#include "filterparser.h"
#include "expressionresult.h"
#include "memorystream.h"
#include "bsonoutputstream.h"
#include "bsoninputstream.h"
#include "buffermanager.h"
#include "buffer.h"
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

BPlusIndexP::BPlusIndexP(const char* fileName)
{
	_bufferManager = new BufferManager(fileName);

	initializeIndex();
}

void BPlusIndexP::initializeIndex() {
	int buffersCount = _bufferManager->buffersCount();

	if (buffersCount > 0) {
		loadIndex();
	} else {
		_head = new IndexPage();
		persistPage(_head);
	}
}

BPlusIndexP::~BPlusIndexP()
{
	if (_head)
	{
		delete _head;
	}

	delete _bufferManager;
	//delete _helperStream;
}

void BPlusIndexP::add(const BSONObj& elem, const djondb::string documentId, long filePos, long indexPos)
{
	Index* index = new Index();
	index->key = new BSONObj(elem);
	index->documentId = documentId;
	index->posData = filePos;
	index->indexPos = indexPos;
	insertIndexElement(_head, index);
}

Index* BPlusIndexP::find(BSONObj* const elem)
{
	//INDEXPOINTERTYPE key = elem->toChar();
	djondb::string key = elem->getDJString("_id");
	Index* result = findIndex(_head, key);

	//free(key);
	return result;
}

void BPlusIndexP::remove(const BSONObj& elem)
{
}

void BPlusIndexP::debug() {
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

bool IndexPage::isLoaded() const {
	return _loaded;
}

void IndexPage::setLoaded(bool loaded) {
	_loaded = loaded;
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

Index* BPlusIndexP::findIndex(IndexPage* start, djondb::string key) {
	Logger* log = getLogger(NULL);
	if (!start->isLoaded()) {
		start->loadPage(_bufferManager);
	}
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

IndexPage* BPlusIndexP::findIndexPage(IndexPage* start, djondb::string key) {
	Logger* log = getLogger(NULL);
	if (!start->isLoaded()) {
		start->loadPage(_bufferManager);
	}
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

void BPlusIndexP::insertIndexElement(IndexPage* page, Index* index) {
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
	_loaded = false;
	this->_bufferIndex = -1;
	this->_bufferPos = -1;
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

void BPlusIndexP::splitAddLeaf(IndexPage* page, Index* index) {
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
	// Clean up the elements moved to the rightPage
	for (int x = page->size; x < BUCKET_MAX_ELEMENTS; x++) {
		page->elements[x] = NULL;
		page->pointers[x] = NULL;
	}
	// cleans the last one
	page->pointers[BUCKET_MAX_ELEMENTS] = NULL;

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
		addElement(parentElement, copyElement, rightPage);
	}

	free(tmpelements);
	refreshParentRelationship(parentElement);

	persistPage(page);
	persistPage(rightPage);
	persistPage(page->parentElement);
}

void BPlusIndexP::splitAddInner(IndexPage* page, Index* index, IndexPage* rightPage) {
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
	// Clean up the elements moved to the rightPage
	for (int x = page->size; x < BUCKET_MAX_ELEMENTS; x++) {
		page->elements[x] = NULL;
		page->pointers[x] = NULL;
	}
	// cleans the last one
	page->pointers[BUCKET_MAX_ELEMENTS] = NULL;

	newRightPage->size = (BUCKET_MAX_ELEMENTS / 2) + 1;
	refreshParentRelationship(newRightPage);

	// Promotion
	IndexPage* parentElement = page->parentElement;
	Index* element = newRightPage->elements[0];

	if (parentElement == NULL) {
		createRoot(element, page, newRightPage);
		parentElement = _head;
	} else {
		addElement(parentElement, element, newRightPage);
	}
	shiftLeftArray((void**)newRightPage->elements, 0, 1, BUCKET_MAX_ELEMENTS - 1);
	shiftLeftArray((void**)newRightPage->pointers, 0, 1, BUCKET_MAX_ELEMENTS);
	newRightPage->size--;

	refreshParentRelationship(parentElement);
	free(tmpelements);
	free(tmppointers);

	persistPage(page);
	persistPage(newRightPage);
	persistPage(page->parentElement);
}

void BPlusIndexP::splitAdd(IndexPage* page, Index* index, IndexPage* rightPointer) {
	Logger* log = getLogger(NULL);

	if (page->isLeaf()) {
		assert(rightPointer == NULL);
		splitAddLeaf(page, index);
	} else {
		splitAddInner(page, index, rightPointer);
	}
}

void BPlusIndexP::addElement(IndexPage* page, Index* index, IndexPage* rightPointer) {
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
		persistPage(page);
	} else {
		splitAdd(page, index, rightPointer);
	}
}

int IndexPage::findInsertPosition(Index* index) const {
	return findInsertPositionArray(elements, index, size, BUCKET_MAX_ELEMENTS);
}

void BPlusIndexP::moveElements(IndexPage* source, IndexPage* destination, int startIndex, int endIndex) {
	copyElements(source, destination, startIndex, endIndex);
	removeElements(source, startIndex, endIndex);
}

void BPlusIndexP::copyElements(IndexPage* source, IndexPage* destination, int startIndex, int endIndex) {
	copyArray((void**)source->elements, (void**)destination->elements, startIndex, endIndex, 0);
	copyArray((void**)source->pointers, (void**)destination->pointers, startIndex, endIndex, 0);
	destination->size += (endIndex - startIndex);
}

void BPlusIndexP::removeElements(IndexPage* source, int startIndex, int endIndex) {
	removeArray((void**)source, startIndex, endIndex);
	source->size -= (endIndex - startIndex);
}

void BPlusIndexP::createRoot(Index* element, IndexPage* left, IndexPage* right) {
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

std::list<Index*> BPlusIndexP::find(FilterParser* parser) {
	std::list<Index*> result;

	if (_head != NULL) {
		std::list<Index*> partial = _head->find(_bufferManager, parser);
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


std::list<Index*> IndexPage::find(BufferManager* manager, FilterParser* parser) {
	if (!isLoaded()) {
		loadPage(manager);
	}
	std::list<Index*> result;
	for (int x = 0; x < size; x++) {
		BSONObj* key = elements[x]->key;
		if (key->getString("_id").compare("c597-43e1-ae9b-6f5451b28295") == 0) {
			cout << "Hey!" << endl;
		}
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
			std::list<Index*> inner = innerPage->find(manager, parser);
			result.insert(result.begin(), inner.begin(), inner.end());
		}
	}
	return result;
}

void persistIndexElement(MemoryStream* stream, Index* index) {
	BSONOutputStream* bos = new BSONOutputStream(stream);
	if (index != NULL) {
		stream->writeInt(1);
		bos->writeBSON(*index->key);
		stream->writeChars(index->documentId.c_str(), index->documentId.length());
		stream->writeLong(index->posData);
		stream->writeLong(index->indexPos);
	} else {
		stream->writeInt(0);
		bos->writeBSON(BSONObj());
		stream->writeChars("", 0);
		stream->writeLong(0);
		stream->writeLong(0);
	}
	delete bos;
}

Index* retrieveIndexElement(MemoryStream* stream) {
	BSONInputStream* bis = new BSONInputStream(stream);
	int i = stream->readInt();
	Index* index = NULL;
	if (i == 1) {
		index = new Index();
		BSONObj* key = bis->readBSON();
		index->key = key;
		std::string documentId(stream->readChars());
		index->documentId = djondb::string(documentId.c_str(), documentId.length());
		index->posData = stream->readLong();
		index->indexPos = stream->readLong();
	} else {
		delete bis->readBSON();
		free(stream->readChars());
		stream->readLong();
		stream->readLong();
	}
	delete bis;
	return index;
}

void persistPointer(MemoryStream* stream, IndexPage* pointer) {
	if ((pointer != NULL) && (pointer->bufferIndex() > -1)) {
		stream->writeInt(1);
		stream->writeInt(pointer->bufferIndex());
		stream->writeInt(pointer->bufferPos());
	} else {
		stream->writeInt(0);
		stream->writeInt(0);
		stream->writeInt(0);
	}
}

IndexPage* retrievePointer(MemoryStream* stream) {
	int flag = stream->readInt();

	IndexPage* result = NULL;
	if (flag == 1) {
		result = new IndexPage();
		result->setBufferIndex(stream->readInt());
		result->setBufferPos(stream->readInt());
	} else {
		stream->readInt();
		stream->readInt();
	}
	//assert((result == NULL) || (result->bufferIndex() > -1));
	return result;
}

void BPlusIndexP::loadIndex() {
	Buffer* buffer = _bufferManager->getBuffer(0);
	buffer->seek(0);

	__int32 index = buffer->readLong();
	__int64 pos = buffer->readLong();

	__int32 keysCount = buffer->readInt();
	for (int x = 0; x < keysCount; x++) {
		std::string* key = buffer->readString();
		_keys.insert(*key);
		delete key;
	}
	_head = new IndexPage();
	_head->setBufferIndex(index);
	_head->setBufferPos(pos);
	_head->loadPage(_bufferManager);
}

void BPlusIndexP::setKeys(std::set<std::string> keys) {
	IndexAlgorithm::setKeys(keys);
	Buffer* buffer = _bufferManager->getBuffer(0);

	buffer->seek(sizeof(__int64) + sizeof(__int64));
	buffer->writeInt(_keys.size());
	for (std::set<std::string>::iterator i = keys.begin(); i != keys.end(); i++) {
		buffer->writeString(*i);
	}
}

void IndexPage::loadPage(BufferManager* manager) {
	Buffer* buffer = manager->getBuffer(bufferIndex());
	buffer->seek(bufferPos());

	__int32 bsize = buffer->readInt();
	char* data = buffer->readChars();

	MemoryStream* helperStream = new MemoryStream(data, bsize);
	helperStream->seek(0);

	helperStream->readInt(); // BUCKET_MAX_ELEMENTS

	size = helperStream->readInt();

	IndexPage* tempPointer = NULL;
	for (int x = 0; x < BUCKET_MAX_ELEMENTS; x++) {
		Index* index = retrieveIndexElement(helperStream);
		elements[x] = index;

		tempPointer = retrievePointer(helperStream);
		pointers[x] = tempPointer;
		if (tempPointer != NULL) {
			tempPointer->parentElement = this;
		}
	}	

	// Recovers the last pointer
	tempPointer = retrievePointer(helperStream);
	pointers[BUCKET_MAX_ELEMENTS] = tempPointer;
	if (tempPointer != NULL) {
		tempPointer->parentElement = this;
	}

	// Recovers siblings 
	leftSibling = retrievePointer(helperStream);
	rightSibling = retrievePointer(helperStream);
	setLoaded(true);

	/* 
		for (int x = 0; x <= BUCKET_MAX_ELEMENTS; x++) {
		IndexPage* temp = page->pointers[x];
		if ((temp != NULL) && (temp->bufferIndex() > -1)) {
		loadPage(temp);
		}
		}
		*/
	delete helperStream;
}

void BPlusIndexP::persistPage(IndexPage* page) {
	//assert((page->size > 0) || (page->parentElement == NULL));
	Buffer* buffer = NULL;
	MemoryStream* helperStream = new MemoryStream();
	helperStream->seek(0);

	helperStream->writeInt(BUCKET_MAX_ELEMENTS);

	helperStream->writeInt(page->size);

	for (int x = 0; x < BUCKET_MAX_ELEMENTS; x++) {
		Index* index = page->elements[x];

		persistIndexElement(helperStream, index);

		IndexPage* pointer = page->pointers[x];
		persistPointer(helperStream, pointer);
	}	

	// Persist the last pointer
	persistPointer(helperStream, page->pointers[BUCKET_MAX_ELEMENTS]);

	// Persist siblings 
	persistPointer(helperStream, page->leftSibling);
	persistPointer(helperStream, page->rightSibling);

	char* pageContent = helperStream->toChars();
	__int32 size = helperStream->size();

	if (page->bufferIndex() > -1) {
		buffer = _bufferManager->getBuffer(page->bufferIndex());
		buffer->seek(page->bufferPos());
	} else {
		if (_bufferManager->buffersCount() > 1) {
			buffer = _bufferManager->getCurrentBuffer(size);
		} else {
			// The buffer 0 will have the head position
			// this will force the buffer to be created
			_bufferManager->getCurrentBuffer(size, true);
			// Now this will force to retrieve the buffer in pos 1
			buffer = _bufferManager->getCurrentBuffer(size, true);
		}
		page->setBufferIndex(buffer->bufferIndex());
		buffer->seek(0, FROMEND_SEEK);
		page->setBufferPos(buffer->currentPos());
	}
	buffer->writeInt(size);
	buffer->writeChars(pageContent, size);

	if (page->parentElement == NULL) {
		// head position will be recorded in the first buffer
		Buffer* controlBuffer = _bufferManager->getBuffer(0);
		controlBuffer->seek(0);
		controlBuffer->writeLong(page->bufferIndex());
		controlBuffer->writeLong(page->bufferPos());
	}

	page->setLoaded(true);

	delete helperStream;
}

