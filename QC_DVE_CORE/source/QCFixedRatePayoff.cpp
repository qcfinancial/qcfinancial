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
	unsigned int numPeriods = _irLeg->size();
	_allRates.resize(numPeriods);
	double rate{ _rate->getValue() };
	for (unsigned int i = 0; i <numPeriods; ++i)
	{
		_allRates.at(i) = rate;
	}
	long curveLength{ _discountCurve->getLength() };
	vector<double> temp(curveLength, 0.0);
	_allRatesDerivatives.resize(numPeriods);
	for (unsigned int i = 0; i < numPeriods; ++i)
	{
		_allRatesDerivatives.at(i) = temp;
	}
}

QCFixedRatePayoff::~QCFixedRatePayoff()
{
}
