#include "QCTimeDepositPayoff.h"

QCTimeDepositPayoff::QCTimeDepositPayoff(QCIntrstRtShrdPtr rate,
	shared_ptr<QCInterestRateLeg> irLeg,
	QCDate valueDate,
	QCIntRtCrvShrdPtr discountCurve
	):
	QCInterestRatePayoff(rate, irLeg, valueDate, discountCurve, discountCurve, nullptr)
{
	if (_irLeg->size() > 1)
	{
		throw invalid_argument("IR Leg for a discount bond payoff can have only one period.");
	}

	_setAllRates();
}

void QCTimeDepositPayoff::_setAllRates()
{
	//cout << "Enter QCTimeDepositPayoff::_setAllRates" << endl;

	unsigned int numPeriods = _irLeg->size();

	_allRates.resize(numPeriods);
	double rate{ _rate->getValue() };
	for (unsigned int i = 0; i < numPeriods; ++i)
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

QCInterestRateLeg::QCInterestRatePeriod QCTimeDepositPayoff::getPeriodAt(unsigned int n) const
{
	return _irLeg->getPeriodAt(n);
}

bool QCTimeDepositPayoff::operator<(const QCTimeDepositPayoff& rhs) const
{
	return get<QCInterestRateLeg::intRtPrdElmntEndDate>(this->getPeriodAt(0)) <
		get<QCInterestRateLeg::intRtPrdElmntEndDate>(rhs.getPeriodAt(0));
}

bool QCTimeDepositPayoff::lessThan(shared_ptr<QCTimeDepositPayoff> lhs, shared_ptr<QCTimeDepositPayoff> rhs)
{
	return *lhs < *rhs;
}

double QCTimeDepositPayoff::getRateValue()
{
	return _rate->getValue();
}

QCTimeDepositPayoff::~QCTimeDepositPayoff()
{
}

