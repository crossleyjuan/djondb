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

#include "dbcontroller.h"
#include "bson.h"
#include "util.h"
#include "fileinputoutputstream.h"
#include "fileinputstream.h"
#include "fileoutputstream.h"

#include "bsonoutputstream.h"
#include "bsoninputstream.h"
#include "filterparser.h"
#include "baseexpression.h"
#include "expressionresult.h"

#include "cachemanager.h"
#include "indexfactory.h"
#include "index.h"
#include <memory>
#include <iostream>
#include <sstream>
#include <string.h>

using namespace std;

DBController::DBController()
{
	_logger = getLogger(NULL);
}

DBController::~DBController()
{
	delete _logger;
}

void DBController::shutdown() {
	if (_logger->isInfo()) _logger->info("DBController shutting down");
	StreamManager::getStreamManager()->saveDatabases();
	clearCache();
}

void DBController::clearCache() {
	if (_logger->isInfo()) _logger->info("DBController::clearCache");
	for (ObjectCache::iterator i = CacheManager::objectCache()->begin(); i != CacheManager::objectCache()->end(); i++) {
		BSONObj* item = i->second;
		delete item;
	}
	CacheManager::objectCache()->clear();

	for (StructureCache::iterator i = CacheManager::structuresCache()->begin(); i != CacheManager::structuresCache()->end(); i++) {
		Structure* item = i->second;
		delete item;
	}
	CacheManager::structuresCache()->clear();

}

void DBController::initialize() {
	std::string dataDir = getSetting("DATA_DIR");
	if ((dataDir.length() > 0) && !endsWith(dataDir.c_str(), FILESEPARATOR)) {
		dataDir = dataDir + FILESEPARATOR;
	} else {
		dataDir = dataDir;
	}

	initialize(dataDir);
	_initialized = true;
}

void DBController::initialize(std::string dataDir) {
	if (_logger->isInfo()) _logger->info("DBController initializing. Data dir");

	_dataDir = dataDir;

	if (_logger->isDebug()) _logger->debug(0, "data dir = %s", _dataDir.c_str());

	if (!existDir(_dataDir.c_str())) {
		makeDir(_dataDir.c_str());
	}

	if (!checkFileCreation(_dataDir.c_str())) {
		_logger->error("An error ocurred using the data folder: %s. Please check that the user has permissions for writing over that directory. Error: %s", _dataDir.c_str(), lastErrorDescription()); 
		exit(1);
	}

	StreamManager::getStreamManager()->setDataDir(_dataDir);

	std::auto_ptr<FileInputStream> fis(new FileInputStream((_dataDir + "djondb.dat").c_str(), "rb"));
	while (!fis->eof()) {
		std::auto_ptr<std::string> db(fis->readString());
		std::auto_ptr<std::string> ns(fis->readString());
		int streams = fis->readInt();
		for (int x = 0; x < streams; x++) {
			FILE_TYPE type = static_cast<FILE_TYPE>(fis->readInt());
			StreamType* stream = StreamManager::getStreamManager()->open(*(db.get()), *(ns.get()), type);

			if (type == INDEX_FTYPE) {
				long currentPos = stream->currentPos();
				stream->seek(0);


				std::string mark = stream->readChars(10);
				// check if the file is marked as versioned version (0.1 version does not have this mark)
				Version dbversion("0.1");
				if (mark.compare("1234") == 0) {
					char* version = stream->readChars(11);
					dbversion = Version(version);
				} else {
					stream->seek(0);
				}
				IndexAlgorithm* impl = NULL;
				while (!stream->eof()) {
					BSONObj* obj = readBSON(stream);

					if (!impl) {
						std::set<std::string> skeys;
						for (BSONObj::const_iterator i = obj->begin(); i != obj->end(); i++) {
							std::string key = i->first;
							skeys.insert(key);
						}
						impl = IndexFactory::indexFactory.index(db->c_str(), ns->c_str(), skeys);
					}
					long indexPos = stream->readLong();
					long posData = stream->readLong();
					impl->add(*obj, obj->getString("_id"), posData, indexPos);
					delete obj;
				}
				stream->seek(currentPos);
			}
		}
	}
	fis->close();
}

long DBController::checkStructure(BSONObj* obj) {
	Structure* structure = new Structure();
	for (std::map<std::string, BSONContent* >::const_iterator i = obj->begin(); i != obj->end(); i++) {
		structure->add(i->first);
	}

	StructureCache* cache = CacheManager::structuresCache();
	long strId = structure->crc();
	if (!cache->containsKey(strId)) {
		cache->add(strId, structure);
	} else {
		delete(structure);
	}
	return strId;
}

void DBController::writeBSON(StreamType* stream, BSONObj* obj) {
	if (_logger->isDebug()) _logger->debug(3, "DBController is writing bson to disc: %s", obj->toChar());

	auto_ptr<BSONOutputStream> out(new BSONOutputStream(stream));
	out->writeBSON(*obj);
	stream->flush();
}

