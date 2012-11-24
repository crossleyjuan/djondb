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

#include "logger.h"
#include "stringfunctions.h"

#include <iostream>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sstream>
#include "settings.h"
#include "defs.h"
#ifndef WINDOWS
#include <syslog.h>
#endif
#ifdef MAC
#include <mach/clock.h>
#include <mach/mach.h>
#endif

#include <boost/algorithm/string.hpp>

Logger::Config* Logger::_configSettings;

#define PRINT(TYPE, CLAZZ) \
	const char* cmessage = message.c_str(); \
	int bufferSize = 10000; \
	char* buffer = (char*)malloc(bufferSize + 1); \
   buffer[bufferSize] = '\0'; \
va_list args; \
va_start (args, message); \
vsprintf (buffer, cmessage, args); \
va_end(args); \
std::string result; \
std::stringstream ss; \
ss << buffer; \
result = ss.str(); \
free(buffer); \
print(TYPE, result);

#ifdef LINUX
timespec diff(timespec start, timespec end)
{
	timespec temp;
	if ((end.tv_nsec-start.tv_nsec)<0) {
		temp.tv_sec = end.tv_sec-start.tv_sec-1;
		temp.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
	} else {
		temp.tv_sec = end.tv_sec-start.tv_sec;
		temp.tv_nsec = end.tv_nsec-start.tv_nsec;
	}
	return temp; 
} 
#endif

#ifndef LINUX // This works in mac and linux
timeval diff(timeval start, timeval end)
{
	timeval temp;
	if ((end.tv_usec-start.tv_usec)<0) {
		temp.tv_sec = end.tv_sec-start.tv_sec-1;
		temp.tv_usec = 1000000000+end.tv_usec-start.tv_usec;
	} else {
		temp.tv_sec = end.tv_sec-start.tv_sec;
		temp.tv_usec = end.tv_usec-start.tv_usec;
	}
	return temp;
}
#endif




#ifdef MAC // OS X does not have clock_gettime, use clock_get_time
int clock_gettime(int X, struct timeval *tv) 
{
	clock_serv_t cclock;
	mach_timespec_t mts;
	host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
	clock_get_time(cclock, &mts);
	mach_port_deallocate(mach_task_self(), cclock);
	tv->tv_sec = mts.tv_sec;
	tv->tv_usec = mts.tv_nsec * 0.000001; 

}
#endif

#ifdef WINDOWS
LARGE_INTEGER getFILETIMEoffset()
{
	SYSTEMTIME s;
	FILETIME f;
	LARGE_INTEGER t;

	s.wYear = 1970;
	s.wMonth = 1;
	s.wDay = 1;
	s.wHour = 0;
	s.wMinute = 0;
	s.wSecond = 0;
	s.wMilliseconds = 0;
	SystemTimeToFileTime(&s, &f);
	t.QuadPart = f.dwHighDateTime;
	t.QuadPart <<= 32;
	t.QuadPart |= f.dwLowDateTime;
	return (t);
}

int clock_gettime(int X, struct timeval *tv)
{
	LARGE_INTEGER           t;
	FILETIME            f;
	double                  microseconds;
	static LARGE_INTEGER    offset;
	static double           frequencyToMicroseconds;
	static int              initialized = 0;
	static BOOL             usePerformanceCounter = 0;

	if (!initialized) {
		LARGE_INTEGER performanceFrequency;
		initialized = 1;
		usePerformanceCounter = QueryPerformanceFrequency(&performanceFrequency);
		if (usePerformanceCounter) {
			QueryPerformanceCounter(&offset);
			frequencyToMicroseconds = (double)performanceFrequency.QuadPart / 1000000.;
		} else {
			offset = getFILETIMEoffset();
			frequencyToMicroseconds = 10.;
		}
	}
	if (usePerformanceCounter) QueryPerformanceCounter(&t);
	else {
		GetSystemTimeAsFileTime(&f);
		t.QuadPart = f.dwHighDateTime;
		t.QuadPart <<= 32;
		t.QuadPart |= f.dwLowDateTime;
	}

	t.QuadPart -= offset.QuadPart;
	microseconds = (double)t.QuadPart / frequencyToMicroseconds;
	t.QuadPart = microseconds;
	tv->tv_sec = t.QuadPart / 1000000;
	tv->tv_usec = t.QuadPart % 1000000;
	return (0);
}
#endif

