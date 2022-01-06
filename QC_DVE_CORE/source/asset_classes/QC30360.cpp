//
// Created by Alvaro Diaz on 16-07-16.
//

#include "asset_classes/QC30360.h"
#include "time/QCDate.h"

double QC30360::yf(const QCDate &firstDate, const QCDate &secondDate)
{
    return this->countDays(firstDate, secondDate) / _basis;
}

double QC30360::yf(long days)
{
	return days / _basis;
}

long QC30360::countDays(const QCDate &firstDate, const QCDate &secondDate)
{
    return QCYearFraction::countDays30360(firstDate, secondDate);
}

std::string QC30360::description()
{
	return "30360";
}