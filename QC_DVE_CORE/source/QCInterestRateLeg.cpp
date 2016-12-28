#include "QCInterestRateLeg.h"


QCInterestRateLeg::QCInterestRateLeg(QCInterestRatePeriods periods,
	unsigned int lastPeriod) :_periods(periods), _lastPeriod(lastPeriod)
{}

void QCInterestRateLeg::operator=(const QCInterestRateLeg& otherLeg)
{
	_periods = otherLeg.periods();
	_lastPeriod = otherLeg.lastPeriod();
}

QCInterestRateLeg::QCInterestRatePeriods QCInterestRateLeg::periods() const
{
	return _periods;
}

unsigned int QCInterestRateLeg::lastPeriod() const
{
	return _lastPeriod;
}

int QCInterestRateLeg::size()
{
	return _lastPeriod + 1;
}

QCInterestRateLeg::QCInterestRatePeriod QCInterestRateLeg::getPeriodAt(unsigned int n)
{
	if (n > _periods.size() - 1)
	{
		return _periods.at(_periods.size() - 1);
	}
	else
	{
		return _periods.at(n);
	}
	
}

QCInterestRateLeg::~QCInterestRateLeg()
{}
