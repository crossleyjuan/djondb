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

#include "commandreader.h"
#include "insertcommand.h"
#include "dropnamespacecommand.h"
#include "updatecommand.h"
#include "findcommand.h"
#include "shutdowncommand.h"
#include "shownamespacescommand.h"
#include "showdbscommand.h"
#include "removecommand.h"
#include "bsoninputstream.h"
#include "util.h"
#include <memory>
#include <assert.h>

CommandReader::CommandReader(InputStream* is)
{
    _stream = is;
}

CommandReader::CommandReader(const CommandReader& orig) {
    this->_stream = orig._stream;
}

CommandReader::~CommandReader()
{
}

InsertCommand* parseInsert(InputStream* is)  {
	 Logger* log = getLogger(NULL);
    InsertCommand* command = new InsertCommand();
    std::string* db = is->readString();
    command->setDB(*db);
    std::string* ns = is->readString();
    command->setNameSpace(*ns);

	 /*
	 std::string* s = is->readString();
	 BSONObj* obj = BSONParser::parse(*s);
	 delete s;
	 */
    std::auto_ptr<BSONInputStream> bsonis(new BSONInputStream(is));
    BSONObj* obj = bsonis->readBSON();
    command->setBSON(*obj);
	 
	 delete log;

    delete ns;
    delete db;
    delete obj;
    return command;
}

DropnamespaceCommand* parseDropnamespace(InputStream* is)  {
    DropnamespaceCommand* command = new DropnamespaceCommand();
    std::string* db = is->readString();
    command->setDB(*db);
    std::string* ns = is->readString();
    command->setNameSpace(*ns);

    delete db;
    delete ns;
    return command;
}

ShowdbsCommand* parseShowdbsCommand(InputStream* is)  {
    ShowdbsCommand* command = new ShowdbsCommand();

    return command;
}

ShownamespacesCommand* parseShownamespacesCommand(InputStream* is)  {
    ShownamespacesCommand* command = new ShownamespacesCommand();
    std::string* db = is->readString();
    command->setDB(*db);

    delete db;
    return command;
}

ShutdownCommand* parseShutdown(InputStream* is)  {
    ShutdownCommand* command = new ShutdownCommand();

    return command;
}

UpdateCommand* parseUpdate(InputStream* is)  {
    UpdateCommand* command = new UpdateCommand();
    std::string* db = is->readString();
    command->setDB(*db);
    std::string* ns = is->readString();
    command->setNameSpace(*ns);
    std::auto_ptr<BSONInputStream> bsonis(new BSONInputStream(is));
    BSONObj* obj = bsonis->readBSON();
    command->setBSON(*obj);

    delete ns;
    delete db;
    delete obj;
    return command;
}

RemoveCommand* parseRemove(InputStream* is)  {
    RemoveCommand* command = new RemoveCommand();
    std::string* db = is->readString();
    command->setDB(*db);
    std::string* ns = is->readString();
    command->setNameSpace(*ns);
    std::string* id = is->readString();
    command->setId(*id);
    std::string* revision = is->readString();
    command->setRevision(*revision);

    delete ns;
    delete db;
    delete id;
    delete revision;
    return command;
}

FindCommand* parseFind(InputStream* is)  {
    FindCommand* command = new FindCommand();
    std::string* db = is->readString();
    command->setDB(*db);
    std::string* ns = is->readString();
    command->setNameSpace(*ns);
    std::string* filter = is->readString();
    command->setFilter(*filter);
	 std::string* select = is->readString();
	 command->setSelect(*select);

    delete db;
    delete ns;
    delete filter;
	 delete select;
    return command;
}

Command* CommandReader::readCommand() {
    if (_stream->isClosed()) {
        return NULL;
    }
	// at this moment no check has been made, but the version will
	// allow to control that the sender has the same version of the
	// server
	Logger* log = getLogger(NULL);
	if (log->isDebug()) log->debug("readCommand: reading version");
	std::string* version = _stream->readString();
	if (log->isDebug()) log->debug("readCommand: version %s", version->c_str());
	delete version;

    COMMANDTYPE type = static_cast<COMMANDTYPE>(_stream->readInt());

	 if (log->isDebug()) log->debug("readCommand: type %d", type);
    Command* cmd = NULL;
    switch (type) {
        case INSERT: // Insert
            cmd = parseInsert(_stream);
            break;
        case UPDATE: // Update
            cmd = parseUpdate(_stream);
            break;
        case FIND:
            cmd = parseFind(_stream);
            break;
        case DROPNAMESPACE:
            cmd = parseDropnamespace(_stream);
            break;
        case SHUTDOWN:
            cmd = parseShutdown(_stream);
            break;
        case CLOSECONNECTION: // Insert
            cmd = new CloseCommand();
            break;
        case SHOWDBS: // Showdbs
				cmd = parseShowdbsCommand(_stream);
            break;
        case SHOWNAMESPACES: // Shownamepsaces
				cmd = parseShownamespacesCommand(_stream);
            break;
        case REMOVE: // Remove
				cmd = parseRemove(_stream);
            break;
        default:
            cout << "unknown command type " << type << endl;
            break;
    }
    assert(cmd != NULL);
	 delete log;
    return cmd;
}

