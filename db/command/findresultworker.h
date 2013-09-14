// =====================================================================================
// 
//  @file:  findresultworker.h
// 
//  @brief:  
// 
//  @version:  1.0
//  @date:     08/31/2013 08:07:22 AM
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
#ifndef FINDRESULTWORKER_INCLUDED_H
#define FINDRESULTWORKER_INCLUDED_H 

#include "worker.h"
#include "util.h"

class BSONArrayObj;
class InputStream;
class OutputStream;
class BSONOutputStream;

/**
 * @brief Provides a worker to write results to an OutputStream
 */
class FindResultWorker: public Worker {
	public:
		/// @brief Default constructor 
		///
		/// @param db
		/// @param ns
		/// @param select
		/// @param filter
		/// @param options
		FindResultWorker(BSONArrayObj* array, InputStream* input, OutputStream* output);
		FindResultWorker(const FindResultWorker& orig);
		virtual ~FindResultWorker();

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
		BSONArrayObj* _array;
		BSONOutputStream* _outputBSONStream;
		OutputStream* _output;
		InputStream* _input;
		__int32 _currentPosition;
};

#endif /* FINDRESULTWORKER_INCLUDED_H */
