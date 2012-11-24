/*
 * File:   DateTime.h
 * Author: cross
 *
 * Created on July 7, 2010, 1:34 PM
 */

#ifndef _DATETIME_H
#define	_DATETIME_H

#include "dtime.h"

#include <string>
#include <ctime>


using namespace std;

class DateTime {
public:
    DateTime();
    DateTime(int year, int month, int day, int hour = 0, int min = 0, int secs = 0);
    DateTime(time_t time);
    DateTime(string dateTime);
    DateTime(double value);
    DateTime(const DateTime& orig);
    virtual ~DateTime();

    static DateTime today(bool includeTime = false);
    DateTime addDays(int days) const;
    int daysTo(const DateTime& dt) const;
    int dayOfTheWeek() const;

    char* toChar() const;
    double toDouble() const;
    void setSecs(int _secs);
    int getSecs() const;
    void setMin(int _min);
    int getMin() const;
    void setHour(int _hour);
    int getHour() const;
    void setDay(int _day);
    int getDay() const;
    void setMonth(int _month);
    int getMonth() const;
    void setYear(int _year);
    int getYear() const;
    DTime dtime() const;

    struct tm* toTimeStruct() const;

    bool operator >(const DateTime& dateTime) const;
    bool operator <(const DateTime& dateTime) const;
    bool operator >=(const DateTime& dateTime) const;
    bool operator <=(const DateTime& dateTime) const;
    long operator -(const DateTime& dateTimeRight) const;
    bool operator ==(const DateTime& dateTime) const;

private:

    int _year;
    int _month;
    int _day;
    int _hour;
    int _min;
    int _secs;

    void loadDate(string dateTime);
};

#endif	/* _DATETIME_H */

