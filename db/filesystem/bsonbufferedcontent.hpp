/*
 * =====================================================================================
 *
 *       Filename:  bsonbufferedcontent.hpp
 *
 *    Description:  This is an override of the default BSONBufferedContent to be used with BSONBufferedObj
 *
 *        Version:  1.0
 *        Created:  12/24/2012 12:54:06 PM
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
#ifndef BSONBUFFEREDCONTENT_INCLUDED_HPP
#define BSONBUFFEREDCONTENT_INCLUDED_HPP 
#include "bson.h"

class BSONBufferedContent: public BSONContent {
	public:
		BSONBufferedContent(char* element, BSONTYPE type, __int32 size)
			: BSONContent(type)	{
				_element = element;
				_size = size;
			}

		BSONBufferedContent()
			: BSONContent(NULL_TYPE)	{
				_element = NULL;
				_size = 0;
			}

		virtual ~BSONBufferedContent() {
			// do not delete the contained element
		}

		BSONBufferedContent(const BSONBufferedContent& orig)
	  	  : BSONContent(orig._type) {
			_element = orig._element;
			_size = orig._size;
		}

		virtual BSONContent* clone() const {
			return new BSONBufferedContent(*this);
		}

		operator djondb::string() const {
			return djondb::string((const char*)const_cast<const char*>(_element), _size);
		}

		operator __int32() const {
			__int32* e = (__int32*)_element;
			return *e;
		}

		operator __int64() const {
			__int64* e = (__int64*)_element;
			return *e;
		}

		operator double() const {
			double* e = (double*)_element;
			return *e;
		}

		operator BSONObj*() const {
			BSONObj* e = (BSONObj*)_element;
			return e;
		}

		operator BSONArrayObj*() const {
			BSONArrayObj* e = (BSONArrayObj*)_element;
			return e;
		}

		char* element() {
			return (char*)_element;
		}

		__int32 size() {
			return _size;
		}

	private:
		__int32 _size;
		char* _element;
};

#endif /* BSONBUFFEREDCONTENT_INCLUDED_HPP */
