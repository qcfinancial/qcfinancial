//
// Created by Alvaro Diaz on 15-07-16.
//

#include <algorithm>
#include "QCBusinessCalendar.h"

QCBusinessCalendar::QCBusinessCalendar(
	const QCDate& startDate,
	int length) :
	_startDate(startDate),
	_length(length)
{
    insertNewYear();
    _firstDayOfWeekend = QCDate::QCWeekDay ::qcSaturday;
    _secondDayOfweekEnd = QCDate::QCWeekDay ::qcSunday;
};

void QCBusinessCalendar::insertNewYear()
{
    for (int i = 0; i < _length + 1; ++i)
    {
        _holydays.push_back(QCDate(1, 1, _startDate.year() + i));
    }

    sortHolydays();
}

void QCBusinessCalendar::addHolyday(const QCDate& holyday)
{
    _holydays.push_back(holyday);
    sortHolydays();
}

void QCBusinessCalendar::sortHolydays()
{
    sort(_holydays.begin(), _holydays.end());
}

QCDate QCBusinessCalendar::nextBusinessDay(const QCDate& fecha)
{
    QCDate::QCWeekDay d = fecha.weekDay();
    long serial = fecha.excelSerial();

    if (d == _secondDayOfweekEnd)
    {
        serial++;
    }

    if (d == _firstDayOfWeekend)
    {
        serial += 2;
    }

    QCDate fechaOut{serial};
    while (binary_search(_holydays.begin(), _holydays.end(), fechaOut))
    {
        fechaOut.setDateFromExcelSerial(++serial);
        if (fechaOut.weekDay() == _firstDayOfWeekend)
            fechaOut.setDateFromExcelSerial(serial + 2);
        serial = fechaOut.excelSerial();
    }
    return fechaOut;
}


QCDate QCBusinessCalendar::previousBusinessDay(const QCDate& fecha)
{
    QCDate::QCWeekDay d = fecha.weekDay();
    long serial = fecha.excelSerial();

    if (d == _secondDayOfweekEnd)
    {
        serial -= 2;
    }

    if (d == _firstDayOfWeekend)
    {
        serial -= 1;
    }

    QCDate fechaOut{serial};
    while (binary_search(_holydays.begin(), _holydays.end(), fechaOut))
    {
        fechaOut.setDateFromExcelSerial(--serial);
        if (fechaOut.weekDay() == _secondDayOfweekEnd)
            fechaOut.setDateFromExcelSerial(serial - 2);
        serial = fechaOut.excelSerial();
    }
    return fechaOut;
}


QCDate QCBusinessCalendar::shift(const QCDate& fecha, int nDays)
{
    if (nDays == 0) return fecha;
    long serial = fecha.excelSerial();
    QCDate result;

    if (nDays > 0)
    {
        for (int i = 1; i < nDays + 1; ++i)
        {
            result.setDateFromExcelSerial(++serial);
            result.setDateFromExcelSerial((this->nextBusinessDay(result)).excelSerial());
        }
    }
    else
    {
        for (int i = 0; i > nDays; --i)
        {
            result.setDateFromExcelSerial(--serial);
            result.setDateFromExcelSerial((this->nextBusinessDay(result)).excelSerial());
        }
    }

    return result;
}

QCDate QCBusinessCalendar::modNextBusinessDay(const QCDate &fecha)
{
    int m1 = fecha.month();
    QCDate result = this->nextBusinessDay(fecha);
    int m2 = result.month();

    if (m1 != m2)
    {
        result = this->previousBusinessDay(fecha);
    }

    return result;
}

DateList QCBusinessCalendar::getHolidays()
{
	return _holydays;
}



