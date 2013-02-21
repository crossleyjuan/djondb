/*
 * =====================================================================================
 *
 *       Filename:  lock.h
 *
 *    Description:  Lock Header
 *
 *        Version:  1.0
 *        Created:  12/05/2012 10:29:42 PM
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
#ifndef LOCK_INCLUDED_H
#define LOCK_INCLUDED_H 
#include <pthread.h>

class Lock {
	public:
		Lock();

		// This will prevent any unwanted copy or assignment
		Lock(const Lock& orig);
		Lock& operator=(const Lock& rhs);
		virtual ~Lock();

		void lock();
		void unlock();

		void wait();
		void notify();
	private:
		pthread_mutex_t _mutex;
		pthread_cond_t  _cond;
};

#endif /* LOCK_INCLUDED_H */
