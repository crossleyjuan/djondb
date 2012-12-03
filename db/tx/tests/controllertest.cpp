/*
 * =====================================================================================
 *
 *       Filename:  controllertest.cpp
 *
 *    Description:  This is just a dummy to do tests of tx module
 *
 *        Version:  1.0
 *        Created:  12/02/2012 08:05:21 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Juan Pablo Crossley (Cross), crossleyjuan@gmail.com
 *   Organization:  djondb
 *
 * This file is part of the djondb project, for license information please refer to the LICENSE file,
 * the application and libraries are provided as-is and free of use under the terms explained in the file LICENSE
 * Its authors create this application in order to make the world a better place to live, but you should use it on
 * your own risks.
 * 
 * Also, be adviced that, the GPL license force the committers to ensure this application will be free of use, thus
 * if you do any modification you will be required to provide it for free unless you use it for personal use (you may 
 * charge yourself if you want), bare in mind that you will be required to provide a copy of the license terms that ensures
 * this program will be open sourced and all its derivated work will be too.
 * =====================================================================================
 */
#include "controllertest.h"
#include "bson.h"

BSONObj* DummyController::insert(char* db, char* ns, BSONObj* bson)  {
	return bson;
}

bool DummyController::dropNamespace(char* db, char* ns)  {
	return true;
}

void DummyController::update(char* db, char* ns, BSONObj* bson)  {
}

void DummyController::remove(char* db, char* ns, const std::string& documentId, const std::string& revision)  {
}

BSONArrayObj* DummyController::find(char* db, char* ns, const char* select, const char* filter) throw (ParseException)  {
	return new BSONArrayObj();
}

BSONObj* DummyController::findFirst(char* db, char* ns, const char* select, const char* filter) throw (ParseException)  {
	return NULL;
}

std::vector<std::string>* DummyController::dbs() const  {
	return new std::vector<std::string>();
}

std::vector<std::string>* DummyController::namespaces(const char* db) const  {
	return new std::vector<std::string>();
}