Logger* getLogger(void* clazz) {
	Logger* logger = new Logger(clazz);
	return logger;
}

Logger::Logger(void* clazz) {
	m_clazz = clazz;
	if (_configSettings == NULL) {
		// this will be readed from a configuration file
		// but right now it'll be hardcoded by config.h
		bool debug = false;
		std::string debug_conf = getSetting("debug");
		if (boost::iequals(debug_conf, std::string("true"))) {
			debug = true;
		} else {
			debug = false;
		}

		std::string logLevel = getSetting("loglevel");
		int detail = 0;
		if (debug && (logLevel.length() > 0)) {
			detail = atoi(logLevel.c_str());
			if (detail > 3) {
				warn("loglevel is greater than 3, adjusted to be equals to 3");
				detail = 3;
			}
			if (detail < 0) {
				warn("loglevel is less than 0, adjusted to be equals to 0");
				detail = 0;
			}
		}
		_configSettings = new Logger::Config();
		_configSettings->m_debug = debug;
		_configSettings->m_warn = true;
		_configSettings->m_info = true;
		_configSettings->_detail = detail;
	}
}

void Logger::print(std::string type, std::string text) {
#ifndef WINDOWS
	if (isDaemon()) {
		syslog(LOG_DEBUG, text.c_str());
	} else {
		cout << type << ": " << text << endl;
	}
#else
	cout << type << ": " << text << endl;
#endif
}

/*
	std::string format(const char * message, ...) {
	char* buffer = (char*)malloc(1000);
	memset(buffer, 0, 1000);
	va_list args;
	va_start (args, message);
	vsprintf (buffer,message, args);
	va_end(args);
	std::string result;
	std::stringstream ss;
	ss << buffer;
	free(buffer);
	}
	*/

void Logger::debug(string message, ...) {
	// default debug behaviour is maximum detail
	if (_configSettings->_detail >= 3) {
		PRINT("DEBUG", m_clazz);
	}
}

void Logger::debug(int detail, string message, ...) {
	if (detail <= _configSettings->_detail) {
		PRINT("DEBUG", m_clazz);
	}
}

void Logger::info(string message, ...) {
	PRINT("INFO", m_clazz);
}


void Logger::warn(string message, ...) {
	PRINT("WARN", m_clazz);
}


void Logger::error(string message, ...) {
	PRINT("ERROR", m_clazz);
}

void Logger::error(exception ex) {
	cout << m_clazz << ":" << ex.what() << endl;
}

bool Logger::isDebug() {
	return _configSettings->m_debug;
}

bool Logger::isInfo() {
	return _configSettings->m_info;
}

bool Logger::isWarn() {
	return _configSettings->m_warn;
}

void Logger::startTimeRecord() {
	int interval = 0;
#ifdef LINUX
	interval = CLOCK_REALTIME;// CLOCK_PROCESS_CPUTIME_ID;
#endif

	clock_gettime(interval, &_ts1);
}

void Logger::stopTimeRecord() {
#ifdef LINUX
	int interval = CLOCK_REALTIME;// CLOCK_PROCESS_CPUTIME_ID;
#else
	int interval = 0;// CLOCK_PROCESS_CPUTIME_ID;
#endif
	clock_gettime(interval, &_ts2);
}

DTime Logger::recordedTime() {
#ifdef LINUX
	timespec etime = diff(_ts1, _ts2);
	double secs = etime.tv_sec + ((double)etime.tv_nsec / 1000000000.0);
#else
	struct timeval etime = diff(_ts1, _ts2);
	double secs = etime.tv_sec + ((double)etime.tv_usec / 1000000.0);
#endif

	DTime result(secs);

	return result;
}
