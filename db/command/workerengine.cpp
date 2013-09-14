// =====================================================================================
// 
//  @file:  workerengine.cpp
// 
//  @brief:  This is the implementation of the worker engine, which will priorize tasks and execute it
//  using a priorized queue
// 
//  @version:  1.0
//  @date:     07/22/2013 05:52:29 AM
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
#include "workerengine.h"

#include "command.h"
#include "workerfactory.h"

WorkerEngine* WorkerEngine::_instance = NULL;

WorkerEngine* WorkerEngine::workerEngine() {
	if (_instance == NULL) {
		_instance = new WorkerEngine();
	}
	return _instance;
}

WorkerEngine::WorkerEngine() {
}

WorkerEngine::~WorkerEngine() {
}

// It's not copyable
// WorkerEngine(const WorkerEngine& orig);


/**
 * @brief This will return the priority based on the task type,
 * the options of the command and the priority base configured in settings
 *
 * @param command
 *
 * @return a number greater than 1
 */
int priority(Command* command) {
	int prioritybase;
	switch (command->commandType()) {
		case INSERT:
		case UPDATE:
		case REMOVE:
			prioritybase = 1000;
			break;
		case FIND:
			prioritybase = 2000;
			break;
		default:
			prioritybase = 500;
			break;
	}

	return prioritybase;
}

void WorkerEngine::enqueueWorker(Worker* worker, int priority) {
	PriorityElement* element = new PriorityElement();
	element->worker = worker;
	element->priority = priority;
	element->queueState = 0;

	// executes one step in the newly created worker, this ensures that simple steps are
	// executed immediately and they wont need for a next cycle to run
	runWorkerStep(element);

	// if the worker is not in sleep (which means it still alive) then it'll not be enqueued
	if (element->worker->state() == Worker::WS_SLEEP) {
		element->queueState = 1;
		_queue.push_back(element);
	}
}

void WorkerEngine::enqueueTask(Command* command, InputStream* input, OutputStream* output) {
	Worker* worker = getWorker(command, input, output);
	int p = priority(command);

	enqueueWorker(worker, p);
}

WorkerEngine::PriorityElement* WorkerEngine::peekWorkerFromQueue() {
	PriorityElement* selected = NULL;
	// if an element is not selected it's priority increase
	// if an element is selected then its priority decrease for the next round
	for (std::list<PriorityElement*>::iterator i = _queue.begin(); i != _queue.end(); i++) {
		PriorityElement* element = *i;
		// Only elements that are on sleep mode will be selected
		// it's pointless to take elements already finished
		if (element->worker->state() == Worker::WS_SLEEP) {
			if (selected == NULL) {
				selected = element;
				selected->priority++;
			} else {
				if (element->priority < selected->priority) {
					// because the current selected task will not be
					// executed it'll increase its priority
					selected->priority--;
					selected = element;
					selected->priority++;
				} else {
					element->priority--;
				}
			}
		}
	}
	return selected;
}

void WorkerEngine::removeWorker(PriorityElement* element) {
	if (element->queueState == 1) {
		for (std::list<PriorityElement*>::iterator i = _queue.begin();
				i != _queue.end(); i++) {
			if (element == *i) {
				_queue.erase(i);
				break;
			}
		}
	}
}

void WorkerEngine::runWorkerStep(PriorityElement* element) {
	element->worker->resume();
	if (element->worker->state() != Worker::WS_SLEEP) {
		Worker* newworker = element->worker->nextActionWorker();
		if (newworker != NULL) {
			// uses the same priority of the original worker
			enqueueWorker(newworker, element->priority);
		}
		removeWorker(element);
	}
}

void WorkerEngine::executeSteps() {
	PriorityElement* element = peekWorkerFromQueue();
	if (element != NULL) {
		runWorkerStep(element);
	}
}

void WorkerEngine::refreshWorker(PriorityElement* element) {
	Worker* worker = element->worker;

	bool removeWorker = false;
	switch (worker->state()) {
		case Worker::WS_SLEEP:
		case Worker::WS_RUNNING:
			// nothing will happend
			break;
		case Worker::WS_END_READY:
			removeWorker = true;
			break;
		case Worker::WS_ABORTED:
			removeWorker = true;
			break;
	}
}

void WorkerEngine::shutdownEngine() {
}

int WorkerEngine::length() const {
	return _queue.size();
}
