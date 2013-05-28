/*
 * =====================================================================================
 *
 *       Filename:  buffermanager.h
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

#ifndef BUFFERMANAGER_INCLUDED_H
#define BUFFERMANAGER_INCLUDED_H

#include "buffer.h"
#include <queue>
#include <map>

class InputOuputStream;
class Thread;

class BufferManager
{
	public:
		BufferManager(const char* fileName);
		// This prevents copying the buffer manager
		BufferManager(const BufferManager& orig);
		~BufferManager();

		/**
		 * @brief Retrieves a buffer based on the position
		 *
		 * @param pos __int32 with the position required
		 *
		 * @return Returns the buffer of that position, if the position is greater than the max, it will return NULL
		 */
		Buffer* getBuffer(__int32 index);

		/**
		 * @brief Recovers the current active buffer to be used as write only buffer
		 * If the current buffer does not have enough space a new buffer will be retrieved
		 *
		 * @param minimumSize Minimum size of space required
		 *
		 * @return Buffer
		 */
		Buffer* getCurrentBuffer(__int32 minimumSize);

		/**
		 * @brief Recovers the current active buffer to be used as write only buffer
		 * If the current buffer does not have enough space a new buffer will be retrieved
		 *
		 * @param minimumSize Minimum size of space required
		 * @param forceNewBuffer if true then a new buffer will be returned, false will work as @sa getCurrentBuffer
		 *
		 * @return Buffer if forceNewBuffer is true then it will retrieve a new buffer
		 */
		Buffer* getCurrentBuffer(__int32 minimumSize, bool forceNewBuffer);

		const std::vector<Buffer*>* getActiveBuffers() const;
		__int32 buffersCount() const;

		void addBuffers(std::vector<Buffer*> buffers);
		std::vector<Buffer*> popAll();
		void dropAllBuffers();
		void dropControlFile();

	private:
		void initialize(const char* file);
		void loadBuffers();
		void addBuffer(Buffer* buffer);
		void addReusable(Buffer* buffer);
		void openLogFile();
		Buffer* createNewBuffer();
		Buffer* pop();
		void dropBuffer(Buffer* buffer);
		void registerBufferControlFile(Buffer* buffer, bool newBuffer, int flag);

#ifdef DEBUG
		void debugFlushBuffers();
#endif

	private:
		std::queue<Buffer*>*  _activeBuffers;
		std::vector<Buffer*>* _vactiveBuffers;
		std::queue<Buffer*>*  _reusableBuffers;

		Lock* _lockActiveBuffers;
		Lock* _lockWait;
		std::map<std::string, int*>* _buffersByLog;

		char* _logFileName;
		InputOutputStream* _controlFile;

		std::string _dataDir;

		__int64 _buffersSize;
		__int32 _buffersCount;
		bool _flushingBuffers;
		Logger* _log;
};

#endif // BUFFERMANAGER_INCLUDED_H
