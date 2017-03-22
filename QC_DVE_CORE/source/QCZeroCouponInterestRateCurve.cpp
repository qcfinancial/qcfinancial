#include "QCZeroCouponInterestRateCurve.h"


QCZeroCouponInterestRateCurve::QCZeroCouponInterestRateCurve(
	shared_ptr<QCInterpolator> curve,
	QCInterestRate intRate) : QCZeroCouponCurve(curve, intRate)
{
}

double QCZeroCouponInterestRateCurve::getRateAt(long d)
{
	return _curve->interpolateAt(d);
}

double QCZeroCouponInterestRateCurve::getDiscountFactorFwd(long d1, long d2)
{
	if (d1 > d2)
	{
		long d = d2;
		d2 = d1;
		d1 = d;
	}
	double rate1 = _curve->interpolateAt(d1);
	double rate2 = _curve->interpolateAt(d2);
	_intRate.setValue(rate1);
	double wf1 = _intRate.wf(d1);
	_intRate.setValue(rate2);
	double wf2 = _intRate.wf(d2);
	return wf1 / wf2;
}

double QCZeroCouponInterestRateCurve::getForwardWf(long d1, long d2)
{
	if (d1 > d2)
	{
		long d = d2;
		d2 = d1;
		d1 = d;
	}
	double rate1 = _curve->interpolateAt(d1);	
	_intRate.setValue(rate1);
	double wf1 = _intRate.wf(d1);
	double dwf1 = _intRate.dwf(d1);
	
	double rate2 = _curve->interpolateAt(d2);
	_intRate.setValue(rate2);
	double wf2 = _intRate.wf(d2);
	double dwf2 = _intRate.dwf(d2);

	for (unsigned int i = 0; i < _curve->getLength(); ++i)
	{
		rate1 = _curve->interpolateAt(d1);
		double der = _curve->rateDerivativeAt(i);
		double ddwf1 = dwf1 * der;

		rate2 = _curve->interpolateAt(d2);
		der = _curve->rateDerivativeAt(i);
		double ddwf2 = dwf2 * der;

		if (d1 <= 0)
		{
			_fwdWfDerivatives.at(i) = ddwf2;
		}
		else
		{
			_fwdWfDerivatives.at(i) = pow(wf1, -2.0) * (ddwf2 * wf1 - wf2 * ddwf1);
		}
	}
	//Se loopea para tener las derivadas
	double result = wf2 / wf1;
	return result;
}

double QCZeroCouponInterestRateCurve::getForwardRate(QCInterestRate& intRate, long d1, long d2)
{
	double df = this->getDiscountFactorFwd(d1, d2);
	return intRate.getRateFromWf(1 / df, d2 - d1);
}

double QCZeroCouponInterestRateCurve::getForwardRate(long d1, long d2)
{
	double df = this->getDiscountFactorFwd(d1, d2);
	return _intRate.getRateFromWf(1 / df, d2 - d1);
}

double QCZeroCouponInterestRateCurve::getInstantForwardRateAt(long d)
{
	//Esto esta mal implementado ya que supone tasas en convencion compuesta continua
	double rt = _curve->interpolateAt(d);
	double drt = _curve->derivativeAt(d);
	return rt + d * drt;
}

double QCZeroCouponInterestRateCurve::getDerivInstantForwardRateAt(long d)
{
	//Esto esta mal implementado ya que supone tasas en convencion compuesta continua
	double drt = _curve->derivativeAt(d);
	double d2rt = _curve->secondDerivativeAt(d);
	return 2.0 * drt + d * d2rt;
}

double QCZeroCouponInterestRateCurve::getDiscountFactorAt(long d)
{
	double rate = _curve->interpolateAt(d);
	_intRate.setValue(rate);
	double wf = _intRate.wf(d);
	//Se loopea para tener las derivadas
	for (unsigned int i = 0; i < _curve->getLength(); ++i)
	{
		_dfDerivatives.at(i) = -pow(wf, -2.0) * _curve->rateDerivativeAt(i) * _intRate.dwf(d);
	}

	return 1 / wf;
}

double QCZeroCouponInterestRateCurve::dfDerivativeAt(unsigned int index)
{
	return _dfDerivatives.at(index);
}

double QCZeroCouponInterestRateCurve::fwdWfDerivativeAt(unsigned int index)
{
	return _fwdWfDerivatives.at(index);
}

QCZeroCouponInterestRateCurve::~QCZeroCouponInterestRateCurve()
{
}
