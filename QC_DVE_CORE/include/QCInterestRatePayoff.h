#ifndef QCINTERESTRATEPAYOFF_H
#define QCINTERESTRATEPAYOFF_H

#include <map>
#include <tuple>
#include <array>

#include "QCDefinitions.h"
#include "QCDate.h"
#include "QCInterestRateLeg.h"
#include "QCZeroCouponCurve.h"
#include "QCInterestRate.h"

//Tengo que terminar de organizar bien el metodo _setAllRates

enum QCCashFlowLabel
{
	qcAccretion,
	qcInterest,
	qcAmortization
};

class QCInterestRatePayoff
{
public:
	const vector<QCDate>& getDatesForPastFixings();
	void setRatesForPastFixings(QCTimeSeriesShrdPtr dateRates);
	void payoff();
	double presentValue();
	tuple<QCDate, QCCashFlowLabel, double> getCashFlowAt(unsigned int n);
	virtual ~QCInterestRatePayoff();

protected:
	QCInterestRatePayoff(
		QCIntrstRtShrdPtr rate,
		QCIntrstRtLgShrdPtr _irLeg,
		QCDate valueDate,
		QCZrCpnCrvShrdPtr discountCurve);
	QCIntrstRtShrdPtr _rate;
	QCIntrstRtLgShrdPtr _irLeg;
	QCDate _valueDate;
	QCZrCpnCrvShrdPtr _discountCurve;
	
	virtual void _setAllRates();
	vector<double> _allRates;

	QCTimeSeriesShrdPtr _pastFixings;
	vector<QCDate> _datesPastFixings;
	int _currentPeriod;
	vector<tuple<QCDate, QCCashFlowLabel, double>> _payoffs;
};

#endif //QCINTERESTRATEPAYOFF_H

