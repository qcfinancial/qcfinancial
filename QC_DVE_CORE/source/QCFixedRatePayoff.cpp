#include "QCFixedRatePayoff.h"


QCFixedRatePayoff::QCFixedRatePayoff(
	QCIntrstRtShrdPtr fixedRate,
	QCIntrstRtLgShrdPtr irLeg,
	QCZrCpnCrvShrdPtr discountCurve,
	QCDate valueDate) :
	QCInterestRatePayoff(fixedRate, irLeg, valueDate, discountCurve)
{}

void QCFixedRatePayoff::_setAllRates()
{
	for (int i = _currentPeriod; i < _irLeg->size(); ++i)
	{
		_allRates.push_back(_rate->getValue());
	}
}

QCFixedRatePayoff::~QCFixedRatePayoff()
{
}
