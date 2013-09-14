#ifndef FINDCOMMAND_H
#define FINDCOMMAND_H

#include "command.h"
#include "bson.h"
#include <vector>

class DBCursor;

class FindCommand : public Command
{
    public:
        /** Default constructor */
        FindCommand();
        /** Default destructor */
        virtual ~FindCommand();
        /** Copy constructor
         *  \param other Object to copy from
         */
        FindCommand(const FindCommand& other);

        virtual void execute();
        virtual void* result();

        virtual void writeCommand(OutputStream* out) const;
        virtual void writeResult(OutputStream* out) const;
        virtual void readResult(InputStream* is);

        void setDB(const std::string& db);
        const std::string* DB() const;
        void setNameSpace(const std::string& ns);
        std::string* nameSpace() const;
        void setSelect(const std::string& select);
		  std::string* select() const;
        void setFilter(const std::string& filter);
		  std::string* filter() const;

		  const char* cursorId() const;

		  /**
			* @brief This will return the current loaded array, if this is an old version prior 0.3.2 then it should
			* contain the full rows array
			*
			* @return bson array
			*/
		  BSONArrayObj* const arrayResult() const;
		  BSONArrayObj* const readedResult() const;
    protected:
    private:
    private:
		  std::string* _select;
        std::string* _namespace;
        std::string* _db;
		  std::string* _filter;

		  char* _cursorId; //!< This will store the id of the cursor returned by the find command, if the client is 0.3.1 or above
		  BSONArrayObj* _arrayresult; //!< Used on commands version 0.3.0 or below, if the client is newer than that then this will be NULL
		  BSONArrayObj* _readedResult; //!< contains the result readed of the incoming response 
};

#endif // FINDCOMMAND_H
