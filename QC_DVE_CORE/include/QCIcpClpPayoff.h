#ifndef QCICPCLPPAYOFF_H
#define QCICPCLPPAYOFF_H

#include "QCInterestRatePayoff.h"
class QCIcpClpPayoff :
	public QCInterestRatePayoff
{
public:
	QCIcpClpPayoff(QCIntrstRtShrdPtr floatingRate,
		double additiveSpread,
		double multipSpread,
		shared_ptr<QCInterestRateLeg> irLeg,
		QCIntRtCrvShrdPtr projectingCurve,
		QCIntRtCrvShrdPtr discountCurve,
		QCDate valueDate,
		QCTimeSeriesShrdPtr fixingData
		);
	double getForwardRateAt(int n);
	virtual ~QCIcpClpPayoff();

protected:
	virtual void _setAllRates() override; 
	QCIntRtCrvShrdPtr _projectingCurve;
	double _additiveSpread;
	double _multipSpread;
	vector<double> _forwardRates; //Aqui se guardan todas las fijaciones libres de spreads
};

#endif //QCICPCLPPAYOFF_H
