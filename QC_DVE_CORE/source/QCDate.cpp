//
// Created by Alvaro Diaz on 14-07-16.
//

#include "QCDate.h"
#include <sstream>
#include <exception>
#include <algorithm>

//
//  QCDate.cpp
//  QC-FXOptions
//
//  Created by Alvaro Diaz on 10-12-12.
//  Copyright (c) 2012 Alvaro Diaz. All rights reserved.
//


QCDate::QCDate() : _day(12), _month(1), _year(1969)
{
}

QCDate::QCDate(const QCDate& otherDate)
{
    _day = otherDate.day();
    _month = otherDate.month();
    _year = otherDate.year();
}

QCDate::QCDate(int d, int m, int y)
{
    if (_validate(d, m, y))
    {
        _day = d;
        _month = m;
        _year = y;
        return;
    }
    else
    {
        throw invalid_argument("Invalid day, month and year");
    }
}

QCDate::QCDate(long excelSerial)
{
    setDateFromExcelSerial(excelSerial);
}

void QCDate::setDateFromExcelSerial(long excelSerial)
{
    // Excel/Lotus 123 have a bug with 29-02-1900. 1900 is not a
    // leap year, but Excel/Lotus 123 think it is...
    if (excelSerial == 60)
    {
        _day = 29;
        _month = 2;
        _year = 1900;

        return;
    }
    else if (excelSerial < 60)
    {
        // Because of the 29-02-1900 bug, any serial date
        // under 60 is one off... Compensate.
        excelSerial++;
    }

    // Modified Julian to DMY calculation with an addition of 2415019
    long l = excelSerial + 68569 + 2415019;
    int n = int(( 4 * l ) / 146097);
    l = l - ( 146097 * n + 3 ) / 4;
    int i = int(( 4000 * ( l + 1 ) ) / 1461001);
    l = l - ( 1461 * i ) / 4 + 31;
    int j = int(( 80 * l ) / 2447);
    _day = l - int(( 2447 * j ) / 80);
    l = int(j / 11);
    _month = j + 2 - ( 12 * l );
    _year = 100 * ( n - 49 ) + i + l;

    return;
}

bool QCDate::_validate(int d, int m, int y)
{
    if ((d < 1) || (d > 31))
    {
        return false;
    }

    if ((m < 1) || (m > 12))
    {
        return false;
    }

    if (y < 1)
    {
        return false;
    }

    if ((d == 29) && (m == 2))
    {
        if (((y % 4 == 0) && (y % 100 != 0)) || (y % 400 == 0))
        {
            printf("%d is a leap year", y);
            cout << endl;
            return true;
        }
        else
        {
            return false;
        }
    }

    if (m == 2 && (d == 30 || d == 31))
    {
        throw invalid_argument("Invalid day for month = 2");
    }
    bool dayMonth = (d == 31) && ((m == 2) || (m == 4) || (m == 6) || (m == 9) || (m == 11));
    if (dayMonth)
    {
        return false;
    }

    return true;
}

void QCDate::setDay(int d)
{
    if (_validate(d, _month, _year))
    {
        _day = d;
        return;
    }
    else
    {
        throw invalid_argument("Invalid day");
    }
}

void QCDate::setMonth(int m)
{
    if (_validate(_day, m, _year))
    {
        _month = m;
        return;
    }
    else
    {
        throw invalid_argument("Invalid month");
    }
}

void QCDate::setYear(int y)
{
    if (_validate(_day, _month, y))
    {
        _year = y;
        return;
    }
    else
    {
        throw invalid_argument("Invalid year");
    }
}

int QCDate::day() const
{
    return _day;
}

int QCDate::month() const
{
    return _month;
}

int QCDate::year() const
{
    return _year;
}

long QCDate::excelSerial() const
{
    // Excel/Lotus 123 have a bug with 29-02-1900. 1900 is not a
    // leap year, but Excel/Lotus 123 think it is...
    if ((_day == 29)&&(_month == 02)&&(_year==1900))
        return 60;

    // DMY to Modified Julian calculate with an extra substraction of 2415019.
    long nSerialDate =
            int(( 1461 * ( _year + 4800 + int(( _month - 14 ) / 12) ) ) / 4) +
            int(( 367 * ( _month - 2 - 12 * ( ( _month - 14 ) / 12 ) ) ) / 12) -
            int(( 3 * ( int(( _year + 4900 + int(( _month - 14 ) / 12) ) / 100) ) ) / 4) +
            _day - 2415019 - 32075;

    if (nSerialDate <= 60)
    {
        //Aqui hay que poner <=60
        // Because of the 29-02-1900 bug, any serial date
        // under 60 is one off... Compensate.
        nSerialDate--;
    }

    return nSerialDate;
}

QCDate::QCWeekDay QCDate::weekDay() const
{
    static int t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
    int auxYear = _year - (_month < 3);
    //_year -= _month < 3;
    return QCWeekDay((auxYear + auxYear/4 - auxYear/100 + auxYear/400 + t[_month-1] + _day) % 7);
}

std::string QCDate::description()
{
    std::stringstream ss;
    std::string auxDay = "";
    std::string auxMonth = "";
    if (_day < 10) { auxDay = "0"; }
    if (_month < 10) { auxMonth = "0"; }
    ss << auxDay << _day << "-" << auxMonth << _month << "-" << _year;
    return ss.str();
}

