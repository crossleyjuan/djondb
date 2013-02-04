/*
 * =====================================================================================
 *
 *       Filename:  bsonbuffered.h
 *
 *    Description:  this is the definition of a bsonobj buffered representation, it's a readonly
 *                  structure that maps a memory buffer using bson logic 
 *
 *        Version:  1.0
 *        Created:  12/21/2012 02:07:05 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Juan Pablo Crossley (Cross), crossleyjuan@gmail.com
 *   Organization:  djondb
 *
 * This file is part of the djondb project, for license information please refer to the LICENSE file,
 * the application and libraries are provided as-is and free of use under the terms explained in the file LICENSE
 * Its authors create this application in order to make the world a better place to live, but you should use it on
 * your own risks.
 * 
 * Also, be adviced that, the GPL license force the committers to ensure this application will be free of use, thus
 * if you do any modification you will be required to provide it for free unless you use it for personal use (you may 
 * charge yourself if you want), bare in mind that you will be required to provide a copy of the license terms that ensures
 * this program will be open sourced and all its derivated work will be too.
 * =====================================================================================
 */ 
#ifndef BSONBUFFERED_INCLUDED_H
#define BSONBUFFERED_INCLUDED_H 

#include "bsoncontent.h"
#include "bsondefs.h"
#include "bsonobj.h"
#include <map>
#include <string>
#include <string.h>
#include "bsonarrayobj.h"
#include <limits.h>

using namespace std;

class BSONBufferedObj: public BSONObj
{
    public:
        BSONBufferedObj(char* buffer, __int64 len);
		  // Unsupported copy constructor, this is defined to avoid calling the method
        BSONBufferedObj(const BSONBufferedObj& orig);
        virtual ~BSONBufferedObj();
		  void reset(char* buffer, int len);

        virtual bool has(std::string) const;

        virtual __int32 getInt(std::string) const throw(BSONException);
        virtual double getDouble(std::string) const throw(BSONException);
        virtual __int64 getLong(std::string) const throw(BSONException);
        virtual const djondb::string getDJString(std::string) const throw(BSONException);
        virtual BSONBufferedObj* getBSON(std::string) const throw(BSONException);
        virtual BSONArrayObj* getBSONArray(std::string) const throw(BSONException);
        virtual BSONContent* get(std::string) const throw(BSONException);
        virtual BSONContent* getContent(std::string) const;
        virtual BSONContent* getContent(std::string, BSONTYPE) const;

		  virtual BSONContent* getXpath(const std::string& xpath) const;

		  virtual BSONObj* select(const char* sel) const;

        virtual BSONTYPE type(std::string) const;

        virtual char* toChar();

		  typedef std::map<std::string, BSONContent* >::iterator iterator;
		  typedef std::map<std::string, BSONContent* >::const_iterator const_iterator;

        virtual const_iterator begin() const;
        virtual const_iterator end() const;
        virtual __int32 length() const;
        virtual __int64 bufferLength() const;

		  virtual bool operator ==(const BSONBufferedObj& obj) const;
		  virtual bool operator !=(const BSONBufferedObj& obj) const;

    protected:
	 private:
		  void initialize();
		  char* getValue(char* key) const;
		  BSONTYPE getType(char* key) const;

		  char readChar(char*& buffer)  const {
			  char c = buffer[0];
			  buffer += 1;
			  return c;
		  }

		  template <typename T>
			  T readData(char*& buffer) {
				  T result = 0;
				  unsigned char* v = (unsigned char*)&result;
				  int size = sizeof(T);
				  for (int i = 0; i < size; i++) {
					  v[i] = readChar(buffer) & UCHAR_MAX;
				  }
				  T clear = 0;
				  for (int i = 0; i < size; i++) {
					  clear = clear << 8;
					  clear = clear | 0xFF;
				  }
				  //printf("\nresult before add: %x\n", result);
				  result = result & clear;
				  //printf("result after add: %x\n", result);
				  return result;
			  }

	 private:
		  char* _buffer;
		  __int64   _bufferMaxLen;

		  __int64   _bufferBSONLen;

		  // Pointers to the values in the stream
		  char**    _keys;
		  __int32** _keySize;
		  __int64** _types;
		  char**    _values;
		  __int64   _elements;

		  char* _cBSON;
};

#endif /* BSONBUFFERED_INCLUDED_H */
