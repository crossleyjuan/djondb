// =====================================================================================
// 
//  @file:  workerengine.h
// 
//  @brief:  
// 
//  @version:  1.0
//  @date:     07/21/2013 05:06:59 AM
//  Compiler:  g++
// 
//  @author:  Juan Pablo Crossley (Cross), cross@djondb.com
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
#ifndef WORKERENGINE_INCLUDED_H
#define WORKERENGINE_INCLUDED_H 

#include <list>
#include "util.h"
#include "worker.h"

class Command;
class InputStream;
class OutputStream;

/**
 * @brief This is the implementation of the worker engine, which will priorize tasks and execute them according to their
 * priority
 */
class WorkerEngine {
	public:
		WorkerEngine(const WorkerEngine& orig);
		virtual ~WorkerEngine();

		/**
		 * @brief Enqueues a new task on the worker process
		 *
		 * @param command Command to be enqueued
		 * @param input InputStream that will be used to retrieve additional information, this could be NULL
		 * @param output OutputStream used to send the result to it, it could be NULL if the worker does not need to send results
		 */
		void enqueueTask(Command* command, InputStream* input, OutputStream* output);

		/**
		 * @brief Adds a worker to the queue with the required priority
		 *
		 * @param worker
		 * @param priority
		 */
		void enqueueWorker(Worker* worker, int priority);

		/**
		 * @brief Execute the next step in the queue
		 */
		void executeSteps();

		/**
		 * @brief shutdown procedure, this must be called before deleting the worker instance
		 */
		void shutdownEngine();

		static WorkerEngine* workerEngine(); //!< Singleton implementation only

		int length() const;

	private:
		struct PriorityElement {
			Worker* worker;
			__int32 priority; //<! The higher priority is 0 and it will go up to any number, 1 has more priority than 100000
			int queueState; //<! simple value to indicate if it's enqueued or not (0-1 at this moment, later this could be used to improve the algorithm) 0 - Not enqueued, 1 - Enqueued
		};

		std::list<PriorityElement*> _queue;
		static WorkerEngine* _instance;

	private:
		WorkerEngine();
		PriorityElement* peekWorkerFromQueue(); //!< returns a worker from the queue, this will takes the worker with higher priority (priority near to 0)
		void refreshWorker(PriorityElement* element); //!< Checks the state of the current worker and refresh the queue based on it
		void runWorkerStep(PriorityElement* element); //!< Executes an step on the specified priority element

		/**
		 * @brief Remove a worker from the queue
		 *
		 * @param queue
		 * @param element
		 */
		void removeWorker(PriorityElement* element);

};
#endif /* WORKERENGINE_INCLUDED_H */
