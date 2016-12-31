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
	double getPvRateDerivativeAt(unsigned int index);
	tuple<QCDate, QCCashFlowLabel, double> getCashflowAt(unsigned int n);
	virtual ~QCInterestRatePayoff();

protected:
	QCInterestRatePayoff(
		QCIntrstRtShrdPtr rate,
		shared_ptr<QCInterestRateLeg> irLeg,
		QCDate valueDate,
		QCIntRtCrvShrdPtr discountCurve,
		QCTimeSeriesShrdPtr fixingData);
	
	QCIntrstRtShrdPtr _rate;
	shared_ptr<QCInterestRateLeg> _irLeg;
	QCDate _valueDate;
	QCIntRtCrvShrdPtr _discountCurve;
	QCTimeSeriesShrdPtr _fixingData;
	
	int _currentPeriod;
	vector<tuple<QCDate, QCCashFlowLabel, double>> _payoffs;

	virtual void _setAllRates();
	vector<double> _allRates;

	vector<double> _pvRateDerivatives;
};

#endif //QCINTERESTRATEPAYOFF_H

