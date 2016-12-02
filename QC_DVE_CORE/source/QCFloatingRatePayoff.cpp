#include "QCFloatingRatePayoff.h"

QCFloatingRatePayoff::QCFloatingRatePayoff(QCIntrstRtShrdPtr floatingRate,
	QCIntrstRtLgShrdPtr irLeg,
	QCZrCpnCrvShrdPtr projectingCurve,
	QCZrCpnCrvShrdPtr discountCurve,
	QCDate valueDate,
	QCTimeSeriesShrdPtr fixingData):
	QCInterestRatePayoff(floatingRate, irLeg, valueDate, discountCurve, fixingData),
	_projectingCurve(projectingCurve)
{
}

void QCFloatingRatePayoff::_setAllRates()
{
	//La clase necesita un fixing date rule
	//La regla debe decir el lag antes de start date, cada cuantos periodos se fixea y el tenor de la tasa fixeada
}

QCFloatingRatePayoff::~QCFloatingRatePayoff()
{
}
