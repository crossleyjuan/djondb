/*
 * =====================================================================================
 *
 *       Filename:  lock.cpp
 *
 *    Description:  Thread Lock implementation
 *
 *        Version:  1.0
 *        Created:  12/05/2012 10:32:10 PM
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
#include "lock.h"
#include "logger.h"
#include <errno.h>
#include <string.h>
#ifdef WINDOWS
#include <windows.h>
#endif

Lock::Lock() {
	pthread_mutexattr_t mutexattr;
	pthread_mutexattr_init(&mutexattr);
	pthread_mutexattr_settype(&mutexattr, PTHREAD_MUTEX_RECURSIVE);

	pthread_mutex_init(&_mutexLock, &mutexattr);

	_condInitialized = false;
}

Lock::~Lock() {
	pthread_mutex_destroy(&_mutexLock);
}

void Lock::lock() {
	pthread_mutex_lock(&_mutexLock);
}

void Lock::unlock() {
	pthread_mutex_unlock(&_mutexLock);
}

void Lock::initializeCondition() {
	if (!_condInitialized) {
		pthread_cond_init(&_cond, NULL);
		_condInitialized = true;
	}
}

void Lock::destroyCondition() {
	if (_condInitialized) {
		pthread_cond_destroy(&_cond);
		_condInitialized = false;
	}
}

void Lock::wait() {
	initializeCondition();
	pthread_cond_wait(&_cond, &_mutexLock);
	destroyCondition();
}

void Lock::wait(__int32 timeout) {
	struct timespec timeToWait;
	int rt;

	initializeCondition();
	bool wait = true;
	while (wait) {
#ifdef WINDOWS
		time_t now;
		time(&now);
		timeToWait.tv_sec = now;
		timeToWait.tv_nsec = now * 1000000000;
#else
		clock_gettime(CLOCK_REALTIME, &timeToWait);
#endif
		timeToWait.tv_sec += timeout;
		rt = pthread_cond_timedwait(&_cond, &_mutexLock, &timeToWait);
		// windows overrides the default ETIMEDOUT from pthread
		if ((rt == ETIMEDOUT) || (rt == 10060)) {
			wait = false;
		} else if (rt > 0) {
			getLogger(NULL)->error("pthread_cond_timedwait rt: %d, error: %d, description: %s", rt, errno, strerror(errno));
			wait = false;
		}
	}
	destroyCondition();
}

void Lock::notify() {
	if (_condInitialized) {
		pthread_cond_signal(&_cond);
	}
}
