//
// Created by Alvaro Diaz on 16-07-16.
//

#include "QCActAct.h"
#include "QCDate.h"

double QCActAct::yf(const QCDate &firstDate, const QCDate &secondDate)
{
    int firstDay = firstDate.day();
    int firstMonth = firstDate.month();
    int firstYear = firstDate.year();

    int secondDay = secondDate.day();
    int secondMonth = secondDate.month();
    int secondYear = secondDate.year();

    QCDate aux1;
    if (secondMonth > firstMonth)
    {
        aux1 = firstDate.addMonths((secondYear - firstYear) * 12);
    }
    else if (secondMonth < firstMonth)
    {
        aux1 = firstDate.addMonths((secondYear - firstYear - 1) * 12);
    }
    else
    {
        if (firstDay <= secondDay)
        {
            aux1 = firstDate.addMonths((secondYear - firstYear) * 12);
        }
        else
        {
            aux1 = firstDate.addMonths((secondYear - firstYear - 1) * 12);
        }
    }

    int a_o1 = aux1.year() - firstYear;
    QCDate aux2 = aux1.addMonths(12);
    double a_o2 = (double)aux1.dayDiff(secondDate) / aux1.dayDiff(aux2);

    return double(a_o1) + a_o2;
}

double QCActAct::yf(long days)
{
	return days / 365.0;
}

long QCActAct::countDays(const QCDate &firstDate, const QCDate &secondDate)
{
    return firstDate.dayDiff(secondDate);
}

std::string QCActAct::description()
{
	return "ActAct";
}


