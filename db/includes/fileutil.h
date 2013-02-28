/* 
 * File:   fileutil.h
 * Author: cross
 *
 * Created on July 7, 2010, 3:08 PM
 */

#ifndef _FILEUTIL_H
#define	_FILEUTIL_H

#include <vector>
#include <string>
#include "defs.h"

char* readFile(char* fileName);
int writeFile(const std::string& fileName, const std::string& text, bool append);
int getdir (const char* dir, std::vector<char*> &files, const char* extension);
bool existFile(const char* fileName);
bool existDir(const char* dir);
bool makeDir(const char* dir);
bool checkFileCreation(const char* dir);
bool removeFile(const char* file);
__int64 fileSize(const char* file);

#ifndef WINDOWS
#define FILESEPARATOR "/"
#else
#define FILESEPARATOR (char*)"\\"
#endif

#endif	/* _FILEUTIL_H */

