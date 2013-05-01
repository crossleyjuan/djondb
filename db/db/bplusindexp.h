// =====================================================================================
// 
//  @file:  bplusindexp.h
// 
//  @brief:  This is the definition of the class BPluisIndexP, the implementation of persistent b++ tree
// 
//  @version:  1.0
//  @date:     04/27/2013 09:08:02 PM
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

#ifndef BPLUSINDEXP_H
#define BPLUSINDEXP_H

#include "index.h"
#include <list>

//typedef char* INDEXPOINTERTYPE;
typedef const char* INDEXPOINTERTYPE;

#define COMPAREKEYS(k1, k2) \
	(strcmp(k1, k2) == 0);

const int BUCKET_MAX_ELEMENTS = 5; // Should be even (3, 5, 7)

class BufferManager;
class MemoryStream;
class Buffer;

/**
 * @brief This class contains the implementation 
 */
class IndexPage {
	public:
		IndexPage();
		~IndexPage();

		/**
		 * @brief This contains the elements of the currentpage
		 */
		Index** elements;
		/**
		 * @brief This contains the pointers to other pages that are in between the elements
		 */
		IndexPage** pointers;

		/**
		 * @brief Pointer to the page at the right
		 */
		IndexPage* rightSibling;
		/**
		 * @brief Pointer to the page at the left
		 */
		IndexPage* leftSibling;

		/**
		 * @brief Pointer to the IndexPage that is in top of this IndexPage
		 */
		IndexPage* parentElement;

		void debugElements() const;
		void debug() const;
		bool isLeaf() const;
		bool isFull() const;
		bool _leaf;
		std::list<Index*> find(BufferManager* manager, FilterParser* parser);
		int findInsertPosition(Index* index) const;

		void moveElements(int startPoint, int count);
		void movePointers(int startPoint, int count);

		__int32 bufferIndex() const {
			return _bufferIndex;
		}

		void setBufferIndex(__int32 bufferIndex) {
			_bufferIndex = bufferIndex;
		}

		__int32 bufferPos() const {
			return _bufferPos;
		}

		void setBufferPos(__int32 pos) {
			_bufferPos = pos;
		}

		int size;
		/**
		 * @brief The property loaded indicates if the elements are ready to be read or the
		 * page needs to be loaded from disk
		 *
		 * @return 
		 */
		bool isLoaded() const;
		/**
		 * @brief The property loaded indicates if the elements are ready to be read or the
		 * page needs to be loaded from disk
		 *
		 * @param loaded
		 */
		void setLoaded(bool loaded);

		void loadPage(BufferManager* manager);
	private:
		__int32 _bufferIndex; /* this contains the index of the buffer where is persisted */
		__int32 _bufferPos; /* This contains the position within the buffer */
		bool _loaded;
};

/**
 * @brief BPlusIndexP implementation, here the BPlusTree is persisted in disc using Buffers
 */
class BPlusIndexP: public IndexAlgorithm
{
	public:
		BPlusIndexP(const char* fileName);
		virtual ~BPlusIndexP();

		virtual void add(const BSONObj& elem, djondb::string documentId, long filePos, long indexPos);
		virtual Index* find(BSONObj* const elem);
		virtual void remove(const BSONObj& elem);
		virtual std::list<Index*> find(FilterParser* parser);

		void debug();
		virtual void setKeys(std::set<std::string> keys);
		
	protected:
	private:
		IndexPage* _head;
		BufferManager* _bufferManager;

	private:
		IndexPage* findIndexPage(IndexPage* start, djondb::string key);
		Index* findIndex(IndexPage* start, djondb::string key);
		void insertIndexElement(IndexPage* page, Index* index);
		void dispose(IndexPage* page);
		void createRoot(Index* element, IndexPage* left, IndexPage* right);
		void addElement(IndexPage* page, Index* element, IndexPage* rightPointer);
		void splitAdd(IndexPage* page, Index* index, IndexPage* rightPointer);
		void splitAddLeaf(IndexPage* page, Index* index);
		void splitAddInner(IndexPage* page, Index* index, IndexPage* rightPointer);

		void copyElements(IndexPage* source, IndexPage* destination, int startIndex, int endIndex);
		void removeElements(IndexPage* source, int startIndex, int endIndex);
		void moveElements(IndexPage* source, IndexPage* destination, int startIndex, int endIndex);
		void initializeIndex();
		void persistPage(IndexPage* page);

		void loadIndex();
};

#endif // BPLUSINDEXP_H
