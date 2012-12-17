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
#include "errors.h"

#include <string>
#include <vector>
#include <map>

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

