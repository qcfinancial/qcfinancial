#include "curves/QCZeroCouponDiscountFactorCurve.h"


QCZeroCouponDiscountFactorCurve::QCZeroCouponDiscountFactorCurve(
	shared_ptr<QCInterpolator> curve,
	QCInterestRate intRate) : QCZeroCouponCurve(curve, intRate)
{
}

double QCZeroCouponDiscountFactorCurve::getRateAt(long d)
{
	return _curve->interpolateAt(d);
}

double QCZeroCouponDiscountFactorCurve::getDiscountFactorAt(long d)
{
	return _intRate.getRateFromWf(_curve->interpolateAt(d), d);
}

double QCZeroCouponDiscountFactorCurve::getDiscountFactorFwd(long d1, long d2)
{
	return _curve->interpolateAt(d2) / _curve->interpolateAt(d1);
}

double QCZeroCouponDiscountFactorCurve::getForwardRate(QCInterestRate& intRate, long d1, long d2)
{
	double df = this->getDiscountFactorFwd(d1, d2);
	return intRate.getRateFromWf(df, d2 - d1);
}

double QCZeroCouponDiscountFactorCurve::getForwardRate(long d1, long d2)
{
	double df = this->getDiscountFactorFwd(d1, d2);
	return _intRate.getRateFromWf(df, d2 - d1);
}

double QCZeroCouponDiscountFactorCurve::getForwardWf(long d1, long d2)
{
	return 1 / this->getDiscountFactorFwd(d1, d2);
}

double QCZeroCouponDiscountFactorCurve::fwdWfDerivativeAt(unsigned int index)
{
	//TO DO
	return 0.0;
}

double QCZeroCouponDiscountFactorCurve::getInstantForwardRateAt(long d)
{
	//TO DO
	return 0.0;
}

double QCZeroCouponDiscountFactorCurve::getDerivInstantForwardRateAt(long d)
{
	//TO DO
	return 0.0;
}

QCZeroCouponDiscountFactorCurve::~QCZeroCouponDiscountFactorCurve()
{
}
