#ifndef FILESYSTEM_DEFS_H_INCLUDED
#define FILESYSTEM_DEFS_H_INCLUDED

#include <string>
#include <map>
#include <vector>

using namespace std;
enum SEEK_DIRECTION {
	FROMSTART_SEEK,
	FROMEND_SEEK
};

class StreamException : public exception {
private:
    string *message;
public:

    StreamException(string *_message) {
        message = _message;
    }

    virtual const char* what() const throw () {
        return message->c_str();
    }
};

#endif

