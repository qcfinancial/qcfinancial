#ifndef QCINTERESTRATELEG_H
#define QCINTERESTRATELEG_H

#include "QCDefinitions.h"

class QCInterestRateLeg
{
public:
	QCInterestRateLeg(QCIntrstRtPrdsShrdPntr periods, int lastPeriod);

	~QCInterestRateLeg();

protected:
	QCIntrstRtPrdsShrdPntr _periods;
	int _lastPeriod;
};

#endif //QCINTERESTRATELEG_H

