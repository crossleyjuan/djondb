#ifndef STRUCTURE_H
#define STRUCTURE_H

#include "bson.h"
#include <vector>
#include <boost/crc.hpp>
#include <string>
#include <string.h>
#include <sstream>

using namespace std;

class Structure
{
    public:
        Structure();
        virtual ~Structure();
        void add(std::string name);

        long crc();

    protected:
    private:
        long _id; // CRC Code
        vector<std::string> _elements;
};


#endif // STRUCTURE_H