QCDate QCDate::businessDay(vector<QCDate>& calendar, QCDate::QCBusDayAdjRules rule) const
{
	//Solo esta implementado el caso FOLLOW
	QCDate result{ _day, _month, _year };
	switch (rule)
	{
	case QCDate::qcNo:
		break;

	case QCDate::qcFollow:
		if (result.weekDay() == 6)
		{
			result = result.addDays(2);
		}
		if (result.weekDay() == 0)
		{
			result = result.addDays(1);
		}
		while (binary_search(calendar.begin(), calendar.end(), result))
		{
			result = result.addDays(1);
		}
		break;

	case QCDate::qcModFollow:
		if (result.weekDay() == 6)
		{
			int month = result.month();
			result = result.addDays(2);
			if (result.month() != month)
				result = result.addDays(-3);
		}
		if (result.weekDay() == 0)
		{
			int month = result.month();
			result = result.addDays(1);
			if (result.month() != month)
				result = result.addDays(-2);
		}
		while (binary_search(calendar.begin(), calendar.end(), result))
		{
			result = result.addDays(-1);
		}
		break;

	case QCDate::qcPrev:
		break;

	case QCDate::qcModPrev:
		break;

	default:
		break;
	}

	return result;
}

QCDate QCDate::shift(vector<QCDate>& calendar, int nDays) const
{
	QCDate result{ _day, _month, _year };
	if (nDays > 0)
	{
		for (int i = 1; i < nDays + 1; ++i)
		{
			result = result.addDays(1).businessDay(calendar, QCDate::qcFollow);
		}
	}
	else
	{
		for (int i = 1; i < -nDays + 1; ++i)
		{
			result = result.addDays(-1).businessDay(calendar, QCDate::qcPrev);
		}
	}
	return result;
}

QCDate QCDate::addMonths(int nMonths) const
{
    //Equivalent to Excel Function EDATE()
    //adds nMonths (number of months) to this for positive/negative values of nMonths

    int dm[13];

    dm[1] = 31;
    dm[2] = 28;
    dm[3] = 31;
    dm[4] = 30;
    dm[5] = 31;
    dm[6] = 30;
    dm[7] = 31;
    dm[8] = 31;
    dm[9] = 30;
    dm[10] = 31;
    dm[11] = 30;
    dm[12] = 31;

    bool eom = false;

    int d = _day;
    int m = _month;
    int y = _year;

    if ((d == dm[m]) && (m != 2))
    {
        eom = true;  //Detect if the date corresponds to the end of a month
    }

	//Aqui hay que arreglar la addMonths
    nMonths += m;

	auto aux = (int)floor(nMonths / 12.0);
    m = nMonths - 12 * aux;

    if (m == 0)
        m = 12;

	aux = (int)floor((nMonths - 1) / 12.0);
    y = y + aux;

    if (y < 0)
        y = 1900;

    if (d > dm[m])
        eom = true;

    if (eom)
    {
        d = dm[m];  //if the date correspond to the end of a month,
                            // the resulting date is forced to be the end of a month
        if (((y / 4) == (y / 4.0)) && (m == 2))
            d = 29;
    }

    return QCDate {d, m, y};
}

long QCDate::dayDiff(const QCDate& otherDate) const
{
    return otherDate.excelSerial() - this->excelSerial();
}

tuple<unsigned long, int> QCDate::monthDiffDayRemainder(const QCDate& otherDate,
	vector<QCDate>& calendar, QCDate::QCBusDayAdjRules rule) const
{
	QCDate lastDate{ _day, _month, _year };
	QCDate nextDate{ _day, _month, _year };
	unsigned long counter{ 0 };

	while (true)
	{
		nextDate = this->addMonths(counter + 1).businessDay(calendar, rule);
		if (nextDate <= otherDate)
		{
			++counter;
			lastDate = nextDate;
		}
		else
		{
			break;
		}
	}

	return make_tuple(counter, (int)lastDate.dayDiff(otherDate));

}

QCDate QCDate::addDays(long nDays) const
{
    long newSerial = this->excelSerial() + nDays;
    return QCDate {newSerial};
}

bool QCDate::operator<(const QCDate& rhs) const
{
    return this->excelSerial() < rhs.excelSerial();
}

bool QCDate::operator>(const QCDate& rhs) const
{
    return this->excelSerial() > rhs.excelSerial();
}

bool QCDate::operator<=(const QCDate& rhs) const
{
    return this->excelSerial() <= rhs.excelSerial();
}

bool QCDate::operator>=(const QCDate& rhs) const
{
    return this->excelSerial() >= rhs.excelSerial();
}

bool QCDate::operator==(const QCDate& rhs) const
{
    return this->excelSerial() == rhs.excelSerial();
}

bool QCDate::operator!=(const QCDate& rhs) const
{
    return this->excelSerial() != rhs.excelSerial();
}

void QCDate::operator=(const QCDate& otherDate)
{
    _day = otherDate.day();
    _month = otherDate.month();
    _year = otherDate.year();
}

void QCDate::setDayMonthYear(int d, int m, int y)
{
    if (_validate(d, m, y))
    {
        _day = d;
        _month = m;
        _year = y;
        return;
    }
    else
    {
        throw invalid_argument("Invalid day, month and year");
    };
}

