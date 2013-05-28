/*
 * =====================================================================================
 *
 *       Filename:  DateTime.cpp
 *
 *    Description:  This class is used to represent date time data type, it has features
 *                  like comparison methods (>, <, ==, <=, etc) and substraction and 
 *                  addition
 *
 *        Version:  1.0
 *        Created:  07/07/2010 1:34 PM
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

#include "datetime.h"
#include <string.h>
#include <stdlib.h>
#include <sstream>
#include <stdio.h>
#include <ctime>

DateTime::DateTime(string dateTime) {
	loadDate(dateTime);
}

DateTime::DateTime(time_t time) {
	struct tm* t = localtime(&time);
	_year = t->tm_year + 1900;
	_month = t->tm_mon + 1;
	_day = t->tm_mday;
	_hour = t->tm_hour;
	_min = t->tm_min;
	_secs = t->tm_sec;
}

DateTime::DateTime(double value) {
	_year = value / 10000;
	value -= (_year * 10000);
	_month = value / 100;
	value -= (_month * 100);
	_day = value;
	value -= _day;
	_hour = value * 100;
	_min = value * 10000;
	_secs = value * 1000000;
}

DateTime::DateTime(int year, int month, int day, int hour, int min, int secs) {
	_year = year;
	_month = month;
	_day = day;
	_hour = hour;
	_min = min;
	_secs = secs;
}

void toInt(char* chr, int* dest) {
	// Non empty string
	if (*chr != '\0') {
		*dest = atoi(chr);
	}
}

void DateTime::loadDate(string dateTime) {
	_year = 0;
	_month = 0;
	_day = 0;
	_hour = 0;
	_min = 0;
	_secs = 0;
	char year[5];
	memset(year, 0, 5);
	char month[3];
	memset(month, 0, 3);
	char day[3];
	memset(day, 0, 3);
	char hour[3];
	memset(hour, 0, 3);
	char min[3];
	memset(min, 0, 3);
	char secs[3];
	memset(secs, 0, 3);

	const char* cdate = dateTime.c_str();
	memcpy(year, cdate, 4);
	memcpy(month, cdate + 4, 2);
	memcpy(day, cdate + 6, 2);
	memcpy(hour, cdate + 9, 2);
	memcpy(min, cdate + 11, 2);
	memcpy(secs, cdate + 13, 2);
	toInt(year, &_year);
	toInt(month, &_month);
	toInt(day, &_day);
	toInt(hour, &_hour);
	toInt(min, &_min);
	toInt(secs, &_secs);
}

DateTime::DateTime() {
}

void DateTime::setSecs(int _secs) {
	this->_secs = _secs;
}

int DateTime::getSecs() const {
	return _secs;
}

void DateTime::setMin(int _min) {
	this->_min = _min;
}

int DateTime::getMin() const {
	return _min;
}

void DateTime::setHour(int _hour) {
	this->_hour = _hour;
}

int DateTime::getHour() const {
	return _hour;
}

void DateTime::setDay(int _day) {
	this->_day = _day;
}

int DateTime::getDay() const {
	return _day;
}

void DateTime::setMonth(int _month) {
	this->_month = _month;
}

int DateTime::getMonth() const {
	return _month;
}

void DateTime::setYear(int _year) {
	this->_year = _year;
}

int DateTime::getYear() const {
	return _year;
}

DateTime::DateTime(const DateTime& orig) {
	_year = orig._year;
	_month = orig._month;
	_day = orig._day;
	_hour = orig._hour;
	_min = orig._min;
	_secs = orig._secs;
}

DateTime::~DateTime() {
}

char* DateTime::toChar() const {
	char* buffer = (char*)malloc(16);
	memset(buffer, 0, 16);
	sprintf(buffer, "%04d%02d%02d.%02d%02d%02d", _year, _month, _day, _hour, _min, _secs);

	return buffer;
}

double DateTime::toDouble() const {
	double date = (_year * 10000) + (_month * 100) + _day + ((double)_hour / 100) + ((double)_min/10000) + ((double)_secs/1000000);

	return date;
}

bool DateTime::operator >(const DateTime& dateTime) const {
	return toDouble() > dateTime.toDouble();
}

bool DateTime::operator <(const DateTime& dateTime) const {
	return toDouble() < dateTime.toDouble();
}

bool DateTime::operator >=(const DateTime& dateTime) const {
	return toDouble() >= dateTime.toDouble();
}

bool DateTime::operator <=(const DateTime& dateTime) const {
	return toDouble() <= dateTime.toDouble();
}

bool DateTime::operator ==(const DateTime& dateTime) const {
	DateTime test = dateTime;
	DateTime thisDate = *this;
	if ((test.getHour() == 0) && (test.getMin() == 0) && (test.getSecs() == 0)) {
		thisDate.setHour(0);
		thisDate.setMin(0);
		thisDate.setSecs(0);
	}
	return test.toDouble() == thisDate.toDouble();
}

struct tm* DateTime::toTimeStruct() const {
	DateTime date = *this; // Executes the copy constructor

	time_t rawtime;
	time(&rawtime);

	struct tm* time = localtime(&rawtime);
	time->tm_year = date._year - 1900;
	time->tm_mon = date._month - 1;
	time->tm_mday = date._day;

	time->tm_hour = date._hour;
	time->tm_min = date._min;
	time->tm_sec = date._secs;

	mktime(time);

	struct tm* result = (struct tm*)malloc(sizeof(struct tm));
	memcpy(result, time, sizeof(struct tm));

	return result;
}

long DateTime::operator -(const DateTime& dateTimeRight) const {
	DateTime left = *this; // Copy this
	DateTime right = dateTimeRight; // copy the argument
	struct tm* tleft = left.toTimeStruct();
	struct tm* tright = right.toTimeStruct();

	int secs = difftime(mktime(tright), mktime(tleft));

	return secs;
}

DateTime DateTime::today(bool includeTime) {
	time_t rawtoday = time(NULL);
	struct tm* tmToday = localtime(&rawtoday);

	DateTime result(tmToday->tm_year + 1900, tmToday->tm_mon + 1, tmToday->tm_mday);
	if (includeTime) {
		result.setHour(tmToday->tm_hour);
		result.setMin(tmToday->tm_min);
		result.setSecs(tmToday->tm_sec);
	}
	return result;
}

DateTime DateTime::addDays(int days) const {
	struct tm* t = toTimeStruct();
	t->tm_mday += days;
	mktime(t);

	DateTime dtNew(mktime(t));

	return dtNew;
}

int DateTime::daysTo(const DateTime& dt) const {
	DateTime current = *this;
	long secs = dt - current;

	int days = (double)secs / 60.0 / 60.0 / 24.0;

	return days;
}

int DateTime::dayOfTheWeek() const {
	struct tm* t = toTimeStruct();

	return t->tm_wday;
}

DTime DateTime::dtime() const {
	int hour = getHour();
	int minute = getMin();
	int secs = getSecs();
	return DTime(hour, minute, secs);
}
