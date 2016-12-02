#include "QCInterestRateLeg.h"


QCInterestRateLeg::QCInterestRateLeg(QCIntrstRtPrdsShrdPntr periods,
	int lastPeriod) :_periods(periods), _lastPeriod(lastPeriod)
{}

int QCInterestRateLeg::size()
{
	return _lastPeriod + 1;
}

QCInterestRatePeriod QCInterestRateLeg::getPeriodAt(unsigned int n)
{
	if (n > _periods->size() - 1)
	{
		return _periods->at(_periods->size() - 1);
	}
	else
	{
		return _periods->at(n);
	}
	
}

QCInterestRateLeg::~QCInterestRateLeg()
{}
