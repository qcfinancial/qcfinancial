#ifndef QCFLOATINGRATEPAYOFF_H
#define QCFLOATINGRATEPAYOFF_H

#include "QCInterestRatePayoff.h"
class QCFloatingRatePayoff :
	public QCInterestRatePayoff
{
public:
	QCFloatingRatePayoff(QCIntrstRtShrdPtr floatingRate,
		QCIntrstRtLgShrdPtr irLeg,
		QCZrCpnCrvShrdPtr projectingCurve,
		QCZrCpnCrvShrdPtr discountCurve,
		QCDate valueDate,
		QCTimeSeriesShrdPtr fixingData);

	virtual ~QCFloatingRatePayoff();

protected:
	virtual void _setAllRates() override;
	QCZrCpnCrvShrdPtr _projectingCurve;
};

#endif //QCFLOATINGRATEPAYOFF_H
