#include "QCZeroCouponCurve.h"


//Constructor
QCZeroCouponCurve::QCZeroCouponCurve(shared_ptr<QCInterpolator> curve,
	QCInterestRate intRate) : QCInterestRateCurve(curve, intRate)
{
	_dfDerivatives.resize(_curve->getLength());
	_fwdWfDerivatives.resize(_curve->getLength());
}

double QCZeroCouponCurve::getRateAt(long d)
{
	return 0.0;
}

double QCZeroCouponCurve::getDiscountFactorAt(long d)
{
	return 0.0;
}

double QCZeroCouponCurve::getDiscountFactorFwd(long d1, long d2)
{
	return 0.0;
}

double QCZeroCouponCurve::getInstantForwardRateAt(long d)
{
	return 0.0;
}

double QCZeroCouponCurve::getDerivInstantForwardRateAt(long d)
{
	return 0.0;
}

double QCZeroCouponCurve::dfDerivativeAt(unsigned int index)
{
	return _dfDerivatives.at(index);
}

//Destructor
QCZeroCouponCurve::~QCZeroCouponCurve(void)
{
}