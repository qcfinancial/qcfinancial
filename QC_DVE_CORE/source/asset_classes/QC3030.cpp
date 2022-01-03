//
// Created by Alvaro Diaz on 21-03-2019.
//

#include "asset_classes/QC3030.h"
#include "time/QCDate.h"

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
    return QCYearFraction::countDays30360(firstDate, secondDate);
}

std::string QC3030::description()
{
	return "3030";
}