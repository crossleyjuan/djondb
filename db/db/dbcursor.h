// =====================================================================================
// 
//  @file:  dbcursor.h
// 
//  @version:  1.0
//  @date:     07/10/2013 01:23:19 PM
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
#ifndef DBCURSOR_INCLUDED_H
#define DBCURSOR_INCLUDED_H 

#include "util.h"
#include "string.h"
#include <set>

class BSONObj;
class FilterParser;
class BSONArrayObj;

/**
 * @brief  DBCursor struct containing the elements of a cursor for non transactional cursors 
 */
struct DBCursor {
	public:
		char* cursorId;
		char* db; //!< Database source
		char* ns; //!< Namespace source
		char* select; //!< comma separated fields to select, "*" for all
		char* filter; //!< filter of the records, NULL if all or ""
		BSONObj* options;
		char* dataDir; //!< Contains the data directory

		int status; //!< Status of the current cursor 0 - Closed, 1 - Open

		int rowsPerPage; //!< rows per page 
		int cursorType; //!< 0 - Index oriented cursor, 1 - Full Scan
		int maxResults; //!< this will restrict the number of records, either by using "top" option or by default size

		int count; //!< Currently retrieved rows, this will contain the retrieved rows, not the total rows
		int currentPosition; //!< Current row
		int currentPageIndex; //!< Current page in the cursor

		FilterParser* parser; //!< Parser to filter the rows
		std::set<std::string> tokens; //!< fileds tokens found on the filter
		BSONArrayObj* rows; //!< this will store the retrieved result
		BSONArrayObj* currentPage; //!< Current page

		char* fileName; //!< File name of the mapped file to do the fullscans
		__int64 position; //!< Current position of the cursor
		bool allRecordsLoaded; //!< At this moment is the only way to mark the cursor as "loaded"
};

/// Functions to control cursors

/**
 * @brief Loads the current page based on the cursor data
 *
 * @param cursor to be used as base to retrieve the page
 */
void dbcursor_currentPage(DBCursor* cursor);

/**
 * @brief Unique method to initialize Cursors
 *
 * @param db
 * @param ns
 * @param select
 * @param filter
 * @param options
 *
 * @return 
 */
DBCursor* createCursor(const char* db, const char* ns, const char* select, const char* filter, const BSONObj* options);

#endif /* DBCURSOR_INCLUDED_H */
