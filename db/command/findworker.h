// =====================================================================================
// 
//  @file:  findworker.h
// 
//  @brief:  Implements the worker class for finds, any find will be processed by this
//           worker, allowing pause and resume tasks taking too much time
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

class Logger;
class BSONObj;
class DBCursor;
class FindCommand;
class InputStream;
class OutputStream;

class FindWorker: public Worker {
	public:
		FindWorker(FindCommand* command, InputStream* input, OutputStream* output);
		FindWorker(const FindWorker& orig);
		virtual ~FindWorker();

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

	private:
		Logger* _log;
		FindCommand* _command;
		InputStream* _input;
		OutputStream* _output;

		DBCursor* _result;
};

#endif /* FINDWORKER_INCLUDED_H */
