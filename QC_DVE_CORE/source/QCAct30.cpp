//
// Created by Alvaro Diaz on 16-07-16.
//

#include "QCAct30.h"
#include "QCDate.h"

double QCAct30::yf(const QCDate &firstDate, const QCDate &secondDate)
{
	long days = firstDate.dayDiff(secondDate);
	return days / _basis;
}

double QCAct30::yf(long days)
{
	return days / _basis;
}

long QCAct30::countDays(const QCDate &firstDate, const QCDate &secondDate)
{
	return firstDate.dayDiff(secondDate);
}

std::string QCAct30::description()
{
	return "Act30";
}

