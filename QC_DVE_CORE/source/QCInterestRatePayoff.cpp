#include "QCInterestRatePayoff.h"

QCInterestRatePayoff::QCInterestRatePayoff(
	QCIntrstRtShrdPtr rate,
	QCIntrstRtLgShrdPtr irLeg,
	QCDate valueDate,
	QCZrCpnCrvShrdPtr discountCurve) :
	_rate(rate),
	_irLeg(irLeg),
	_valueDate(valueDate),
	_discountCurve(discountCurve)
{
	_payoffs.resize(_irLeg->size() * 3); //Por cada periodo hay 3 posibles flujos
	//En este loop se determina cual es la fecha de inicio del cupon
	//corriente y que indice tiene ese periodo
	for (int i = 0; i < _irLeg->size(); ++i)
	{
		if (get<intRtPrdElmntStartDate>(_irLeg->getPeriodAt(i)) >= _valueDate)
		{
			_datesPastFixings.push_back(
				get<intRtPrdElmntStartDate>(_irLeg->getPeriodAt(i)));
			_currentPeriod = i;
			return;
		}
	}
}

const vector<QCDate>& QCInterestRatePayoff::getDatesForPastFixings()
{
	return _datesPastFixings;
}

void QCInterestRatePayoff::setRatesForPastFixings(QCTimeSeriesShrdPtr dateRates)
{
	for (auto const& x : *_pastFixings)
	{
		(*_pastFixings)[x.first] = x.second;
	}
}

void QCInterestRatePayoff::_setAllRates()
{}

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

tuple<QCDate, QCCashFlowLabel, double> QCInterestRatePayoff::getCashFlowAt(unsigned int n)
{
	return _payoffs.at(n);
}

double QCInterestRatePayoff::presentValue()
{
	return 0.0;
}

QCInterestRatePayoff::~QCInterestRatePayoff()
{}
