#include "QCFXForward.h"


QCFXForward::QCFXForward(
	vector<shared_ptr<QCDiscountBondPayoff>> legs,
	QCCurrencyConverter::QCCurrencyEnum marketValueCurrency,
	shared_ptr<map<QCCurrencyConverter::QCFxRateEnum, double>> fxRates) :
	_legs(legs), _marketValueCurrency(marketValueCurrency), _fxRates(fxRates)
{
	if (_legs.size() != 2)
	{
		throw invalid_argument("FX Forward has cannot have more than two legs.");
	}
	
	QCDate endDate0{ get<QCInterestRateLeg::intRtPrdElmntEndDate>(_legs.at(0)->getPeriod()) };
	QCDate endDate1{ get<QCInterestRateLeg::intRtPrdElmntEndDate>(_legs.at(1)->getPeriod()) };

	if (endDate0 != endDate1)
	{
		throw invalid_argument("End dates are different in both legs.");
	}
	_endDate = endDate0;
}

double QCFXForward::marketValue()
{
	QCCurrencyConverter conv;
	QCCurrencyConverter::QCCurrencyEnum legCurrency = _legs.at(0)->getNotionalCurrency();
	QCCurrencyConverter::QCFxRateEnum fxRate = conv.getStandardFxRate(legCurrency, _marketValueCurrency);
	double fxRateValue;
	auto it = _fxRates->find(fxRate);
	if (it != _fxRates->end())
	{
		fxRateValue = it->second;
	}
	else
	{
		throw invalid_argument("QCFXForward::marketValue: No Fx Rate value for FX.");
	}
	double value1 = _legs.at(0)->presentValue();
	value1 = conv.convert(value1, legCurrency, fxRateValue, fxRate);

	legCurrency = _legs.at(1)->getNotionalCurrency();
	fxRate = conv.getStandardFxRate(legCurrency, _marketValueCurrency);
	it = _fxRates->find(fxRate);
	if (it != _fxRates->end())
	{
		fxRateValue = it->second;
	}
	else
	{
		throw invalid_argument("QCFXForward::marketValue: No Fx Rate value for FX.");
	}
	double value2 = _legs.at(1)->presentValue();
	value2 = conv.convert(value2, legCurrency, fxRateValue, fxRate);
	return value1 + value2;

}

QCDate QCFXForward::getEndDate() const
{
	return _endDate;
}

bool QCFXForward::operator<(const QCFXForward& other)
{
	return this->getEndDate() < other.getEndDate();
}

bool QCFXForward::lessThan(shared_ptr<QCFXForward> lhs, shared_ptr<QCFXForward> rhs)
{
	return *lhs < *rhs;
}

shared_ptr<QCDiscountBondPayoff> QCFXForward::getPayoff(unsigned int numPayoff)
{
	if (numPayoff == 0 || numPayoff == 1)
	{
		return _legs.at(numPayoff);
	}
	else
	{
		return _legs.at(0);
	}
}

QCInterestRateLeg::QCInterestRatePeriod QCFXForward::getPeriod(unsigned int numLeg)
{
	if (numLeg == 0 || numLeg == 1)
	{
		return _legs.at(numLeg)->getPeriod();
	}
	return _legs.at(0)->getPeriod();

}

double QCFXForward::getPvRateDerivativeAtFromLeg(unsigned int index, unsigned int numPayoff)
{
	if (numPayoff > 1) numPayoff = 0;
	if (numPayoff == 0 || numPayoff == 1)
	{
		QCCurrencyConverter conv;
		QCCurrencyConverter::QCCurrencyEnum legCurrency = _legs.at(numPayoff)->getNotionalCurrency();
		QCCurrencyConverter::QCFxRateEnum fxRate = conv.getStandardFxRate(legCurrency, _marketValueCurrency);
		double fxRateValue;
		auto it = _fxRates->find(fxRate);
		if (it != _fxRates->end())
		{
			fxRateValue = it->second;
		}
		else
		{
			throw invalid_argument("QCFXForward::marketValue: No Fx Rate value for FX.");
		}
		double value = _legs.at(0)->getPvRateDerivativeAt(index);
		return conv.convert(value, legCurrency, fxRateValue, fxRate);
	}
    return 0.0;
}

QCFXForward::~QCFXForward()
{
}
