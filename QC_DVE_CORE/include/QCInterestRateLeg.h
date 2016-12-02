#ifndef QCINTERESTRATELEG_H
#define QCINTERESTRATELEG_H

#include "QCDefinitions.h"

class QCInterestRateLeg
{
public:
	QCInterestRateLeg(QCIntrstRtPrdsShrdPntr periods,
		int lastPeriod);
	int size();
	QCInterestRatePeriod getPeriodAt(unsigned int n);
	~QCInterestRateLeg();

protected:
	QCIntrstRtPrdsShrdPntr _periods;
	int _lastPeriod;
};

#endif //QCINTERESTRATELEG_H

