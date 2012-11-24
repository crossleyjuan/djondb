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

struct BucketElement {
    Index* index;

    INDEXPOINTERTYPE key;

    BucketElement* previous;
    BucketElement* next;
};

struct Bucket {
    int size;
    INDEXPOINTERTYPE minKey;
    INDEXPOINTERTYPE maxKey;

    BucketElement* root;
    BucketElement* tail;

    Bucket* left;
    Bucket* right;

    Bucket* parentBucket;
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
	 protected:
	 private:
		  Bucket* _head;
//		  PriorityCache<INDEXPOINTERTYPE, Index*>* _priorityCache;

	 private:
		  bool insertElement(const Index& elem);
		  BucketElement* findBucketElement(Bucket* start, const Index& idx, bool create);
		  void initializeBucket(Bucket* const element);
		  void initializeBucketElement(BucketElement* const elem);

		  void insertBucketElement(Bucket* bucket, BucketElement* element);
		  void checkBucket(Bucket* const bucket);

		  std::list<Index*> find(FilterParser* parser, Bucket* bucket);
		  std::list<Index*> findElements(FilterParser* parser, Bucket* bucket);
};

#endif // BPLUSINDEX_H
