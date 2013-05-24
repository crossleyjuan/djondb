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

#include "bsoncontent.h"
#include "bsonobj.h"
#include "bsonarrayobj.h"
#include "util.h"
#include <stdlib.h>
#include <assert.h>

using namespace std;

BSONContent::BSONContent(BSONTYPE type) {
	this->_type = type;
}

BSONContentBoolean::BSONContentBoolean(bool element):
	BSONContent(BOOL_TYPE)
{
	_element = element;
}

BSONContentBoolean::BSONContentBoolean(const BSONContentBoolean& orig):
	BSONContent(BOOL_TYPE)
{
	this->_element = orig._element;
}

BSONContentBoolean* BSONContentBoolean::clone() const {
	return new BSONContentBoolean(*this);
}

BSONContentInt::BSONContentInt(__int32 element):
	BSONContent(INT_TYPE)
{
	_element = element;
}

BSONContentInt::BSONContentInt(const BSONContentInt& orig):
	BSONContent(INT_TYPE)
{
	this->_element = orig._element;
}

BSONContentInt* BSONContentInt::clone() const {
	return new BSONContentInt(*this);
}

BSONContentLong::BSONContentLong(__int64 element):
	BSONContent(LONG_TYPE)
{
	_element = element;
}

BSONContentLong::BSONContentLong(const BSONContentLong& orig):
	BSONContent(LONG_TYPE)
{
	this->_element = orig._element;
}

BSONContentLong* BSONContentLong::clone() const {
	return new BSONContentLong(*this);
}

BSONContentDouble::BSONContentDouble(double element):
	BSONContent(DOUBLE_TYPE)
{
	_element = element;
}

BSONContentDouble::BSONContentDouble(const BSONContentDouble& orig):
	BSONContent(DOUBLE_TYPE)
{
	this->_element = orig._element;
}

BSONContentDouble* BSONContentDouble::clone() const {
	return new BSONContentDouble(*this);
}

BSONContentString::BSONContentString(char* element, __int32 len):
	BSONContent(PTRCHAR_TYPE)
{
	_element = djondb::string(element, len);
}

BSONContentString::BSONContentString(const BSONContentString& orig):
	BSONContent(PTRCHAR_TYPE)
{
	this->_element = orig._element;
}

BSONContentString* BSONContentString::clone() const {
	return new BSONContentString(*this);
}

BSONContentBSON::BSONContentBSON(BSONObj* element):
	BSONContent(BSON_TYPE)
{
	_element = element;
}

BSONContentBSON::BSONContentBSON(const BSONContentBSON& orig):
	BSONContent(BSON_TYPE)
{
	this->_element = new BSONObj(*orig._element);
}

BSONContentBSON* BSONContentBSON::clone() const {
	return new BSONContentBSON(*this);
}

BSONContentBSONArray::BSONContentBSONArray(BSONArrayObj* element):
	BSONContent(BSONARRAY_TYPE)
{
	_element = element;
}

BSONContentBSONArray::BSONContentBSONArray(const BSONContentBSONArray& orig):
	BSONContent(BSONARRAY_TYPE)
{
	this->_element = new BSONArrayObj(*orig._element);
}

BSONContentBSONArray* BSONContentBSONArray::clone() const {
	return new BSONContentBSONArray(*this);
}

BSONContent::~BSONContent() {
}

BSONContentBoolean::~BSONContentBoolean() {
}

BSONContentInt::~BSONContentInt() {
}

BSONContentLong::~BSONContentLong() {
}

BSONContentDouble::~BSONContentDouble() {
}

BSONContentString::~BSONContentString() {
}

BSONContentBSON::~BSONContentBSON() {
	delete(_element);
}

BSONContentBSONArray::~BSONContentBSONArray() {
	delete(_element);
}

bool BSONContent::operator ==(const BSONContent& content) {
	throw BSONException("cannot compare BSONContents");
}

