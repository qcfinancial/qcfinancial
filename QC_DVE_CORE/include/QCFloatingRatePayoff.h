#ifndef QCFLOATINGRATEPAYOFF_H
#define QCFLOATINGRATEPAYOFF_H

#include "QCInterestRatePayoff.h"
class QCFloatingRatePayoff :
	public QCInterestRatePayoff
{
public:
	QCFloatingRatePayoff(QCIntrstRtShrdPtr floatingRate,
		double additiveSpread,
		double multipSpread,
		shared_ptr<QCInterestRateLeg> irLeg,
		QCIntRtCrvShrdPtr projectingCurve,
		QCIntRtCrvShrdPtr discountCurve,
		QCDate valueDate,
		QCTimeSeriesShrdPtr fixingData
		);
	double getForwardRateAt(int n);
	virtual ~QCFloatingRatePayoff();

protected:
	virtual void _setAllRates() override;
	QCIntRtCrvShrdPtr _projectingCurve;
	double _additiveSpread;
	double _multipSpread;
	vector<double> _forwardRates; //Aqui se guardan todas las fijaciones libres de spreads
};

#endif //QCFLOATINGRATEPAYOFF_H
