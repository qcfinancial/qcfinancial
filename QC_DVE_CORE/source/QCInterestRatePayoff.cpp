#include "QCInterestRatePayoff.h"

QCInterestRatePayoff::QCInterestRatePayoff(
	QCIntrstRtShrdPtr rate,
	shared_ptr<QCInterestRateLeg> irLeg,
	QCDate valueDate,
	QCZrCpnCrvShrdPtr discountCurve,
	QCTimeSeriesShrdPtr fixingData) :
	_rate(rate),
	_irLeg(irLeg),
	_valueDate(valueDate),
	_discountCurve(discountCurve),
	_fixingData(fixingData)
{
	//Determinar _currentPeriod
	unsigned long numPeriods = _irLeg->size();
	if (_valueDate > get<QCInterestRateLeg::intRtPrdElmntEndDate>(_irLeg->getPeriodAt(numPeriods)))
	{
		_currentPeriod = numPeriods; //Despues se controla al calcular pv si _currentPeriod tiene sentido
		return;
	}

	for (unsigned long i = 0; i < numPeriods; ++i)
	{
		if (_valueDate >= get<QCInterestRateLeg::intRtPrdElmntStartDate>(_irLeg->getPeriodAt(i)) &&
			_valueDate < get<QCInterestRateLeg::intRtPrdElmntEndDate>(_irLeg->getPeriodAt(i)))
		{
			_currentPeriod = i;
			return;
		}
	}
	_currentPeriod = -1;
}

//Esta se implementa en las subclases
void QCInterestRatePayoff::_setAllRates() {}

void QCInterestRatePayoff::payoff()
{
	_payoffs.clear();
	int tempCurrentPeriod;
	if (_currentPeriod == -1)
	{
		tempCurrentPeriod = 0;
	}
	else
	{
		tempCurrentPeriod = _currentPeriod;
	}
	for (int i = tempCurrentPeriod; i < _irLeg->size(); ++i)
	{
		QCInterestRateLeg::QCInterestRatePeriod prd = _irLeg->getPeriodAt(i);

		//Agregamos la disposicion inicial si corresponde
		_payoffs.push_back(make_tuple(get<QCInterestRateLeg::intRtPrdElmntStartDate>(prd),
			qcAccretion, get<QCInterestRateLeg::intRtPrdElmntInitialAccrtn>(prd) *
			get<QCInterestRateLeg::intRtPrdElmntAcctrnIsCshflw>(prd)));

		//Se fija el valor de la tasa
		_rate->setValue(_allRates.at(i));

		//Se calcula el interes
		double notional = get<QCInterestRateLeg::intRtPrdElmntNotional>(prd);
		QCDate startDate = get<QCInterestRateLeg::intRtPrdElmntStartDate>(prd);
		QCDate endDate = get<QCInterestRateLeg::intRtPrdElmntEndDate>(prd);
		double interest = notional * (_rate->wf(startDate, endDate) - 1);
		_payoffs.push_back(make_tuple(get<QCInterestRateLeg::intRtPrdElmntSettlmntDate>(prd),
			qcInterest, interest));

		//Se agrega la amortizacion si corresponde
		_payoffs.push_back(make_tuple(get<QCInterestRateLeg::intRtPrdElmntSettlmntDate>(prd),
			qcAmortization, get<QCInterestRateLeg::intRtPrdElmntFinalAmrtztn>(prd) *
			get<QCInterestRateLeg::inRtPrdElmntAmrtztnIsCshflw>(prd)));
	}
}

int QCInterestRatePayoff::payoffSize()
{
	return _payoffs.size();
}

tuple<QCDate, QCCashFlowLabel, double> QCInterestRatePayoff::getCashflowAt(unsigned int n)
{
	return _payoffs.at(n);
}

double QCInterestRatePayoff::presentValue()
{
	payoff();
	double pv{ 0.0 };
	//Tamagno del vector de derivadas
	_pvRateDerivatives.resize(_discountCurve->getLength());
	for (unsigned int i = 0; i < _pvRateDerivatives.size() - 1; ++i)
	{
		_pvRateDerivatives.at(i) = 0;
	}
	for (const auto& cshflw : _payoffs)
	{
		long d = _valueDate.dayDiff(get<0>(cshflw));
		double flujo = get<2>(cshflw);
		double df = _discountCurve->getDiscountFactorAt(d);
		pv += flujo * df;
		for (unsigned int i = 0; i < _discountCurve->getLength(); ++i)
		{
			_pvRateDerivatives.at(i) += flujo * _discountCurve->dfDerivativeAt(i);
		}
	}

	
	return pv;
}

double QCInterestRatePayoff::getPvRateDerivativeAt(unsigned int index)
{
	return _pvRateDerivatives.at(index);
}


QCInterestRatePayoff::~QCInterestRatePayoff() {}
