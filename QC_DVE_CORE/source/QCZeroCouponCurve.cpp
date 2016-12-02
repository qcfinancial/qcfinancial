#include "QCZeroCouponCurve.h"
#include <math.h>

QCZeroCouponCurve::QCZeroCouponCurve(shared_ptr<QCInterpolator> curva,
	shared_ptr<QCYearFraction> yf, shared_ptr<QCWealthFactor> wf) : _curva(curva),
	_yf(yf), _wf(wf)
{}

double QCZeroCouponCurve::getRateAt(double t)
{
	return _curva->interpolateAt(t);
}

double QCZeroCouponCurve::getInstantForwardRateAt(double t)
{
	double rt = _curva->interpolateAt(t);
	double drt = _curva->derivativeAt(t);
	return rt + t * drt;
}

double QCZeroCouponCurve::getDerivInstantForwardRateAt(double t)
{
	double drt = _curva->derivativeAt(t);
	double d2rt = _curva->secondDerivativeAt(t);
	return 2.0 * drt + t * d2rt;
}

double QCZeroCouponCurve::getDiscountFactorAt(double t)
{
	//Esta funcion necesita mas trabajo para poder especificar
	//un factor de capitalizacion arbitrario.
	double rate = _curva->interpolateAt(t);
	double yf = _yf->yf(t);
	return 1.0 / _wf->wf(rate, yf);
	//return exp(-t * _curva->interpolateAt(t));
}

QCZeroCouponCurve::~QCZeroCouponCurve(void)
{
}
