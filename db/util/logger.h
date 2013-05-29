#ifndef LOGGER_H_INCLUDED
#define LOGGER_H_INCLUDED

#include <string>
#include "defs.h"
#include "util.h"
#include "dtime.h"
#include <ctime>
#ifndef WINDOWS
#include <time.h>
#else

// The pthread.h already defines the timespec struct, this wards prevent double reference
#if !defined(HAVE_STRUCT_TIMESPEC)
#define HAVE_STRUCT_TIMESPEC
#if !defined(_TIMESPEC_DEFINED)
#define _TIMESPEC_DEFINED
struct timespec {
        time_t tv_sec;
        long tv_nsec;
};
#endif /* _TIMESPEC_DEFINED */
#endif /* HAVE_STRUCT_TIMESPEC */
#endif

#ifdef WINDOWS
	#include <Windows.h>
	#include <winsock.h>
	//struct timeval {
	//		long    tv_sec;         /* seconds */
	//		long    tv_usec;        /* and microseconds */
	//};
#endif

using namespace std;

class Logger {
	private:
		class Config {
			public:
			bool m_debug;
			bool m_info;
			bool m_warn;
			int _detail;

		};
		void* m_clazz;
		static Config* _configSettings;

		int _interval;
		struct timespec _ts1;
		struct timespec _ts2;

	private:
		void print(std::string type, std::string message);

	public:
		Logger(void* clazz);
		virtual ~Logger();
		bool isDebug();
		bool isInfo();
		bool isWarn();
		void debug(string message, ...);
		void debug(int detail, string message, ...);
		void error(string error, ...);
		void error(exception ex);
		void info(string message, ...);
		void warn(string warn, ...);

		void startTimeRecord();
		void stopTimeRecord();

		DTime recordedTime();
};

Logger* getLogger(void* clazz);

#endif
