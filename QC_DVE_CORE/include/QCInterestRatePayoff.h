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

enum QCCashFlowLabel
{
	qcAccretion,
	qcInterest,
	qcAmortization
};

class QCInterestRatePayoff
{
public:
	void payoff();
	int payoffSize();
	double presentValue();
	tuple<QCDate, QCCashFlowLabel, double> getCashflowAt(unsigned int n);
	virtual ~QCInterestRatePayoff();

protected:
	QCInterestRatePayoff(
		QCIntrstRtShrdPtr rate,
		QCIntrstRtLgShrdPtr irLeg,
		QCDate valueDate,
		QCZrCpnCrvShrdPtr discountCurve,
		QCTimeSeriesShrdPtr fixingData);
	QCIntrstRtShrdPtr _rate;
	QCIntrstRtLgShrdPtr _irLeg;
	QCDate _valueDate;
	QCZrCpnCrvShrdPtr _discountCurve;
	QCTimeSeriesShrdPtr _fixingData;
	
	int _currentPeriod;
	vector<tuple<QCDate, QCCashFlowLabel, double>> _payoffs;

	virtual void _setAllRates();
	vector<double> _allRates;
};

#endif //QCINTERESTRATEPAYOFF_H

