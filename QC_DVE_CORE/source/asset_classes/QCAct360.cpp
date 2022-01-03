//
// Created by Alvaro Diaz on 16-07-16.
//

#include "asset_classes/QCAct360.h"
#include "time/QCDate.h"

double QCAct360::yf(const QCDate &firstDate, const QCDate &secondDate)
{
    auto days = countDays(firstDate, secondDate);
    return days / _basis;
}

double QCAct360::yf(long days)
{
	return days / _basis;
}

long QCAct360::countDays(const QCDate &firstDate, const QCDate &secondDate)
{
    return QCYearFraction::countDaysAct(firstDate, secondDate);
}

std::string QCAct360::description()
{
	return "Act360";
}