bool BSONContent::operator !=(const BSONContent& content) {
	throw BSONException("cannot compare BSONContents");
}

bool BSONContentBoolean::operator ==(const BSONContentBoolean& content) {
	bool i1 = _element;
	bool i2 = (BSONContentBoolean)content;
	return (i1 == i2);
}

bool BSONContentBoolean::operator !=(const BSONContentBoolean& content) {
	bool i1 = _element;
	bool i2 = (BSONContentBoolean)content;
	return (i1 != i2);
}

bool BSONContentInt::operator ==(const BSONContentInt& content) {
	__int32 i1 = _element;
	__int32 i2 = (BSONContentInt)content;
	return (i1 == i2);
}

bool BSONContentInt::operator !=(const BSONContentInt& content) {
	__int32 i1 = _element;
	__int32 i2 = (BSONContentInt)content;
	return (i1 != i2);
}

bool BSONContentLong::operator ==(const BSONContentLong& content) {
	__int64 i1 = _element;
	__int64 i2 = (BSONContentLong)content;
	return (i1 == i2);
}

bool BSONContentLong::operator !=(const BSONContentLong& content) {
	__int64 i1 = _element;
	__int64 i2 = (BSONContentLong)content;
	return (i1 != i2);
}

bool BSONContentDouble::operator ==(const BSONContentDouble& content) {
	double i1 = _element;
	double i2 = (BSONContentDouble)content;
	return (i1 == i2);
}

bool BSONContentDouble::operator !=(const BSONContentDouble& content) {
	double i1 = _element;
	double i2 = (BSONContentDouble)content;
	return (i1 != i2);
}

bool BSONContentString::operator ==(const BSONContentString& content) {
	djondb::string i1 = _element;
	djondb::string i2= content;
	return (i1 == i2);
}

bool BSONContentString::operator !=(const BSONContentString& content) {
	djondb::string i1 = _element;
	djondb::string i2= content;
	return (i1 != i2);
}

bool BSONContentBSON::operator ==(const BSONContentBSON& content) {
	BSONObj* i1 = _element;
	BSONObj* i2 = (BSONContentBSON)content;
	return (*i1 == *i2);
}

bool BSONContentBSON::operator !=(const BSONContentBSON& content) {
	BSONObj* i1 = _element;
	BSONObj* i2 = (BSONContentBSON)content;
	return (*i1 != *i2);
}

bool BSONContentBSONArray::operator ==(const BSONContentBSONArray& content) {
	// Never compare arrays, it's futile
	return false;
}

bool BSONContentBSONArray::operator !=(const BSONContentBSONArray& content) {
	// Never compare arrays, it's futile
	return true;
}


BSONContent::operator bool() const {
	throw BSONException("This operator should be overriden by the inherited class");
}

BSONContent::operator __int32() const {
	throw BSONException("This operator should be overriden by the inherited class");
}

BSONContentBoolean::operator bool() const {
	return _element; 
}

BSONContentInt::operator __int32() const {
	return _element; 
}

BSONContent::operator __int64() const {
	throw BSONException("This operator should be overriden by the inherited class");
}

BSONContentLong::operator __int64() const {
	return _element; 
}

BSONContent::operator double() const {
	throw BSONException("This operator should be overriden by the inherited class");
}

BSONContentDouble::operator double() const {
	return _element; 
}

BSONContent::operator djondb::string() const {
	throw BSONException("This operator should be overriden by the inherited class");
}

BSONContentString::operator djondb::string() const {
	return _element; 
}

BSONContent::operator BSONObj*() const {
	throw BSONException("This operator should be overriden by the inherited class");
}

BSONContentBSON::operator BSONObj*() const {
	return _element; 
}

BSONContent::operator BSONArrayObj*() const {
	throw BSONException("This operator should be overriden by the inherited class");
}

BSONContentBSONArray::operator BSONArrayObj*() const {
	return _element; 
}

