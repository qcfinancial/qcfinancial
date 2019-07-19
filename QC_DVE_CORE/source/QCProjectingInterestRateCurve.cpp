#include "QCProjectingInterestRateCurve.h"


QCProjectingInterestRateCurve::QCProjectingInterestRateCurve(shared_ptr<QCInterpolator> curve,
	QCInterestRate intRate) : QCInterestRateCurve(curve, intRate)
{
}

double QCProjectingInterestRateCurve::getRateAt(long d)
{
	return _curve->interpolateAt(d);
}

double QCProjectingInterestRateCurve::getDiscountFactorAt(long d)
{
	return 1.0;
}

double QCProjectingInterestRateCurve::dfDerivativeAt(unsigned int index)
{
	return 1.0;
}

double QCProjectingInterestRateCurve::getForwardRate(QCInterestRate& intRate, long d1, long d2)
{
	return getForwardRate(d1, d2);
}
double QCProjectingInterestRateCurve::getForwardRate(long d1, long d2)
{
	//Este metodo tiene esta firma para que, para las patas flotantes,
	//el uso de curva cero cupon o curva de proyeccion sea transparente
	return _curve->interpolateAt(d1);
}

double QCProjectingInterestRateCurve::getForwardWf(long d1, long d2)
{
	_intRate.setValue(_curve->interpolateAt(d1));
	return _intRate.wf(d1);
}

double QCProjectingInterestRateCurve::fwdWfDerivativeAt(unsigned int index)
{
	return 0.0;
}

QCProjectingInterestRateCurve::~QCProjectingInterestRateCurve()
{
}
