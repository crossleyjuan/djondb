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

#include "structure.h"
#include <string>

Structure::Structure()
{
    //ctor
}

Structure::~Structure()
{
    //dtor
}

void Structure::add(std::string name)
{
    _elements.push_back(name);
    _id = 0;
}

long Structure::crc()
{
    if (_id == 0)
    {
        std::stringstream structure;

        for (std::vector<std::string>::const_iterator i = _elements.begin(); i != _elements.end(); i++)
        {
            std::string key = *i;
            structure << key << ";";
        }

        boost::crc_32_type crc;

        std::string s = structure.str();
        const char* buffer = s.c_str();
        crc.process_bytes(buffer, strlen(buffer));
        _id = crc.checksum();
    }
    return _id;
}
