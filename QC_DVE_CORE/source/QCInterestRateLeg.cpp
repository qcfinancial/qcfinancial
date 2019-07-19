#include "QCInterestRateLeg.h"


QCInterestRateLeg::QCInterestRateLeg(QCInterestRatePeriods periods,
	size_t lastPeriod) :_periods(periods), _lastPeriod(lastPeriod)
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

size_t QCInterestRateLeg::lastPeriod() const
{
	return _lastPeriod;
}

size_t QCInterestRateLeg::size()
{
	return _lastPeriod + 1;
}

QCInterestRateLeg::QCInterestRatePeriod QCInterestRateLeg::getPeriodAt(size_t n)
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
