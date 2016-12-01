#ifndef QCINTERESTRATEPAYOFF_H
#define QCINTERESTRATEPAYOFF_H

#include <map>

#include "QCDefinitions.h"
#include "QCInterestRateLeg.h"
#include "QCZeroCouponCurve.h"

class QCInterestRatePayoff
{
public:
	QCInterestRatePayoff(const QCZrCpnCrvShrdPtr crv);
	virtual double payoff(const QCIntrstRtLgShrdPtr irLeg) { return 0.0; };
	virtual ~QCInterestRatePayoff();

protected:
	QCTimeSeriesShrdPtr _pastFixings;
};


#endif //QCINTERESTRATEPAYOFF_H

