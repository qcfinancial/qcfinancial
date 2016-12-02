//
// Created by Alvaro Diaz on 16-07-16.
//

#include "QCAct365.h"
#include "QCDate.h"

double QCAct365::yf(const QCDate &firstDate, const QCDate &secondDate)
{
    long days = firstDate.dayDiff(secondDate);
    return days / _basis;
}

double QCAct365::yf(long days)
{
	return days / _basis;
}

long QCAct365::countDays(const QCDate &firstDate, const QCDate &secondDate)
{
    return firstDate.dayDiff(secondDate);
}
