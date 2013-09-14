// =====================================================================================
// 
//  @file:  dbcursor.cpp
// 
//  @brief:  
// 
//  @version:  1.0
//  @date:     08/05/2013 01:29:52 PM
//  Compiler:  g++
// 
//  @author:  Juan Pablo Crossley (Cross), crossleyjuan@gmail.com
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
// =====================================================================================
#include "dbcursor.h"

#include "util.h"
#include "bson.h"

void dbcursor_currentPage(DBCursor* cursor) {
	int startPos = cursor->currentPageIndex * cursor->rowsPerPage;
	int rows = 0;
	BSONArrayObj* result = NULL;
	while ((startPos < cursor->count) && (rows < cursor->rowsPerPage)) {
		if (result == NULL) result = new BSONArrayObj();
		result->add(*cursor->rows->get(startPos));
		startPos++;
		rows++;
	}
	cursor->currentPage = result;
}

DBCursor* createCursor(const char* db, const char* ns, const char* select, const char* filter, const BSONObj* options) {
	DBCursor* cursor = new DBCursor();
	std::string* tempID = uuid();
	cursor->cursorId = strcpy(tempID->c_str());
	delete tempID;

	cursor->db = strcpy(db, strlen(db));
	cursor->ns = strcpy(ns, strlen(ns));
	if (select != NULL) {
		cursor->select = strcpy(select, strlen(select));
	} else {
		cursor->select = NULL;
	}
	if (filter != NULL) {
		cursor->filter = strcpy(filter, strlen(filter));
	} else {
		cursor->filter = NULL;
	}
	if (options != NULL) {
		cursor->options = new BSONObj(*options);
	} else {
		cursor->options = NULL;
	}

	cursor->rows = NULL;
	cursor->currentPage = NULL;
	return cursor;
}
