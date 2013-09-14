#ifndef COMMAND_H_INCLUDED
#define COMMAND_H_INCLUDED

#include <string>
#include "util.h"

using namespace std;

class BSONObj;
class Controller;
class OutputStream;
class InputStream;

enum COMMANDTYPE {
    INSERT,
    UPDATE,
    FIND,
    CLOSECONNECTION,
	 DROPNAMESPACE,
    SHUTDOWN,
	 SHOWDBS,
	 SHOWNAMESPACES,
	 REMOVE,
	 COMMIT,
	 ROLLBACK,
	 FETCHCURSOR
};

/**
 * @brief Every command requested by the client driver will end in a command at the database level, this is the base class for all the supported commands
 * in djondb. The COMMANDTYPE enum will list all the possible options that djondb will support at the server side. Some commands might react different
 * based on the version of the client.
 */
class Command {
    public:
        Command(COMMANDTYPE commandType);

        Command(const Command& orig);
        virtual ~Command();

        virtual void execute() = 0;
        virtual void* result() = 0;

        virtual void writeCommand(OutputStream* out) const = 0;
        virtual void writeResult(OutputStream* out) const = 0;
        virtual void readResult(InputStream* is) = 0;

        Controller* dbController() const {
            return  _dbController;
        }

        void setDBController(Controller* dbController) {
            _dbController = dbController;
        }

        COMMANDTYPE commandType() const;
		  int resultCode() const {
			  return _resultCode;
		  }

		  std::string resultMessage() const {
			  return _resultMessage;
		  }

		  void setOptions(const BSONObj* options);

		  const BSONObj* options() const {
			  return _options;
		  }

		  /**
			* @brief Version of the command, some commands would react diferent depending on the version used by the client
			*
			* @param version
			*/
		  void setVersion(const Version& version) {
			  _version = new Version(version);
		  }

		  /**
			* @brief Version of the command, some commands would react diferent depending on the version used by the client
			*
			*/
		  const Version* version() const {
			  return _version;
		  }

	 protected:
		  void setResultCode(int rc) {
			  _resultCode = rc;
		  }

		  void setResultMessage(std::string message) {
			  _resultMessage = message;
		  }

		  int _resultCode;
		  std::string _resultMessage;

	 private:
		  COMMANDTYPE _commandType;
		  Controller* _dbController;
		  BSONObj* _options;
		  Version* _version;
};

class CloseCommand: public Command {
	public:
		CloseCommand();

		CloseCommand(const CloseCommand& orig);
		virtual ~CloseCommand();

		virtual void execute();

		virtual void* result() {
			return NULL;
		}

		virtual void writeCommand(OutputStream* out) const;
		virtual void writeResult(OutputStream* out) const;
		virtual void readResult(InputStream* is);
};



#endif // COMMAND_H_INCLUDED
