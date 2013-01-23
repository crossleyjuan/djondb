#ifndef BSONOBJ_H
#define BSONOBJ_H

#include "bsoncontent.h"
#include "bsondefs.h"
#include <map>
#include <string>
#include <string.h>
#include "bsonarrayobj.h"

using namespace std;

class BSONObj
{
    public:
        BSONObj();
        BSONObj(const BSONObj& orig);
        virtual ~BSONObj();
//        void add(char*, void*);
        void add(std::string, __int32);
        void add(std::string, double);
        void add(std::string, __int64);
        void add(std::string, char*);
        void add(std::string, char*, __int32);
        void add(std::string, const BSONObj&);
        void add(std::string, const BSONArrayObj&);

        virtual bool has(std::string) const;

        virtual __int32 getInt(std::string) const throw(BSONException);
        virtual double getDouble(std::string) const throw(BSONException);
        virtual __int64 getLong(std::string) const throw(BSONException);
        virtual const std::string getString(std::string) const throw(BSONException);
        virtual BSONObj* getBSON(std::string) const throw(BSONException);
        virtual BSONArrayObj* getBSONArray(std::string) const throw(BSONException);

		  virtual BSONObj* select(const char* sel) const;

        virtual BSONTYPE type(std::string) const;

        virtual char* toChar();

		  typedef std::map<std::string, BSONContent* >::iterator iterator;
		  typedef std::map<std::string, BSONContent* >::const_iterator const_iterator;

        virtual const_iterator begin() const;
        virtual const_iterator end() const;
        virtual __int32 length() const;

		  virtual bool operator ==(const BSONObj& obj) const;
		  virtual bool operator !=(const BSONObj& obj) const;
};

#endif // BSONOBJ_H
