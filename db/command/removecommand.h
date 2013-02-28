#ifndef REMOVECOMMAND_H
#define REMOVECOMMAND_H

#include "command.h"
#include "bson.h"

class RemoveCommand: public Command {
    public:
        RemoveCommand();

        virtual ~RemoveCommand();
        RemoveCommand(const RemoveCommand& orig);

        virtual void execute();
        virtual void* result();
        virtual void writeCommand(OutputStream* out) const;
        virtual void writeResult(OutputStream* out) const;
        virtual void readResult(InputStream* is);

        void setDB(const std::string& db);
        const std::string* DB() const;
        void setNameSpace(const std::string& ns);
        const std::string* nameSpace() const;
        void setId(const std::string& id);
		  const std::string* id() const;
        void setRevision(const std::string& revision);
		  const std::string* revision() const;
    private:
        const std::string* _namespace;
        const std::string* _db;
        const std::string* _id;
        const std::string* _revision;

};

#endif // REMOVECOMMAND_H
