/*
 * =====================================================================================
 *
 *       Filename:  transactioncontroller.h
 *
 *    Description: This class work as a front controller and bridge for any controller
 *                 operation that needs a transaction 
 *
 *        Version:  1.0
 *        Created:  09/26/2012 08:26:29 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Juan Pablo Crossley (Cross), crossleyjuan@gmail.com
 *   Organization:  djondb
 *
 * This file is part of the djondb project, for license information please refer to the LICENSE file,
 * the application and libraries are provided as-is and free of use under the terms explained in the file LICENSE
 * Its authors create this application in order to make the world a better place to live, but you should use it on
 * your own risks.
 * 
 * Also, be adviced that, the GPL license force the committers to ensure this application will be free of use, thus
 * if you do any modification you will be required to provide it for free unless you use it for personal use (you may 
 * charge yourself if you want), bare in mind that you will be required to provide a copy of the license terms that ensures
 * this program will be open sourced and all its derivated work will be too.
 * =====================================================================================
 */

#ifndef TRANSACTIONCONTROLLER_INCLUDED_H
#define TRANSACTIONCONTROLLER_INCLUDED_H

#include "filterdefs.h"
#include "memorystream.h"
#include "controller.h"

#include <string>
#include <vector>
#include <map>

class BSONObj;
class FileInputOutputStream;
class FileInputStream;
class FileOutputStream;
class Command;

class TransactionController: public Controller 
{
	public:
		TransactionController(Controller* controller);
		TransactionController(Controller* controller, std::string transactionId);
		TransactionController(const TransactionController& orig);
		~TransactionController();

		void loadControlFile();
		virtual BSONObj* insert(char* db, char* ns, BSONObj* bson);
		virtual bool dropNamespace(char* db, char* ns);
		virtual void update(char* db, char* ns, BSONObj* bson);
		virtual void remove(char* db, char* ns, const std::string& documentId, const std::string& revision);
		virtual std::vector<BSONObj*>* find(char* db, char* ns, const char* select, const char* filter) throw (ParseException);
		virtual BSONObj* findFirst(char* db, char* ns, const char* select, const char* filter) throw (ParseException);
		virtual std::vector<std::string>* dbs() const;
		virtual std::vector<std::string>* namespaces(const char* db) const;

	private:
		Controller* _controller;
		std::string* _transactionId;
		std::string _dataDir;

		// This contains all the NORMAL records
		std::map<std::string, std::map<std::string, std::vector<BSONObj*> > > _elements;

		FileInputOutputStream* _controlFile;

		enum OPERATION_STATUS {
			DIRTY, // in writing process, it's not trustable yet
			NORMAL, // the record it's ok and it's trustable
			REMOVED, // rollbacked or any other state, should not be used
			DONE // Moved to the main data repository, ready to be dropped.
		};

		struct Operation {
			OPERATION_STATUS status;
			Command* command;	
		};

		struct Control {
			long startPos; // Contains the first valid position to be used on the first logFile
			long lastValidPos; // this is a reference to the last valid position in the last logFile
			std::vector<FileInputOutputStream*> logFiles;
			FileInputOutputStream* currentFile;
		};	

		Control _control;

		enum TRANSACTION_OPER {
			INSERT,
			DROPNAMESPACE,
			UPDATE,
			DELETERECORD
		};

	private:
		void checkState();
		void writeCommandToRegister(char* db, char* ns, Command* cmd);
		Command* readCommandFromRegister(char* db, char* ns);
		std::vector<Command*>* findCommands(char* db, char* ns);
};

#endif // TRANSACTIONCONTROLLER_INCLUDED_H
