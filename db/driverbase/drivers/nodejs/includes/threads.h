/*
 * File:   threads.h
 * Author: cross
 *
 * Created on November 18, 2008, 9:42 PM
 */

#ifndef THREADS_H_INCLUDED
#define THREADS_H_INCLUDED

#include <pthread.h>

class Thread {
private:
    pthread_t internal;
    int m_threadId;

    static bool m_mutexInitalized;
    static pthread_mutex_t m_mutex_t;

public:
    void *(*runFunction)(void* arg);

    Thread(void *(*run)(void* arg));

    ~Thread();

    void start(void* arg);

    void join();

    static void mutex_lock();
    static void mutex_unlock();
	static void sleep(int milisecs);
};


#endif
