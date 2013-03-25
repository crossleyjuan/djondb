#ifndef ROLLBACKCOMMAND_H 
#define ROLLBACKCOMMAND_H

#include "command.h"

class RollbackCommand: public Command {
    public:
        RollbackCommand();

        virtual ~RollbackCommand();
        RollbackCommand(const RollbackCommand& orig);

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

#endif // ROLLBACKCOMMAND_H