BSONObj* DBController::readBSON(StreamType* stream) {
	auto_ptr<BSONInputStream> is(new BSONInputStream(stream));
	BSONObj* res = is->readBSON();
	if (_logger->isDebug()) _logger->debug(3, "DBController read bson from disc: %s", res->toChar());
	return res;
}

BSONObj* DBController::insert(char* db, char* ns, BSONObj* obj) {
	if (_logger->isDebug()) _logger->debug(2, "DBController::insert ns: %s, bson: %s", ns, obj->toChar());
	std::string sdb(db);
	std::string sns(ns);
	StreamType* streamData = StreamManager::getStreamManager()->open(sdb, sns, DATA_FTYPE);

	BSONObj* result = NULL;
	if (!obj->has("_id")) {
		if (_logger->isDebug()) _logger->debug(2, "BSON does not contain an id, DBController is creating one");
		string* tid = uuid();
		std::string key("_id");
		obj->add(key, *tid);
		result = new BSONObj();
		result->add("_id", *tid);
		delete tid;
	}
	if (!obj->has("_revision")) {
		if (_logger->isDebug()) _logger->debug(2, "BSON does not contain revision, DBController is creating one");
		string* trev = uuid();
		std::string key("_revision");
		obj->add(key, *trev);
		result = new BSONObj();
		result->add("_revision", *trev);
		delete trev;
	}
	// _status flag
	obj->add("_status", 1);

	std::string id;
	if (obj->type("_id") == STRING_TYPE) {
		id = obj->getString("_id");
	}
	assert(id.length() > 0);
	//    long crcStructure = checkStructure(obj);

	//    char* text = obj->toChar();
	//    streamData->writeChars(text, strlen(text));
	//    free(text);

	insertIndex(db, ns, obj, streamData->currentPos());

	writeBSON(streamData, obj);

	//CacheManager::objectCache()->add(id, new BSONObj(*obj));

	return result;
}

void DBController::update(char* db, char* ns, BSONObj* obj) {
	if (_logger->isDebug()) _logger->debug(2, "DBController::update ns: %s, bson: %s", ns, obj->toChar());
	StreamType* streamData = StreamManager::getStreamManager()->open(std::string(db), std::string(ns), DATA_FTYPE);

	//    long crcStructure = checkStructure(obj);

	//    char* text = obj->toChar();
	//    streamData->writeChars(text, strlen(text));
	//    free(text);

	updateIndex(db, ns, obj, streamData->currentPos());

	writeBSON(streamData, obj);

	std::string id = obj->getString("_id");

	CacheManager::objectCache()->add(id, new BSONObj(*obj));
}

void DBController::deleteRecord(char* db, char* ns, const std::string& documentId, const std::string& revision) {
	if (_logger->isDebug()) _logger->debug(2, "DBController::update db: %s, ns: %s, documentId: %s, revision: %s", db, ns, documentId.c_str(), revision.c_str());
	StreamType* streamData = StreamManager::getStreamManager()->open(std::string(db), std::string(ns), DATA_FTYPE);

	//    long crcStructure = checkStructure(obj);

	//    char* text = obj->toChar();
	//    streamData->writeChars(text, strlen(text));
	//    free(text);

	IndexAlgorithm* impl = IndexFactory::indexFactory.index(db, ns, "_id");

	BSONObj indexBSON;
	indexBSON.add("_id", documentId);
	Index* index = impl->find(indexBSON);
	if (index != NULL) {

		// TODO check the revision id
		StreamType* out = StreamManager::getStreamManager()->open(db, ns, DATA_FTYPE);
		out->flush();

		long currentPos = out->currentPos();

		out->seek(index->posData);

		BSONObj* obj = readBSON(out);
		obj->add("_status", 2); // DELETED

		writeBSON(out, obj);

		out->seek(currentPos);

		std::string id = obj->getString("_id");

		CacheManager::objectCache()->add(id, new BSONObj(*obj));
	}
}

void DBController::updateIndex(char* db, char* ns, BSONObj* bson, long filePos) {

	IndexAlgorithm* impl = IndexFactory::indexFactory.index(db, ns, "_id");

	BSONObj indexBSON;
	indexBSON.add("_id", bson->getString("_id"));
	Index* index = impl->find(indexBSON);

	index->posData = filePos;

	StreamType* out = StreamManager::getStreamManager()->open(db, ns, INDEX_FTYPE);
	long currentPos = out->currentPos();
	out->seek(index->indexPos);
	BSONObj* key = index->key;
	writeBSON(out, key);
	out->writeLong(index->indexPos);
	out->writeLong(index->posData);
	out->seek(currentPos);
}

