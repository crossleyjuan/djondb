/// =====================================================================================
/// 
///  @file:  worker.h
/// 
///  @version:  1.0
///  @date:     07/10/2013 01:12:58 PM
///  Compiler:  g++
/// 
///  @author:  Juan Pablo Crossley (Cross), crossleyjuan@gmail.com
/// 
/// This file is part of the djondb project, for license information please refer to the LICENSE file,
/// the application and libraries are provided as-is and free of use under the terms explained in the file LICENSE
/// Its authors create this application in order to make the world a better place to live, but you should use it on
/// your own risks.
/// 
/// Also, be adviced that, the GPL license force the committers to ensure this application will be free of use, thus
/// if you do any modification you will be required to provide it for free unless you use it for personal use (you may 
/// charge yourself if you want), bare in mind that you will be required to provide a copy of the license terms that ensures
/// this program will be open sourced and all its derivated work will be too.
/// =====================================================================================
#ifndef WORKER_INCLUDED_H
#define WORKER_INCLUDED_H 

/*!  \brief  This contains the common interface for worker processes, this allows to stop and resume
 *  tasks which takes too much time
 *
 *  The workflow of a task is as follows: \n
 *  WS_SLEEP -> WS_RUNNING \n
 *  WS_SLEEP -> WS_ABORTED \n
 *  WS_RUNNING -> WS_ABORTED \n
 *  WS_RUNNING -> WS_SLEEP \n
 *  WS_RUNNING -> WS_END_READY \n
 **/
class Worker {
	public:
		enum WORKER_STATE {
			WS_NOTINITIATED, ///!< The process is paused and waiting for resume
			WS_SLEEP, ///!< The process is paused and waiting for resume
			WS_RUNNING, ///!< The process is active and running
			WS_ABORTED, ///!< The process was aborted, the results can not be collected
			WS_END_READY ///!< The worker ended and the results are ready
		};

		Worker() {
			_state = WS_NOTINITIATED;
		};

		/// @brief This method executes steps in the current taks
		/// the client should check the state to see if the task state
		virtual void resume() = 0;

		/// @brief This should return the results of the worker
		/// If the worker is in an invalid state (SLEEP, ABORT, etc) then this will have
		/// an unexpected behavior
		virtual void* result() = 0;

		/**
		 * @brief Implements the Chain of responsability, each worker should provide a next worker
		 * if required or NULL if not, for example: The find should provide a worker to write the
		 * results using the worker model
		 *
		 * @return 
		 */
		virtual Worker* nextActionWorker() = 0;

		/// @brief returns the state of the task
		///
		/// @return returns a WORKER_STATE of the current state
		virtual WORKER_STATE state() {
			return _state;
		}

		/// @brief Sets the state of the current worker, it will validate the new state based on the current
		///         one to avoid wrong behavior
		///
		/// @param state
		virtual void setState(WORKER_STATE state) {
			/// checks the valid workflow
			switch (_state) {
				case WS_SLEEP: 
				case WS_RUNNING: {
										switch (state) {
											case WS_SLEEP:
											case WS_RUNNING:
											case WS_ABORTED:
											case WS_END_READY:
												break;
											default:
												throw "Invalid state. It's not allowed to go from WS_SLEEP or WS_RUNNING to this state";
										}
										break;
									}
				case WS_END_READY:
				case WS_ABORTED: {
										  switch (state) {
											  case WS_ABORTED:
											  case WS_END_READY:
												  break;
											  default:
												  throw "Invalid state. It's not allowed to go from WS_END_READY or WS_ABORTED to this state";
										  }
										  break;
									  }
			}
			_state = state;
		}

	private:
		WORKER_STATE _state;
};

#endif /* WORKER_INCLUDED_H */
