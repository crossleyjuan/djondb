#ifndef DBCONTROLLER_H
#define DBCONTROLLER_H

#include <map>
#include <vector>
#include <string>
#include "filterdefs.h"
#include "streammanager.h"
#include "controller.h"

class FileInputOutputStream;
class FileInputStream;
class BSONObj;
class BSONArrayObj;
class Command;
class Logger;
class FilterParser;
class Index;

class DBController: public Controller
{
    public:
        DBController();
        virtual ~DBController();

        void initialize();
        void initialize(std::string dataDir);
        void shutdown();

        const BSONObj* insert(const char* db, const char* ns, BSONObj* bson);
		  bool dropNamespace(const char* db, const char* ns);
        void update(const char* db, const char* ns, BSONObj* bson);
        void remove(const char* db, const char* ns, const char* documentId, const char* revision);
        BSONArrayObj* find(const char* db, const char* ns, const char* select, const char* filter) throw (ParseException);
        BSONObj* findFirst(const char* db, const char* ns, const char* select, const char* filter) throw (ParseException);
        BSONObj* readBSON(StreamType* stream);
		  std::vector<std::string>* dbs() const;
		  std::vector<std::string>* namespaces(const char* db) const;

		  static void fillRequiredFields(BSONObj* bson);
	 protected:

    private:
		  Logger* _logger;
		  bool _initialized;
		  std::string _dataDir;

	 private:
		  BSONArrayObj* findFullScan(const char* db, const char* ns, const char* select, FilterParser* parser) throw (ParseException);
		  void clearCache();
		  long checkStructure(BSONObj* bson);
		  void updateIndex(const char* db, const char* ns, Index* index, long filePos);
		  Index* findIndex(const char* db, const char* ns, BSONObj* bson);
		  void insertIndex(const char* db, const char* ns, BSONObj* bson, long filePos);
		  void writeBSON(StreamType* stream, BSONObj* obj);
};

#endif // DBCONTROLLER_H
