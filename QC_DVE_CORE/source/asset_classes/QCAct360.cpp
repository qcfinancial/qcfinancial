//
// Created by Alvaro Diaz on 16-07-16.
//

#include "asset_classes/QCAct360.h"
#include "time/QCDate.h"

double QCAct360::yf(const QCDate &firstDate, const QCDate &secondDate)
{
    long days = firstDate.dayDiff(secondDate);
    return days / _basis;
}

double QCAct360::yf(long days)
{
	return days / _basis;
}

long QCAct360::countDays(const QCDate &firstDate, const QCDate &secondDate)
{
    return firstDate.dayDiff(secondDate);
}

std::string QCAct360::description()
{
	return "Act360";
}

