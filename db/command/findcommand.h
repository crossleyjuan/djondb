#ifndef FINDCOMMAND_H
#define FINDCOMMAND_H

#include "command.h"
#include "bson.h"
#include <vector>

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
    protected:
    private:
    private:
		  std::string* _select;
        std::string* _namespace;
        std::string* _db;
		  std::string* _filter;

		  BSONArrayObj* _findresult;
};

#endif // FINDCOMMAND_H
