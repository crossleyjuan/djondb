// =====================================================================================
// 
//  @file:  workerfactory.cpp
// 
//  @brief:  Implementation of the worker factory
// 
//  @version:  1.0
//  @date:     07/21/2013 05:00:06 AM
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
#include "workerfactory.h"

#include "command.h"
#include "inputstream.h"
#include "outputstream.h"
#include "worker.h"
#include "findworker.h"
#include "insertworker.h"
#include "updateworker.h"
#include "removeworker.h"
#include "simpleworker.h"

Worker* getWorker(Command* cmd, InputStream* input, OutputStream* output) {
	Worker* result = NULL;
	switch (cmd->commandType()) {
		case INSERT: {
							 result = new InsertWorker(cmd, input, output);
							 break;
						 }

		case UPDATE: {
							 result = new UpdateWorker(cmd, input, output);
							 break;
						 }

		case FIND: {
						  result = new FindWorker((FindCommand*)cmd, input, output);

						  break;
					  }

		case REMOVE: {
							 result = new RemoveWorker(cmd, input, output);
							 break;
						 }
		case CLOSECONNECTION: {
										 break;
									 }

		case SHUTDOWN: 
		case SHOWDBS: 
		case SHOWNAMESPACES: 
		case DROPNAMESPACE: 
		case COMMIT: 
		case FETCHCURSOR: 
		case ROLLBACK: {
								result = new SimpleWorker(cmd, input, output);

								break;
							}
	}
	return result;
}

