#ifndef BSONPARSER_H
#define BSONPARSER_H

#include <string>
#include "bsondefs.h"

class BSONObj;
class BSONArrayObj;

class BSONParser
{
	public:
		/** Default constructor */
		BSONParser();
		/** Default destructor */
		virtual ~BSONParser();

		static BSONObj* parse(const std::string& sbson);
		static BSONArrayObj* parseArray(const std::string& sbson);

	protected:
	private:
		static BSONObj* parseBSON(const char* c, int& pos) throw(BSONException);
		static BSONArrayObj* parseArray(const char* chrs, int& pos);
};

#endif // BSONPARSER_H
