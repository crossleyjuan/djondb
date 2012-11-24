/*
 * =====================================================================================
 *
 *       Filename:  StreamManager.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/11/2012 11:10:35 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include "streammanager.h"
#include "fileoutputstream.h"
#include <stdlib.h>
#include <memory>
#include <cstring>

StreamManager* StreamManager::_manager;

StreamManager* StreamManager::getStreamManager() {
	if (_manager == NULL) {
		_manager = new StreamManager();
	}
	return _manager;
}

StreamManager::StreamManager() {
	_logger = getLogger(NULL);
}

StreamManager::~StreamManager() {
	delete _logger;
}

void StreamManager::setDataDir(const std::string& dataDir) {
	_dataDir = dataDir;
}

std::string StreamManager::fileName(std::string ns, FILE_TYPE type) const {
	char* cfile = (char*)malloc(ns.length() + 5);
	memset(cfile, 0, ns.length() + 5);
	char* fileType;
	switch (type) {
		case DATA_FTYPE:
			fileType =  "dat";
			break;
		case INDEX_FTYPE:
			fileType = "idx";
			break;
		case STRC_FTYPE:
			fileType = "stt";
			break;
	}
	sprintf(cfile, "%s.%s", ns.c_str(), fileType);
	std::string result(cfile);
	free(cfile);
	return result;
}

StreamType* StreamManager::open(std::string db, std::string ns, FILE_TYPE type) {
	std::auto_ptr<Logger> log(getLogger(NULL));
	StreamType* stream = NULL;

	map<std::string, SpacesType>* spaces = NULL;
	map<std::string, map<std::string, SpacesType>* >::iterator itDB = _spaces.find(db);
	if (itDB == _spaces.end()) {
		spaces = new map<std::string, SpacesType>();
		_spaces.insert(pair<std::string, map<std::string, SpacesType>* >(db, spaces));
	} else {
		spaces = itDB->second;
	}

	map<std::string, SpacesType>::iterator it = spaces->find(ns);

	map<FILE_TYPE, StreamType*>* streams = NULL;
	if (it != spaces->end()) {
		streams = (it->second).streams;
		map<FILE_TYPE, StreamType*>::iterator istreams = streams->find(type);
		if (istreams != streams->end()) {
			stream = istreams->second;
		}
	} 

	if (stream != NULL) {
		return stream;
	}

	std::string filedir = concatStrings(_dataDir, db);
	filedir += FILESEPARATOR;
	if (log->isDebug()) log->debug(3, "StreamManager::open checking directory %s", filedir.c_str());
	if (!existDir(filedir.c_str())) {
		if (log->isDebug()) log->debug(3, "StreamManager::open creating directory %s", filedir.c_str());
		makeDir(filedir.c_str());
	}
	std::string file = fileName(ns, type);
	std::string streamfile = concatStrings(filedir, file);
	char* flags;
	if (existFile(streamfile.c_str())) {
		flags = "rb+";
	} else {
		flags = "wb+";
	}
	StreamType* output;
  	output = new StreamType(streamfile, flags);
	if (streams) {
		streams->insert(pair<FILE_TYPE, StreamType*>(type, output));
	} else {
		SpacesType space;
		space.ns = ns;
		space.streams = new map<FILE_TYPE, StreamType*>();
		space.streams->insert(pair<FILE_TYPE, StreamType*>(type, output));
		spaces->insert(pair<std::string, SpacesType>(ns, space));
	}

	return output;
}

bool StreamManager::close(char* db, char* ns) {
	map<std::string, map<std::string, SpacesType>* >::iterator itDb = _spaces.find(std::string(db));
	if (itDb != _spaces.end()) {
		map<std::string, SpacesType>* spaces = itDb->second;
		map<std::string, SpacesType>::iterator it = spaces->find(std::string(ns));
		if (it != spaces->end()) {
			SpacesType space = it->second;

			map<FILE_TYPE, StreamType*>* streams = space.streams;
			for (map<FILE_TYPE, StreamType*>::iterator istreams = streams->begin(); istreams != streams->end(); istreams++) {
				StreamType* stream = istreams->second;
				stream->close();
				delete stream;
				stream = 0;
			}
			spaces->erase(it);
		}
	}
	return true;
}

void StreamManager::saveDatabases() {
	if (_logger->isDebug()) _logger->debug(2, "StreamManager::saveDatabases");

	std::auto_ptr<FileOutputStream> fos(new FileOutputStream(const_cast<char*>( (_dataDir + "djondb.dat").c_str()), "wb"));
	for (std::map<std::string, std::map<std::string, SpacesType>* >::iterator idb = _spaces.begin(); idb != _spaces.end(); idb++) {
		std::string db = idb->first;
		std::map<std::string, SpacesType>* spaces = idb->second;
		for (std::map<std::string, SpacesType>::iterator i = spaces->begin(); i != spaces->end(); i++) {
			SpacesType space = i->second;
			std::string ns = space.ns;
			fos->writeString(db);
			fos->writeString(ns);
			fos->writeInt(space.streams->size());
			for (std::map<FILE_TYPE, StreamType*>::iterator istream = space.streams->begin(); istream != space.streams->end(); istream++) {
				FILE_TYPE type = istream->first;
				fos->writeInt(static_cast<int>(type));
				StreamType* stream = istream->second;
				stream->close();
				delete stream;
			}
			delete space.streams;
		}
		delete spaces;
	}
	fos->close();
}

bool StreamManager::dropNamespace(char* db, char* ns) {
	StreamType* stream = NULL;
	bool result = false;
	map<std::string, map<std::string, SpacesType>* >::iterator itspaces = _spaces.find(std::string(db));
	if (itspaces == _spaces.end()) {
		return false;
	}
	map<std::string, SpacesType>* spaces = itspaces->second;

	vector<std::string> keysToRemove;

	for (map<std::string, SpacesType>::iterator it = spaces->begin(); it != spaces->end(); it++) {
		std::string key = it->first;

		if (key.compare(ns) == 0) {
			SpacesType space = it->second;

			std::string filedir = _dataDir + std::string(db) + FILESEPARATOR;

			for (map<FILE_TYPE, StreamType*>::iterator istream = space.streams->begin(); istream != space.streams->end(); istream++) {
				FILE_TYPE type = istream->first;
				std::string file = fileName(std::string(ns), type);

				// drops the file
				if (remove((filedir + file).c_str()) != 0) {
					result = false;
					break;
				}
			}
			keysToRemove.push_back(key);
			result = true;
		}
	}
	// remove the elementsss
	for (std::vector<std::string>::iterator it = keysToRemove.begin(); it != keysToRemove.end(); it++) {
		spaces->erase(*it);
	} 
	return result;
}

std::vector<std::string>* StreamManager::dbs() const {
	std::vector<std::string>* result = new std::vector<std::string>();;

	for (map<std::string, map<std::string, SpacesType>* >::const_iterator it = _spaces.begin(); it != _spaces.end(); it++) {
		std::string db = it->first;
		result->push_back(db);
	}
	return result;
}

std::vector<std::string>* StreamManager::namespaces(const char* db) const {
	std::vector<std::string>* result = new std::vector<std::string>();;

	map<std::string, map<std::string, SpacesType>* >::const_iterator itspaces = _spaces.find(std::string(db));
	if (itspaces != _spaces.end()) {
		map<std::string, SpacesType>* spaces = itspaces->second;
		for (map<std::string, SpacesType>::iterator it = spaces->begin(); it != spaces->end(); it++) {
			std::string key = it->first;
			std::string ns = key.substr(0, key.find_last_of("."));
			result->push_back(ns);
		}

	}
	return result;
}
