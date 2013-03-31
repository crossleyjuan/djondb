/*
 * =====================================================================================
 *
 *       Filename:  txbuffermanager.h
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

#ifndef TXBUFFERMANAGER_INCLUDED_H
#define TXBUFFERMANAGER_INCLUDED_H

#include "txbuffer.h"
#include <queue>
#include <map>

class InputOuputStream;
class TransactionOperation;
class Controller;
class Thread;

class TxBufferManager
{
	public:
		TxBufferManager(Controller* controller, const char* fileName, bool mainLog);
		// This prevents copying the buffer manager
		TxBufferManager(const TxBufferManager& orig);
		~TxBufferManager();

		TxBuffer* getBuffer(__int32 minimumSize);
		TxBuffer* getBuffer(__int32 minimumSize, bool force);
		const std::vector<TxBuffer*>* getActiveBuffers() const;
		__int32 buffersCount() const;

		void writeOperationToRegister(const char* db, const char* ns, const TransactionOperation& operation);
		TransactionOperation* readOperationFromRegister(TxBuffer* buffer);
		TransactionOperation* readOperationFromRegister(TxBuffer* buffer, char* db, char* ns);
		void addBuffers(std::vector<TxBuffer*> buffers);
		void startMonitor();
		void stopMonitor();
		bool runningMonitor() const;
		std::vector<TxBuffer*> popAll();
		// Joins the current thread to avoid unwanted abortion
		void join();
		void dropAllBuffers();
		void dropControlFile();

	private:
		void initialize(const char* file);
		void loadBuffers();
		void addBuffer(TxBuffer* buffer);
		void addReusable(TxBuffer* buffer);
		void openLogFile();
		static void *monitorBuffers(void* arg);
		virtual void flushBuffer();
		TxBuffer* createNewBuffer();
		TxBuffer* pop();
		void dropBuffer(TxBuffer* buffer);
		void registerBufferControlFile(TxBuffer* buffer, bool newBuffer, int flag);

#ifdef DEBUG
		void debugFlushBuffers();
#endif

		bool mainLog() const {
			return _mainLog;
		};

	private:
		std::queue<TxBuffer*>*  _activeBuffers;
		std::vector<TxBuffer*>* _vactiveBuffers;
		std::queue<TxBuffer*>*  _reusableBuffers;
		Lock* _lockActiveBuffers;
		Thread* _monitorThread;
		bool _runningMonitor;
		std::map<std::string, int*>* _buffersByLog;

		char* _logFileName;
		InputOutputStream* _controlFile;
		Controller* _controller;

		std::string _dataDir;

		__int64 _buffersSize;
		__int32 _buffersCount;
		bool _flushingBuffers;
		bool _mainLog;
		Logger* _log;
};

#endif // TXBUFFERMANAGER_INCLUDED_H
