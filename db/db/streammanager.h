/*
 * =====================================================================================
 *
 *       Filename:  StreamManager.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/11/2012 11:08:36 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#include "dbfilestream.h"
#include "util.h"
#include <map>
#include <string>
#include <vector>

enum FILE_TYPE {
    DATA_FTYPE,
    STRC_FTYPE,
    INDEX_FTYPE
};

typedef DBFileStream StreamType;
struct Space {
	std::string ns;
	std::map<FILE_TYPE, StreamType*>* streams;
};
typedef Space SpacesType;

class StreamManager {
	public:
		StreamManager();
		virtual ~StreamManager();

		static StreamManager* getStreamManager();
		StreamType* open(std::string db, std::string ns, FILE_TYPE type);
		  std::vector<std::string>* dbs() const;
		  std::vector<std::string>* namespaces(const char* db) const;
		void saveDatabases();
		void closeDatabases();
		bool dropNamespace(char* db, char* ns);
		void setDataDir(const std::string& dataDir);

		void setInitializing(bool initializing);

	private:
		bool close(char* db, char* ns);
		StreamType* checkVersion(StreamType* stream);

	private:
		std::map<std::string, std::map<std::string, SpacesType>* > _spaces;
		std::string fileName(std::string ns, FILE_TYPE type) const;

		std::string _dataDir;

		static StreamManager* _manager;
		Logger* _logger;
		bool _initializing;
};

