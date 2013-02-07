/*
 * =====================================================================================
 *
 *       Filename:  txbuffermanager.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2/6/2013 08:26:29 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Juan Pablo Crossley (Cross), cross@djondb.com
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
#include "txbuffermanager.h"
#include <stdlib.h>


TxBufferManager::TxBufferManager(InputOuputStream* stream) {
	this->_stream = stream;
	stream->seek(0, SEEK_END);
	_buffersSize = 64*1024*1024;
	_buffersCount = 0;
	this->_maxPos = stream->currentPos();
}

TxBufferManager::~TxBufferManager() {
	while (!_activeBuffers.empty()) {
		delete _activeBuffers.front();
		_activeBuffers.pop();
	}
	while (!_reusableBuffers.empty()) {
		delete _reusableBuffers.front();
		_reusableBuffers.pop();
	}
}

TXBuffer* TxBufferManager::getBuffer(__int32 minimumSize) {
	TXBuffer* result = NULL;
	if (!_activeBuffers.empty()) {
		result = _activeBuffers.back();
	}
	if ((_buffersSize - result->currentPos()) < minimumSize) {
		if (_reusableBuffers.empty()) {
			result = new TXBuffer(_stream, _buffersCount * _buffersSize);
			_activeBuffers.push_back(result);
		} else {
			result = _reusableBuffers.front();
			_reusableBuffers.pop();
		}
		_buffersCount++;
	}
	return result;
}
