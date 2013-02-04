#ifndef DJONDB_CONNECTION_H
#define DJONDB_CONNECTION_H

#include <string>
#include <vector>
#include "bson.h"
#include "util.h"
#include "filterparser.h"

#ifdef WINDOWS
   #define LibraryExport   __declspec( dllexport )
#else
   #define LibraryExport
#endif

class NetworkOutputStream;
class NetworkInputStream;
class CommandWriter;
class Transaction;

#define SERVER_PORT 1243

namespace djondb {

    class LibraryExport DjondbConnection
    {
        public:
            /** Default constructor */
            DjondbConnection(std::string host);
            DjondbConnection(std::string host, int port);
            DjondbConnection(const DjondbConnection& orig);

            /** Default destructor */
            virtual ~DjondbConnection();

            bool open();
            void close();
            void internalClose();
            bool isOpen() const;

				bool shutdown() const;

				bool insert(const std::string& db, const std::string& ns, const std::string& json);
            bool insert(const std::string& db, const std::string& ns, const BSONObj& obj);
            BSONObj* findByKey(const std::string& db, const std::string& ns, const std::string& select, const std::string& id);
            BSONObj* findByKey(const std::string& db, const std::string& ns, const std::string& id);
				BSONArrayObj* find(const std::string& db, const std::string& ns) throw(ParseException);
				BSONArrayObj* find(const std::string& db, const std::string& ns, const std::string& filter) throw(ParseException);
				BSONArrayObj* find(const std::string& db, const std::string& ns, const std::string& select, const std::string& filter) throw(ParseException);
            bool update(const std::string& db, const std::string& ns, const std::string& json);
            bool update(const std::string& db, const std::string& ns, const BSONObj& bson);
            bool remove(const std::string& db, const std::string& ns, const std::string& id, const std::string& revision);

				bool dropNamespace(const std::string& db, const std::string& ns);
				std::vector<std::string>* dbs() const;
				std::vector<std::string>* namespaces(const std::string& db) const;

				std::string host() const;

		  protected:
		  private:
				NetworkOutputStream*  _outputStream;
				NetworkInputStream*   _inputStream;
				CommandWriter*        _commandWriter;

				std::string _host;
				int _port;
				bool _open;
				Logger* _logger;
	 };

}

#endif // DJONDB_CONNECTION_H
