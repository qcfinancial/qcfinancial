#include "QCInterestRatePayoff.h"

QCInterestRatePayoff::QCInterestRatePayoff(
	QCIntrstRtShrdPtr rate,
	QCIntrstRtLgShrdPtr irLeg,
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
	for (int i = 0; i < _irLeg->size(); ++i)
	{
		if (_valueDate <= get<intRtPrdElmntStartDate>(_irLeg->getPeriodAt(i)))
		{
			_currentPeriod = i;
			return;
		}
	}
	_currentPeriod = _irLeg->size(); //Despues se controla al calcular pv si _currentPeriod tiene sentido
}

//Esta se implementa en las subclases
void QCInterestRatePayoff::_setAllRates() {}

void QCInterestRatePayoff::payoff()
{
	_payoffs.clear();
	for (int i = _currentPeriod; i < _irLeg->size(); ++i)
	{
		QCInterestRatePeriod prd = _irLeg->getPeriodAt(i);

		//Agregamos la disposicion inicial si corresponde
		_payoffs.push_back(make_tuple(get<intRtPrdElmntStartDate>(prd),
			qcAccretion, get<intRtPrdElmntInitialAccrtn>(prd) *
			get<intRtPrdElmntAcctrnIsCshflw>(prd)));

		//Se fija el valor de la tasa
		_rate->setValue(_allRates.at(i));

		//Se calcula el interes
		double notional = get<intRtPrdElmntNotional>(prd);
		QCDate startDate = get<intRtPrdElmntStartDate>(prd);
		QCDate endDate = get<intRtPrdElmntEndDate>(prd);
		double interest = notional * (_rate->wf(startDate, endDate) - 1);
		_payoffs.push_back(make_tuple(get<intRtPrdElmntSettlmntDate>(prd),
			qcInterest, interest));

		//Se agrega la amortizacion si corresponde
		_payoffs.push_back(make_tuple(get<intRtPrdElmntSettlmntDate>(prd),
			qcAmortization, get<intRtPrdElmntFinalAmrtztn>(prd) *
			get<inRtPrdElmntAmrtztnIsCshflw>(prd)));
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
	for (const auto& cshflw : _payoffs)
	{
		long d = _valueDate.dayDiff(get<0>(cshflw));
		double flujo = get<2>(cshflw);
		double df = _discountCurve->getDiscountFactorAt(d);
		pv += flujo * df;
	}

	return pv;
}

QCInterestRatePayoff::~QCInterestRatePayoff() {}
