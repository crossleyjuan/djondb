/*
 * File:   util.h
 * Author: cross
 *
 * Created on July 7, 2010, 1:33 PM
 */

#ifndef _UTIL_H
#define	_UTIL_H

#include "datetime.h"
#include "dtime.h"
#include "fileutil.h"
#include "stringfunctions.h"
#include "errorhandle.h"
#include "version.h"
#include "logger.h"
#include "threads.h"
#include "settings.h"
#include "circular_queue.h"
#include "djon_error_codes.h"

#include <string>
#include <vector>
#include <map>

class DjondbException: public std::exception {
	public:
		DjondbException(int code, const char* error);
		DjondbException(const DjondbException& orig);
		virtual const char* what() const throw();
		int errorCode() const;

	private:
		int _errorCode;
		const char* _errorMessage;
};
/*****************************************************************
  Type Definitions and macros
  */

bool isDaemon();
void setDaemon(bool daemon);

void logInfo(char* text);

bool endsWith(const char* text, const char* end);

std::string* uuid();

bool makedir(const char* path);
std::string* getHomeDir();
std::string getTempDir();

Version getCurrentVersion();
Version getVersion(const char* version);

/***********************************************************************
 * Memory functions
 ***********************************************************************/
void* mmalloc(size_t size);

/***********************************************************************/

#endif	/* _UTIL_H */

