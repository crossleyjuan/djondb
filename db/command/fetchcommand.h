// =====================================================================================
// 
//  @file:  fetchcommand.h
// 
//  @brief:  Execute fetch over a cursor retrieving the next page
// 
//  @version:  1.0
//  @date:     08/14/2013 09:37:51 PM
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
#ifndef FETCHCOMMAND_INCLUDED_H
#define FETCHCOMMAND_INCLUDED_H 

#include "command.h"

class OutputStream;
class InputStream;
class BSONArrayObj;

class FetchCommand: public Command {
	public:
		FetchCommand();
		virtual ~FetchCommand();
		FetchCommand(const FetchCommand& other);

		virtual void execute();
		virtual void* result();

		virtual void writeCommand(OutputStream* out) const;
		virtual void writeResult(OutputStream* out) const;
		virtual void readResult(InputStream* is);

		void setCursorId(const char* cursorId);
		const char* cursorId() const;

	private:
		char* _cursorId;

		BSONArrayObj* _result;
};

#endif /* FETCHCOMMAND_INCLUDED_H */
