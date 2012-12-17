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

#include "bsoninputstream.h"
#include "inputstream.h"
#include "bson.h"
#include "bsonutil.h"
#include "util.h"
#include "memorystream.h"

#include <memory>
#include <stdlib.h>

BSONInputStream::BSONInputStream(InputStream* is)
{
	_inputStream = is;
	_log = getLogger(NULL);
}

BSONInputStream::~BSONInputStream()
{
	delete _log;
}

BSONObj* BSONInputStream::readBSON() const {
	return readBSON("*");
}

BSONObj* BSONInputStream::readBSON(const char* select) const {
	Logger* log = getLogger(NULL);

	bool select_all = (strcmp("*", select) == 0);
	std::set<std::string> columns;
	if (!select_all) {
		columns = bson_splitSelect(select);
	}
	BSONObj* obj = new BSONObj();
	__int64 elements = _inputStream->readLong();
	if (elements == 0) {
		cout << "Error" << endl;
	}
	if (log->isDebug()) log->debug("BSONInputStream::readBSON elements: %d", elements);
	BSONInputStream* bis;

	for (__int32 x = 0; x < elements; x++) {
		std::auto_ptr<string> key(_inputStream->readString());

		__int64 type = _inputStream->readLong();
		void* data = NULL;
		BSONObj* inner;
		bool include = false;
		if (select_all || (columns.find(*key.get()) != columns.end())) {
			include = true;
		}
		switch (type) {
			case BSON_TYPE: {
									 char* sub = "*";
									 if (!select_all) {
										 sub = bson_subselect(select, key->c_str());
									 }
									 inner = readBSON(sub);
#ifdef DEBUG
									 if (log->isDebug()) log->debug("BSONInputStream::readBSON key: %s, (BSONObj)", key->c_str());
#endif
									 if (include) {
										 obj->add(*key.get(), *inner);
									 }
									 delete inner;
									 break;
								 }
			case INT_TYPE: {
									if (include) {
										__int32 i = _inputStream->readInt();
										obj->add(*key.get(), i);
#ifdef DEBUG
										if (log->isDebug()) log->debug("BSONInputStream::readBSON key: %s, value: %d", key->c_str(), i);
#endif
									} else {
										// Jumps to the next pos
										_inputStream->seek(_inputStream->currentPos() + sizeof(__int32));
									}
									break;
								}
			case LONG_TYPE: {
										if (include) {
											__int64 l = _inputStream->readLong();
#ifdef DEBUG
											if (log->isDebug()) log->debug("BSONInputStream::readBSON key: %s, value: %d", key->c_str(), l);
#endif
											obj->add(*key.get(), l);
										} else {
											// Jumps to the next pos
											_inputStream->seek(_inputStream->currentPos() + sizeof(__int64));
										}
										break;
									}
			case LONG64_TYPE: {
										if (include) {
											__int64 l = _inputStream->readLong64();
#ifdef DEBUG
											if (log->isDebug()) log->debug("BSONInputStream::readBSON key: %s, value: %d", key->c_str(), l);
#endif
											obj->add(*key.get(), l);
										} else {
											// Jumps to the next pos
											_inputStream->seek(_inputStream->currentPos() + sizeof(__int64));
										}
										break;
									}
			case DOUBLE_TYPE: {
										if (include) {
											double d = _inputStream->readDoubleIEEE();
#ifdef DEBUG
											if (log->isDebug()) log->debug("BSONInputStream::readBSON key: %s, value: %d", key->c_str(), d);
#endif
											obj->add(*key.get(), d);
										} else {
											_inputStream->seek(_inputStream->currentPos() + sizeof(double));
										}
										break;
									}
			case PTRCHAR_TYPE: {
										 // Included only for backward compatibility
										 if (include) {
											 data = _inputStream->readChars();
#ifdef DEBUG
											 if (log->isDebug()) log->debug("BSONInputStream::readBSON key: %s, value: %s", key->c_str(), data);
#endif
											 obj->add(*key.get(), (char*)data);
										 } else {
											 __int32 len = _inputStream->readInt();
											 _inputStream->seek(_inputStream->currentPos() + len);
										 }
										 free((char*)data);
										 break;
									 }
			case STRING_TYPE: {
										if (include) {
											data = _inputStream->readString();
#ifdef DEBUG
											if (log->isDebug()) log->debug("BSONInputStream::readBSON key: %s, value: %s", key->c_str(), ((std::string*)data)->c_str());
#endif
											obj->add(*key.get(), *(std::string*)data);
										} else {
											__int32 len = _inputStream->readInt();
											_inputStream->seek(_inputStream->currentPos() + len);
										}
										delete (std::string*)data;
										break;
									}
			case BSONARRAY_TYPE:
									{
										char* sub = "*";
										if (!select_all) {
											sub = bson_subselect(select, key->c_str());
										}
										BSONArrayObj* array = readBSONInnerArray(sub);
#ifdef DEBUG
										if (log->isDebug()) log->debug("BSONInputStream::readBSON key: %s, (BSONArray)", key->c_str());
#endif
										if (include) {
											obj->add(*key.get(), *array);
										}
										delete array;
										break;
									}
		}
	}
	delete log;
	return obj;
}

BSONArrayObj* BSONInputStream::readBSONInnerArray() const {
	return readBSONInnerArray("*");
}

BSONArrayObj* BSONInputStream::readBSONInnerArray(const char* select) const {
#ifdef DEBUG
	if (_log->isDebug()) _log->debug(3, "BSONInputStream::readBSONInnerArray");
#endif
	__int64 elements = _inputStream->readLong();
#ifdef DEBUG
	if (_log->isDebug()) _log->debug(3, "elements read: %d", elements);
#endif
	BSONArrayObj* result = new BSONArrayObj();

	for (__int32 x= 0; x < elements; x++) {
		BSONObj* obj = readBSON(select);
#ifdef DEBUG
		if (_log->isDebug()) _log->debug(3, "obj: %s", obj->toChar());
#endif
		result->add(*obj);
		delete obj;
	}

	return result;
}

std::vector<BSONObj*>* BSONInputStream::readBSONArray() const {
	return readBSONArray("*");
}

std::vector<BSONObj*>* BSONInputStream::readBSONArray(const char* select) const {
#ifdef DEBUG
	if (_log->isDebug()) _log->debug(3, "BSONInputStream::readBSONArray");
#endif
	__int64 elements = _inputStream->readLong();
#ifdef DEBUG
	if (_log->isDebug()) _log->debug(3, "elements read: %d", elements);
#endif
	std::vector<BSONObj*>* result = new std::vector<BSONObj*>();

	for (__int32 x= 0; x < elements; x++) {
		BSONObj* obj = readBSON(select);
#ifdef DEBUG
		if (_log->isDebug()) _log->debug(3, "obj: %s", obj->toChar());
#endif
		result->push_back(obj);
	}

	return result;
}

