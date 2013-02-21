/*
 * =====================================================================================
 *
 *       Filename:  controllertest.h
 *
 *    Description:  This is the header file for the DummyController 
 *
 *        Version:  1.0
 *        Created:  12/02/2012 08:17:12 AM
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
#ifndef CONTROLLERTEST_INCLUDED_H
#define CONTROLLERTEST_INCLUDED_H 
#include <vector>
#include <string>
#include "controller.h"
#include "filterdefs.h"
#include "streammanager.h"

class BSONObj;
class BSONArrayObj;
using namespace std;

class DummyController: public Controller
{
	public:
		virtual BSONObj* insert(char* db, char* ns, BSONObj* bson);
		virtual bool dropNamespace(char* db, char* ns);
		virtual void update(char* db, char* ns, BSONObj* bson);
		virtual void remove(char* db, char* ns, char* documentId, char* revision);
		virtual BSONArrayObj* find(char* db, char* ns, const char* select, const char* filter) throw (ParseException);
		virtual BSONObj* findFirst(char* db, char* ns, const char* select, const char* filter) throw (ParseException);
		virtual std::vector<std::string>* dbs() const;
		virtual std::vector<std::string>* namespaces(const char* db) const;
};
#endif /* CONTROLLERTEST_INCLUDED_H */
