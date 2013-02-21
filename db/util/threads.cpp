// *********************************************************************************************************************
// file:
// author: Juan Pablo Crossley (crossleyjuan@gmail.com)
// created:
// updated:
// license:
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
// *********************************************************************************************************************

#include "threads.h"
#include <iostream>
#include "defs.h"
#ifdef WINDOWS
#include <Windows.h>
#else
#include <sys/time.h>
#endif
#include <time.h>

int m_numtreads;
bool Thread::m_mutexInitalized;
pthread_mutex_t Thread::m_mutex_t;

Thread::Thread(void *(*run)(void* arg)) {
	runFunction = run;
	m_numtreads++;
	m_threadId = m_numtreads;
	_attached = false;
}

void Thread::start(void* arg) {
	_attached = true;
	int rc = pthread_create(&internal, NULL, runFunction, (void*)arg);
	if (rc) {
		throw "Error creating the thread";
	}
}

void Thread::join() {
	if (_attached) {
		pthread_join(internal, NULL);
	}
}

void Thread::detach() {
	if (_attached) {
		pthread_detach(internal);
		_attached = false;
	}
}

Thread::~Thread() {
	detach();
}

void Thread::stop() {
	detach();
}

void Thread::sleep(int milisecs) {
#ifndef WINDOWS
	struct timespec timeToWait;
	struct timeval now;
	int rt;

	int currentsecs;
	int currentusecs;

	gettimeofday(&now, NULL);

	timeToWait.tv_sec = now.tv_sec + (milisecs / 1000);
	timeToWait.tv_nsec = (now.tv_usec *1000);

	pthread_mutex_t fakeMutex = PTHREAD_MUTEX_INITIALIZER;
	pthread_cond_t fakeCond = PTHREAD_COND_INITIALIZER;	pthread_mutex_lock(&fakeMutex);

	rt = pthread_cond_timedwait(&fakeCond, &fakeMutex, &timeToWait);
	pthread_mutex_unlock(&fakeMutex);
#else
	System::Threading::Thread::CurrentThread->Sleep(milisecs);
#endif
}


void Thread::mutex_lock() {
	if (!m_mutexInitalized) {
		pthread_mutex_init(&m_mutex_t, NULL);
		m_mutexInitalized = true;
	}
	pthread_mutex_lock(&m_mutex_t);
}

void Thread::mutex_unlock() {
	pthread_mutex_unlock(&m_mutex_t);
}

