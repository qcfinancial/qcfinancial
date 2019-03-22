//
// Created by Alvaro Diaz on 21-03-2019.
//

#include "QC3030.h"
#include "QCDate.h"

double QC3030::yf(const QCDate &firstDate, const QCDate &secondDate)
{
	return this->countDays(firstDate, secondDate) / _basis;
}

double QC3030::yf(long days)
{
	return days / _basis;
}

long QC3030::countDays(const QCDate &firstDate, const QCDate &secondDate)
{
	int firstDay = firstDate.day();
	int firstMonth = firstDate.month();
	int firstYear = firstDate.year();

	int secondDay = secondDate.day();
	int secondMonth = secondDate.month();
	int secondYear = secondDate.year();

	if (firstDay == 31)
		firstDay = 30;

	if (secondDay == 31 && firstDay == 30)
		secondDay = 30;

	long result = (secondDay - firstDay) + 30 * (secondMonth - firstMonth)
		+ 360 * (secondYear - firstYear);
	return result;
}

std::string QC3030::description()
{
	return "3030";
}