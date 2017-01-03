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
	double _additiveSpread;
	double _multipSpread;
	
	//Aqui se guardan todas las fijaciones libres de spreads
	vector<double> _forwardRates;
};

#endif //QCICPCLPPAYOFF_H
