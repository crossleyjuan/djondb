#ifndef BSONCONTENT_H_INCLUDED
#define BSONCONTENT_H_INCLUDED

#include "bsondefs.h"
#include "util.h"
#include <string>

class BSONArrayObj;
class BSONObj;

class BSONContent {
    public:
        BSONContent();
        virtual ~BSONContent();

        BSONContent(const BSONContent& orig);
        void* _element;

		  BSONTYPE type() const {
			  return _type;
		  };

		  void setType(BSONTYPE type) {
			  _type = type;
		  }

		  bool operator ==(const BSONContent& content);
		  bool operator !=(const BSONContent& content);
		  operator __int32();
		  operator __int64();
		  operator double();
		  operator std::string();
		  operator BSONObj();
		  operator BSONArrayObj();
	protected:
        BSONTYPE _type;
};

#endif // BSONCONTENT_H_INCLUDED