void DBController::insertIndex(char* db, char* ns, BSONObj* bson, long filePos) {
	BSONObj indexBSON;
	std::string id = bson->getString("_id");
	indexBSON.add("_id", id);

	IndexAlgorithm* impl = IndexFactory::indexFactory.index(db, ns, "_id");

	StreamType* out = StreamManager::getStreamManager()->open(std::string(db), std::string(ns), INDEX_FTYPE);

	impl->add(indexBSON, id, filePos, out->currentPos());

	writeBSON(out, &indexBSON);
	out->writeLong(out->currentPos());
	out->writeLong(filePos);
}

std::vector<BSONObj*>* DBController::find(char* db, char* ns, const char* select, const char* filter) throw(ParseException) {
	if (_logger->isDebug()) _logger->debug(2, "DBController::find db: %s, ns: %s, select: %s, filter: %s", db, ns, select, filter);

	std::vector<BSONObj*>* result;

	FilterParser* parser = FilterParser::parse(filter);

	std::set<std::string> tokens = parser->tokens();

	if (IndexFactory::indexFactory.containsIndex(db, ns, tokens)) {
		IndexAlgorithm* impl = IndexFactory::indexFactory.index(db, ns, tokens);

		std::list<Index*> elements = impl->find(parser);

		std::string filedir = _dataDir + db;
		filedir = filedir + FILESEPARATOR;

		std::stringstream ss;
		ss << filedir << ns << ".dat";

		std::string filename = ss.str();
		result = new std::vector<BSONObj*>();
		FileInputStream* fis = new FileInputStream(filename.c_str(), "rb");

		BSONInputStream* bis = new BSONInputStream(fis);
		for (std::list<Index*>::iterator it = elements.begin(); it != elements.end(); it++) {
			Index* index = *it;

			long posData = index->posData;
			fis->seek(posData);

			BSONObj* obj = bis->readBSON(select);

			result->push_back(obj);
		}
		delete bis;
		delete fis;

	} else {
		result = findFullScan(db, ns, select, parser);
	}

	return result;
}

BSONObj* DBController::findFirst(char* db, char* ns, const char* select, const char* filter) throw(ParseException) {
	if (_logger->isDebug()) _logger->debug(2, "DBController::findFirst db: %s, ns: %s, select: %s, filter: %s", db, ns, select, filter);
	std::string filedir = _dataDir + db;
	filedir = filedir + FILESEPARATOR;

	std::stringstream ss;
	ss << filedir << ns << ".dat";

	std::string filename = ss.str();

	FileInputStream* fis = new FileInputStream(filename.c_str(), "rb");
	std::vector<BSONObj*> result;

	BSONInputStream* bis = new BSONInputStream(fis);

	FilterParser* parser = FilterParser::parse(filter);

	BSONObj* bsonResult = NULL;
	while (!fis->eof()) {
		BSONObj* obj = bis->readBSON();

		ExpressionResult* result = parser->eval(*obj);
		if (result->type() == ExpressionResult::RT_BOOLEAN) {
			bool* bres = (bool*)result->value();
			if (*bres) {
				bsonResult = obj->select(select);
				delete obj;
				break;
			}
		}
		delete result;
		delete obj;
		if (bsonResult) {
			break;
		}
	}

	delete parser;
	return bsonResult;
}

std::vector<BSONObj*>* DBController::findFullScan(char* db, char* ns, const char* select, FilterParser* parser) throw(ParseException) {
	if (_logger->isDebug()) _logger->debug(2, "DBController::findFullScan with parser db: %s, ns: %s", db, ns);
	std::string filedir = _dataDir + db;
	filedir = filedir + FILESEPARATOR;

	std::stringstream ss;
	ss << filedir << ns << ".dat";

	std::string filename = ss.str();

	FileInputStream* fis = new FileInputStream(filename.c_str(), "rb");
	std::vector<BSONObj*>* result = new std::vector<BSONObj*>();

	BSONInputStream* bis = new BSONInputStream(fis);

	while (!fis->eof()) {
		BSONObj* obj = bis->readBSON();

		bool match = false;
		ExpressionResult* expresult = parser->eval(*obj);
		if (expresult->type() == ExpressionResult::RT_BOOLEAN) {
			bool* bres = (bool*)expresult->value();
			match = *bres;
		}
		if (match) {
			result->push_back(obj->select(select));
		} else {
		}
		delete obj;
		delete expresult;
	}

	delete parser;
	delete bis;
	delete fis;

	return result;
}

std::vector<std::string>* DBController::dbs() const {
	return StreamManager::getStreamManager()->dbs();
}

std::vector<std::string>* DBController::namespaces(const char* db) const {
	return StreamManager::getStreamManager()->namespaces(db);
}

bool DBController::dropNamespace(char* db, char* ns) {
	return StreamManager::getStreamManager()->dropNamespace(db, ns);
}
