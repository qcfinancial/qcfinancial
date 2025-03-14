//
// Created by Alvaro Diaz on 15-07-16.
//

#include <algorithm>
#include <utility>
#include "time/QCBusinessCalendar.h"

QCBusinessCalendar::QCBusinessCalendar(
	const QCDate& startDate,
	int length) :
	_startDate(startDate),
	_length(length)
{
    // insertNewYear();
    _firstDayOfWeekend = QCDate::QCWeekDay ::qcSaturday;
    _secondDayOfWeekEnd = QCDate::QCWeekDay ::qcSunday;
};

void QCBusinessCalendar::insertNewYear() {
    for (int i = 0; i < _length + 1; ++i) {
        _holidays.insert(QCDate(1, 1, _startDate.year() + i));
    }
}
void QCBusinessCalendar::addHoliday(const QCDate& holiday)
{
    _holidays.insert(holiday);
    // sortHolidays();
}

QCBusinessCalendar QCBusinessCalendar::operator+(QCBusinessCalendar const& cal)
{
    auto min_date = _startDate;
    if (cal.getStartDate() < _startDate) {
        min_date = _startDate;
    }

    auto max_length = _length;
    if (cal.getLength() > max_length) {
        max_length = cal.getLength();
    }

    auto result = QCBusinessCalendar(min_date, max_length);

    for (const auto& fecha: _holidays) {
        result.addHoliday(fecha);
    }

    for (const auto& fecha: cal.getHolidaysAsSet()) {
        result.addHoliday(fecha);
    }

    return result;
}

QCDate QCBusinessCalendar::nextBusinessDay(const QCDate& fecha)
{
    QCDate::QCWeekDay d = fecha.weekDay();
    long serial = fecha.excelSerial();

    if (d == _secondDayOfWeekEnd)
    {
        serial++;
    }

    if (d == _firstDayOfWeekend)
    {
        serial += 2;
    }

    QCDate fechaOut{serial};
    while (binary_search(_holidays.begin(), _holidays.end(), fechaOut))
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

    if (d == _secondDayOfWeekEnd)
    {
        serial -= 2;
    }

    if (d == _firstDayOfWeekend)
    {
        serial -= 1;
    }

    QCDate fechaOut{serial};
    while (binary_search(_holidays.begin(), _holidays.end(), fechaOut))
    {
        fechaOut.setDateFromExcelSerial(--serial);
        if (fechaOut.weekDay() == _secondDayOfWeekEnd)
            fechaOut.setDateFromExcelSerial(serial - 2);
        serial = fechaOut.excelSerial();
    }
    return fechaOut;
}

QCDate QCBusinessCalendar::shift(const QCDate& fecha, int nDays)
{
    if (nDays == 0) return fecha;
	QCDate result = fecha;
	if (nDays > 0)
	{
		for (int i = 1; i <= nDays; ++i)
		{
			result = nextBusinessDay(result.addDays(1));
		}
	}
	else
	{
		nDays = -1 * nDays;
		for (int i = 1; i <= nDays; ++i)
		{
			result = previousBusinessDay(result.addDays(-1));
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

std::vector<QCDate> QCBusinessCalendar::getHolidays()
{
    std::vector<QCDate> _vecHolidays(_holidays.begin(), _holidays.end());
	return _vecHolidays;
}

void QCBusinessCalendar::setHolidays(std::set<QCDate> holidays) {
    _holidays = std::move(holidays);
}

QCDate QCBusinessCalendar::getStartDate() const {
    return _startDate;
}

int QCBusinessCalendar::getLength() const {
    return _length;
}

std::set<QCDate> QCBusinessCalendar::getHolidaysAsSet() const {
    return _holidays;
}



