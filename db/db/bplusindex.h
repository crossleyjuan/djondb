#ifndef BPLUSINDEX_H
#define BPLUSINDEX_H

#include "index.h"
#include <list>

template <class K, class V>
class PriorityCache;

#include <boost/shared_ptr.hpp>

typedef char* INDEXPOINTERTYPE;

#define COMPAREKEYS(k1, k2) \
	(strcmp(k1, k2) == 0);

const int BUCKET_MAX_ELEMENTS = 3; // Should be even (3, 5, 7)

class IndexPage;

class IndexPage {
	public:
		IndexPage();
		int size;

		Index** elements;
		IndexPage** pointers;

		IndexPage* parentElement;

		void debug() const;
		bool isLeaf() const;
		bool isFull() const;
		int add(Index* index);
		bool _leaf;
};

class BPlusIndex: public IndexAlgorithm
{
	public:
		BPlusIndex(std::set<std::string> keys);
		virtual ~BPlusIndex();

		virtual void add(const BSONObj& elem, std::string documentId, long filePos, long indexPos);
		virtual Index* find(BSONObj* const elem);
		virtual void remove(const BSONObj& elem);
		virtual std::list<Index*> find(FilterParser* parser);

		void debug();
		
		void checkPage(IndexPage* page);
	protected:
	private:
		IndexPage* _head;

	private:
		IndexPage* findIndexPage(IndexPage* start, INDEXPOINTERTYPE key) const;
		void insertIndexElement(IndexPage* page, Index* index);
		/*  
			 bool insertElement(const Index& elem);
			 BucketElement* findBucketElement(Bucket* start, const Index& idx, bool create);
			 void initializeBucket(Bucket* const element);
			 void initializeBucketElement(BucketElement* const elem);

			 void insertBucketElement(Bucket* bucket, BucketElement* element);
			 void checkBucket(Bucket* const bucket);

			 std::list<Index*> find(FilterParser* parser, Bucket* bucket);
			 std::list<Index*> findElements(FilterParser* parser, Bucket* bucket);
			 */
};

#endif // BPLUSINDEX_H
