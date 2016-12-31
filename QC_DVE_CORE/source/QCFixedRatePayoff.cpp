#include "QCFixedRatePayoff.h"


QCFixedRatePayoff::QCFixedRatePayoff(
	QCIntrstRtShrdPtr fixedRate,
	shared_ptr<QCInterestRateLeg> irLeg,
	QCIntRtCrvShrdPtr discountCurve,
	QCDate valueDate,
	QCTimeSeriesShrdPtr fixingData) :
	QCInterestRatePayoff(fixedRate, irLeg, valueDate, discountCurve, fixingData)
{
	_setAllRates();
}

void QCFixedRatePayoff::_setAllRates()
{
	for (int i = 0; i < _irLeg->size(); ++i)
	{
		_allRates.push_back(_rate->getValue());
	}
}

QCFixedRatePayoff::~QCFixedRatePayoff()
{
}
