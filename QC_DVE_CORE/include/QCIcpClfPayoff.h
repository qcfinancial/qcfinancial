#ifndef QCICPCLFPAYOFF_H
#define QCICPCLFPAYOFF_H

#include "QCInterestRatePayoff.h"
class QCIcpClfPayoff :
	public QCInterestRatePayoff
{
public:
	QCIcpClfPayoff(QCIntrstRtShrdPtr floatingRate,
		double additiveSpread,
		double multipSpread,
		shared_ptr<QCInterestRateLeg> irLeg,
		QCIntRtCrvShrdPtr projectingCurve,
		QCIntRtCrvShrdPtr discountCurve,
		QCDate valueDate,
		QCTimeSeriesShrdPtr fixingData,
		QCTimeSeriesShrdPtr fixingDataUF
		);
	double getForwardRateAt(int n);
	virtual ~QCIcpClfPayoff();

protected:
	virtual void _setAllRates() override;
	double _additiveSpread;
	double _multipSpread;
	vector<double> _forwardRates; //Aqui se guardan todas las fijaciones libres de spreads
	QCTimeSeriesShrdPtr _fixingDataUF;
};

#endif //QCICPCLFPAYOFF_H
