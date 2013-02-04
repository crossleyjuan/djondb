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

#include "shutdowncommand.h"

#include "bsonoutputstream.h"
#include "dbcontroller.h"
#include "outputstream.h"
#include "util.h"

ShutdownCommand::ShutdownCommand()
: Command(SHUTDOWN)
{
	//ctor
}

ShutdownCommand::~ShutdownCommand()
{
}

ShutdownCommand::ShutdownCommand(const ShutdownCommand& other)
:Command(SHUTDOWN)
{
}

void ShutdownCommand::execute() {
	Logger* log = getLogger(NULL);
	if (log->isDebug()) log->debug("executing shutdown command ");
}

void* ShutdownCommand::result() {
	return NULL;
}

void ShutdownCommand::writeCommand(OutputStream* out) const {
}

void ShutdownCommand::readResult(InputStream* is)  {
}

void ShutdownCommand::writeResult(OutputStream* out) const {
	Logger* log = getLogger(NULL);
}

