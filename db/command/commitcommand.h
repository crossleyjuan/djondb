#ifndef COMMITCOMMANDCOMMAND_H 
#define COMMITCOMMANDCOMMAND_H

#include "command.h"
#include "bson.h"

class CommitCommand: public Command {
    public:
        CommitCommand();

        virtual ~CommitCommand();
        CommitCommand(const CommitCommand& orig);

        virtual void execute();
        virtual void* result();
        virtual void writeCommand(OutputStream* out) const;
        virtual void writeResult(OutputStream* out) const;
        virtual void readResult(InputStream* is);

        void setTransactionId(const std::string& txId);
        const std::string* transactionId() const;
    private:
        const std::string* _transactionId;
		  bool _result;
};

#endif // COMMITCOMMANDCOMMAND_H
