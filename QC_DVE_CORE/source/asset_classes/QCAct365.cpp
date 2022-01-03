//
// Created by Alvaro Diaz on 16-07-16.
//

#include "asset_classes/QCAct365.h"
#include "time/QCDate.h"

double QCAct365::yf(const QCDate &firstDate, const QCDate &secondDate)
{
    auto days = countDays(firstDate, secondDate);
    return days / _basis;
}

double QCAct365::yf(long days)
{
	return days / _basis;
}

long QCAct365::countDays(const QCDate &firstDate, const QCDate &secondDate)
{
    return QCYearFraction::countDaysAct(firstDate, secondDate);
}

std::string QCAct365::description()
{
	return "Act365";
}