// =====================================================================================
// 
//  @file:  commandfactory.cpp
// 
//  @brief:  Implements the methods to retrieve commands, based on the future plans this class should
//  be written using more C code that C++ code to make easier to be ported later
// 
//  @version:  1.0
//  @date:     08/31/2013 06:44:21 AM
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
#include "commandfactory.h"

#include "fetchcommand.h"

FetchCommand* fetchCommand(const char* cursorId) {
	FetchCommand* cmd = new FetchCommand();
	cmd->setCursorId(cursorId);
	return cmd;
}

