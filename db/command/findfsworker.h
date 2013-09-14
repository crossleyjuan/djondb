// =====================================================================================
// 
//  @file:  findworker.h
// 
//
//  @version:  1.0
//  @date:     07/10/2013 01:23:19 PM
//  Compiler:  g++
// 
//  @author:  Juan Pablo Crossley (Cross), crossleyjuan@gmail.com
// 
// This file is part of the djondb project, for license information please refer to the LICENSE file,
// the application and libraries are provided as-is and free of use under the terms explained in the file LICENSE
// Its authors create this application in order to make the world a better place to live, but you should use it on
// your own risks.
// 
// Also, be adviced that, the GPL license force the committers to ensure this application will be free of use, thus
// if you do any modification you will be required to provide it for free unless you use it for personal use (you may 
// charge yourself if you want), bare in mind that you will be required to provide a copy of the license terms that ensures
// this program will be open sourced and all its derivated work will be too.
// =====================================================================================
#ifndef FINDWORKER_INCLUDED_H
#define FINDWORKER_INCLUDED_H 

#include "worker.h"
#include "util.h"

class Logger;
class BSONObj;
class BSONBufferedObj;
class MMapInputStream;
class FilterParser;
class BSONArrayObj;
class FindCommand;
class InputStream;
class OutputStream;
class BSONOutputStream;
class Command;

/**
 *  @brief Implementation of the find full scan worker class, this will hold every state required
 *  to pause and resume full scan operations, it will not block any other action.
 *
 *  This is intended to be used by commands, but it can be used on any other class that could
 *  require pausing and resuming full scans
 */
class FindFullscanWorker: public Worker {
	public:
		/// @brief Default constructor 
		///
		/// @param db
		/// @param ns
		/// @param select
		/// @param filter
		/// @param options
		FindFullscanWorker(Command* command, InputStream* input, OutputStream* output);
				//const char* db, const char* ns, const char* select, const char* filter, const BSONObj* options);
		FindFullscanWorker(const FindFullscanWorker& orig);
		virtual ~FindFullscanWorker();

		/// @brief This method executes steps in the current taks
		/// the client should check the state to see if the task state
		virtual void resume();

		/// @brief This should return the results of the worker
		/// If the worker is in an invalid state (SLEEP, ABORT, etc) then this will have
		/// an unexpected behavior
		virtual void* result();

		/**
		 * @brief Implements the Chain of responsability, this worker will provide a worker for writing the results
		 *
		 * @return 
		 */
		virtual Worker* nextActionWorker();
	private:
		void initialize(const char* db, const char* ns, const char* select, const char* filter, const BSONObj* options);
		void initializeFullScan();

	private:
		Logger* _log;

		FindCommand* _command; //!< Command to be used as source of information
		InputStream* _input; //!< If required the input stream will contain additional information on the request
		OutputStream* _output;  //!< Here's where the result will be posted

		const char* _db;
		const char* _ns;
		const char* _select;
		const char* _filter;
		const BSONObj* _options;
		BSONArrayObj* _result;

		std::string _dataDir;

		bool _resultsReady; //!< If true it will means the results can be collected and the worker ended
		bool _running; //!< If true means that the worker is started and running

		// FindFullscan variables
		BSONBufferedObj* _bufferedObj;
		MMapInputStream* _stream; //!< This variable maps the file stream of a database file
		__int64 _maxResults; //!< Maximum results expected
		__int64 _count;  //!< Count of elements recollected
		FilterParser* _filterParser;
};

#endif /* FINDWORKER_INCLUDED_H */
