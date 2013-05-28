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

#include "commandwriter.h"
#include "insertcommand.h"
#include "dropnamespacecommand.h"
#include "updatecommand.h"
#include "findcommand.h"
#include "shutdowncommand.h"
#include "bsonoutputstream.h"
#include "shownamespacescommand.h"
#include <memory>
#include <iostream>

using namespace std;

CommandWriter::CommandWriter(OutputStream* out)
{
	_stream = out;
}

CommandWriter::~CommandWriter()
{
	//dtor
}


CommandWriter::CommandWriter(const CommandWriter& orig) {
	this->_stream = orig._stream;
}

int CommandWriter::writeCommand(Command* cmd) {
	std::string version = "0.3.0";
	_stream->writeString(version);
	int type = cmd->commandType();
	_stream->writeInt(type);

	BSONOutputStream out(_stream);
	if (cmd->options() != NULL) {
		out.writeBSON(*cmd->options());
	} else {
		BSONObj options;
		out.writeBSON(options);
	}
	cmd->writeCommand(_stream);

	return 0;
}

