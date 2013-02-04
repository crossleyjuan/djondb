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

#include "fileutil.h"
#include "stringfunctions.h"
#include "errorhandle.h"
#include "util.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#ifndef WINDOWS
#include <dirent.h>
#include <errno.h>
#else
using namespace System;
using namespace System::IO;
using namespace System::Runtime::InteropServices;
#include <direct.h>
#endif
#include <string.h>
#include <stdlib.h>

using namespace std;

char* readFile(char* fileName) {
    ifstream ifs;

    ifs.open(fileName, ifstream::in);

    stringbuf* buffer = new stringbuf();

    while (ifs.good()) {
        ifs >> buffer;
    }
    ifs.close();

    string str = buffer->str();

    delete(buffer);

	if (str.length() > 0) {
		char* result = strcpy(str);

		return result;
	} else {
		return NULL;
	}
}

int writeFile(const std::string& fileName, const std::string& text, bool append) {
    ofstream ofs;

    ios_base::openmode flags;
    if (append) {
        flags = ios_base::app | ios_base::out;
    } else {
        flags = ios_base::out;
    }
    ofs.open(fileName.c_str(),  flags);
    if (ofs.fail() != 0) {
        setLastError(1, (std::string("Failed to open the file: ") + fileName).c_str());
        return 1;
    }

    ofs << text;
    ofs.close();
    return 0;
}

int getdir (const char* dir, vector<char*> &files, const char* extension)
{
	if (!existDir(dir)) {
        setLastError(1, "Error opening the dir: %s", dir);
        return 1;
	}
    int len = strlen(extension) + 2;
    char* fileextension = (char*)malloc(len);
    memset(fileextension, 0, len);
    fileextension[0] = '.';
    strcpy(fileextension+1, extension);

#ifndef WINDOWS
	 DIR *dp;
	 struct dirent *dirp;
    while ((dirp = readdir(dp)) != NULL) {
        char* currentFile (dirp->d_name);
        if (endsWith(currentFile, fileextension)) {
            files.push_back(strcpy(currentFile));
        }
    }
    closedir(dp);
#else
	String^ folder = Marshal::PtrToStringAnsi((IntPtr) (char *) dir);;
   array<String^>^ arrFiles = Directory::GetFiles( folder );
   for (int i=0; i<arrFiles->Length; i++) {
      String^ file = arrFiles[i];
	  char* currentFile = (char*)(void*)Marshal::StringToHGlobalAnsi(file);
      if (endsWith(currentFile, fileextension)) {
		files.push_back(strcpy(currentFile));
	  }
   }
#endif // #ifndef WINDOWS
    free(fileextension);
    return 0;
}

bool existFile(const char* fileName) {
    ifstream ifs;

    ifs.open(fileName, ifstream::in);

    bool exists = true;
    if (ifs.fail()) {
        exists = false;
    }
    ifs.close();
    return exists;
}

bool existDir(const char* dir) {
	Logger* log = getLogger(NULL);
	if (log->isDebug()) log->debug(3, "existDir(%s)", dir);
	bool exists = true;
#ifndef WINDOWS
    DIR *dp;
    if((dp  = opendir(dir)) == NULL) {
        exists = false;
    }
	 if (dp != NULL) {
	 	closedir(dp);
	 }
#else 
	String^ folder = Marshal::PtrToStringAnsi((IntPtr) (char *) dir);;
	exists = Directory::Exists(folder);
	if (log->isDebug()) log->debug(3, "Directory::Exists(%s) returns: %d", dir, exists);
#endif //#ifndef WINDOWS
    return exists;
}

bool checkFileCreation(const char* dir) {
	Logger* log = getLogger(NULL);
	if (log->isDebug()) log->debug(3, "checkFileCreation(%s)", dir);
	char* file = (char*)malloc(strlen(dir) + 10);
	memset(file, 0, strlen(dir) + 10);
	strcat(file, dir);
	if (!endsWith(dir, FILESEPARATOR)) {
		strcat(file, FILESEPARATOR);
	}
	strcat(file, "file.chk");

	FILE* f = fopen(file, "w");
	bool result = true;
	if (f == NULL) {
		char* error = strerror(errno);
		setLastError(errno, error);
		if (log->isDebug()) log->debug(3, "the file cannot be opened at: %s ,due to: %s ", dir, error);
		result = false;
	}
	if (f != NULL) {
		fclose(f);
	}

	removeFile(file);
	free (file);
	return result;
}

bool removeFile(const char* file) {
	if (remove(file) != 0) {
		const char* error = strerror(errno);
		setLastError(errno, error);
		return false;
	} else {
		return true;
	}
}

bool makeDir(const char* dir) {
	Logger* logger = getLogger(NULL);
	if (logger->isDebug()) logger->debug(3, "makeDir(%s)", dir);
	std::vector<std::string> dirs = split(dir, FILESEPARATOR);
	std::stringstream ss;

#ifndef WINDOWS
	ss << FILESEPARATOR;
#endif
	for (std::vector<std::string>::const_iterator it = dirs.begin(); it != dirs.end(); it++) {
		std::string cdir = *it;
		ss << cdir << FILESEPARATOR;

		std::string currentdir = ss.str();

		if (!existDir(currentdir.c_str())) {
#ifndef WINDOWS
			int res = mkdir(currentdir.c_str(), 0777);
#else
			int res = _mkdir(currentdir.c_str());
#endif
			if (res < 0) {
				char* error = strerror(errno);
				logger->error("An error ocurred creating the directory %s. Error: %s", dir, error);
				exit(1);
			}
		}
	}
	return true;
}

__int64 fileSize(const char* file) {
	struct stat st;
	Logger* log = getLogger(NULL);

	__int64 ret = -1;
	if (stat(file, &st) == 0)
		ret = st.st_size;
	else 
		log->error("Error getting the file: %s size: %d", file, errno); 

	return ret;
}
