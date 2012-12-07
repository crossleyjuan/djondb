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
        void add(std::string, std::string);
        void add(std::string, const BSONObj&);
        void add(std::string, const BSONArrayObj&);

        bool has(std::string) const;

        __int32 getInt(std::string) const throw(BSONException);
        double getDouble(std::string) const throw(BSONException);
        __int64 getLong(std::string) const throw(BSONException);
        std::string getString(std::string) const throw(BSONException);
        BSONObj* getBSON(std::string) const throw(BSONException);
        BSONArrayObj* getBSONArray(std::string) const throw(BSONException);
        BSONContent get(std::string) const throw(BSONException);
        BSONContent* getContent(std::string) const;
        BSONContent* getContent(std::string, BSONTYPE) const;

		  BSONContent getXpath(const std::string& xpath) const;

		  BSONObj* select(const char* sel) const;

        BSONTYPE type(std::string) const;

        char* toChar();

		  typedef std::map<std::string, BSONContent* >::iterator iterator;
		  typedef std::map<std::string, BSONContent* >::const_iterator const_iterator;

        const_iterator begin() const;
        const_iterator end() const;
        __int32 length() const;

		  bool operator ==(const BSONObj& obj) const;
		  bool operator !=(const BSONObj& obj) const;

    protected:
	 private:
		  void fillContent(std::string kkey, BSONTYPE ttype, void* vval);
	 private:
		  std::map<std::string, BSONContent* > _elements;
		  char* _cBSON;
};

#endif // BSONOBJ_H
