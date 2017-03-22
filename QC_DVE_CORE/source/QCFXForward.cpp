#include "QCFXForward.h"


QCFXForward::QCFXForward(
	vector<shared_ptr<QCDiscountBondPayoff>> legs,
	QCCurrencyConverter::QCCurrency marketValueCurrency,
	shared_ptr<map<QCCurrencyConverter::QCFxRate, double>> fxRates) :
	_legs(legs), _marketValueCurrency(marketValueCurrency), _fxRates(fxRates)
{
}

double QCFXForward::marketValue()
{
	QCCurrencyConverter conv;
	QCCurrencyConverter::QCCurrency legCurrency = _legs.at(0)->getNotionalCurrency();
	QCCurrencyConverter::QCFxRate fxRate = conv.getStandardFxRate(legCurrency, _marketValueCurrency);
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
	double value2 = _legs.at(0)->presentValue();
	return value1 + conv.convert(value2, legCurrency, fxRateValue, fxRate);
}

QCFXForward::~QCFXForward()
{
}
