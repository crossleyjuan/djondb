#ifndef DBCONTROLLER_H
#define DBCONTROLLER_H

#include <map>
#include <vector>
#include <string>
#include "filterdefs.h"
#include "streammanager.h"

class FileInputOutputStream;
class FileInputStream;
class BSONObj;
class BSONArrayObj;
class Command;
class Logger;
class FilterParser;
class Index;

class DBController
{
    public:
        DBController();
        virtual ~DBController();

        void initialize();
        void initialize(std::string dataDir);
        void shutdown();


        BSONObj* insert(char* db, char* ns, BSONObj* bson);
		  bool dropNamespace(char* db, char* ns);
        void update(char* db, char* ns, BSONObj* bson);
        void remove(char* db, char* ns, const std::string& documentId, const std::string& revision);
        BSONArrayObj* find(char* db, char* ns, const char* select, const char* filter) throw (ParseException);
        BSONObj* findFirst(char* db, char* ns, const char* select, const char* filter) throw (ParseException);
        BSONObj* readBSON(StreamType* stream);
		  std::vector<std::string>* dbs() const;
		  std::vector<std::string>* namespaces(const char* db) const;

    private:
		  Logger* _logger;
		  bool _initialized;
		  std::string _dataDir;

	 private:
		  BSONArrayObj* findFullScan(char* db, char* ns, const char* select, FilterParser* parser) throw (ParseException);
		  void clearCache();
		  long checkStructure(BSONObj* bson);
		  void updateIndex(char* db, char* ns, Index* index, long filePos);
		  Index* findIndex(char* db, char* ns, BSONObj* bson);
		  void insertIndex(char* db, char* ns, BSONObj* bson, long filePos);
		  void writeBSON(StreamType* stream, BSONObj* obj);
};

#endif // DBCONTROLLER_H
