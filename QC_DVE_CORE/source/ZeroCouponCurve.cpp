#include <math.h>

#include "ZeroCouponCurve.h"

namespace QCode
{
	namespace Financial
	{
		ZeroCouponCurve::ZeroCouponCurve(
			shared_ptr<QCInterpolator> curve,
			QCInterestRate intRate) : InterestRateCurve(curve, intRate)
		{
		}

		double ZeroCouponCurve::getRateAt(long d)
		{
			return _curve->interpolateAt(d);
		}

		QCInterestRate ZeroCouponCurve::getQCInterestRateAt(long d)
		{
			auto rate = getRateAt(d);
			InterestRateCurve::_intRate.setValue(rate);
			return InterestRateCurve::_intRate;
		}

		double ZeroCouponCurve::getForwardWf(long d1, long d2)
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

		double ZeroCouponCurve::getForwardRateWithRate(QCInterestRate& intRate, long d1, long d2)
		{
			double wf = this->getForwardWf(d1, d2);
			return intRate.getRateFromWf(wf, d2 - d1);
		}

		double ZeroCouponCurve::getForwardRate(long d1, long d2)
		{
			double wf = this->getForwardWf(d1, d2);
			return _intRate.getRateFromWf(wf, d2 - d1);
		}

		double ZeroCouponCurve::getDiscountFactorAt(long d)
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

		double ZeroCouponCurve::dfDerivativeAt(unsigned int index)
		{
			return _dfDerivatives.at(index);
		}

		double ZeroCouponCurve::fwdWfDerivativeAt(unsigned int index)
		{
			return _fwdWfDerivatives.at(index);
		}

		ZeroCouponCurve::~ZeroCouponCurve()
		{
		}
	}
}
