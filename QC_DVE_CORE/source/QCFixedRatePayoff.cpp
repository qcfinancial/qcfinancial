#include <exception>
#include "QCFixedRatePayoff.h"


QCFixedRatePayoff::QCFixedRatePayoff(
	QCIntrstRtShrdPtr fixedRate,
	shared_ptr<QCInterestRateLeg> irLeg,
	QCIntRtCrvShrdPtr discountCurve,
	QCDate valueDate,
	QCTimeSeriesShrdPtr fixingData) :
	QCInterestRatePayoff(fixedRate, irLeg, valueDate, discountCurve, discountCurve, fixingData)
{
	_setAllRates();
}

void QCFixedRatePayoff::_setAllRates()
{
	size_t numPeriods = _irLeg->size();

	_allRates.resize(numPeriods);
	double rate{ _rate->getValue() };
	for (unsigned int i = 0; i <numPeriods; ++i)
	{
		_allRates.at(i) = rate;
	}
	unsigned long long curveLength{ _discountCurve->getLength() };
	vector<double> temp(curveLength, 0.0);
	_allRatesDerivatives.resize(numPeriods);
	for (unsigned int i = 0; i < numPeriods; ++i)
	{
		_allRatesDerivatives.at(i) = temp;
	}
}

bool QCFixedRatePayoff::operator<(const QCFixedRatePayoff& rhs)
{
	auto lastPeriod = _irLeg->lastPeriod();
	QCDate lastDate = get<QCInterestRateLeg::intRtPrdElmntEndDate>(this->getPeriodAt(lastPeriod));
	lastPeriod = rhs.getLastPeriodIndex();
	QCDate lastDateRhs = get<QCInterestRateLeg::intRtPrdElmntEndDate>(rhs.getPeriodAt(lastPeriod));
	return lastDate < lastDateRhs;
}

bool QCFixedRatePayoff::lessThan(shared_ptr<QCFixedRatePayoff> lhs, shared_ptr<QCFixedRatePayoff> rhs)
{
	return *lhs < *rhs;
}

double QCFixedRatePayoff::getRateValue()
{
	return _rate->getValue();
}

QCFixedRatePayoff::~QCFixedRatePayoff()
{
}
