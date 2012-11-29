/*
 * =====================================================================================
 *
 *       Filename:  controller.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  09/27/2012 09:58:58 PM
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

#ifndef CONTROLLER_INCLUDED_H
#define CONTROLLER_INCLUDED_H
#include <vector>
#include <string>
#include "filterdefs.h"
#include "streammanager.h"

class BSONObj;
class BSONArrayObj;
using namespace std;

class Controller 
{
	public:
		virtual BSONObj* insert(char* db, char* ns, BSONObj* bson) = 0;
		virtual bool dropNamespace(char* db, char* ns) = 0;
		virtual void update(char* db, char* ns, BSONObj* bson) = 0;
		virtual void remove(char* db, char* ns, const std::string& documentId, const std::string& revision) = 0;
		virtual BSONArrayObj* find(char* db, char* ns, const char* select, const char* filter) throw (ParseException) = 0;
		virtual BSONObj* findFirst(char* db, char* ns, const char* select, const char* filter) throw (ParseException) = 0;
		virtual std::vector<std::string>* dbs() const = 0;
		virtual std::vector<std::string>* namespaces(const char* db) const = 0;
};
#endif // CONTROLLER_INCLUDED_H
