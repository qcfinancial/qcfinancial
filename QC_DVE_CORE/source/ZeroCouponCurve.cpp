#include <math.h>

#include "asset_classes/ZeroCouponCurve.h"

namespace QCode
{
	namespace Financial
	{
		ZeroCouponCurve::ZeroCouponCurve(
			shared_ptr<QCInterpolator> curve,
			QCInterestRate intRate) : InterestRateCurve(curve, intRate)
		{
            _wfDerivatives.resize(_curve->getLength());
            _fwdWfDerivatives.resize(_curve->getLength());
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

			size_t curveLength = _curve->getLength();

			double rate1;
			double wf1;
			double dwf1;
            std::vector<double> der1;
            der1.resize(curveLength);
			if (d1 > 0)
            {
			    rate1 = _curve->interpolateAt(d1);
                for (size_t i = 0; i < curveLength; ++i)
                {
                    der1.at(i) = _curve->rateDerivativeAt(i);
                }
                _intRate.setValue(rate1);
                wf1 = _intRate.wf(d1);
                dwf1 = _intRate.dwf(d1);
            }
			else
            {
			    wf1 = 1.0;
			    dwf1 = 1.0;
                for (size_t i = 0; i < curveLength; ++i)
                {
                    der1.at(i) = 0.0;
                }
            }

			double rate2;
			double wf2;
			double dwf2;
            std::vector<double> der2;
            der2.resize(curveLength);
			if (d2 > 0)
            {
                rate2 = _curve->interpolateAt(d2);
                for (size_t i = 0; i < curveLength; ++i)
                {
                    der2.at(i) = _curve->rateDerivativeAt(i);
                }
                _intRate.setValue(rate2);
                wf2 = _intRate.wf(d2);
                dwf2 = _intRate.dwf(d2);
            }
			else
            {
                wf2 = 1.0;
                dwf2 = 1.0;
                for (size_t i = 0; i < curveLength; ++i)
                {
                    der2.at(i) = 0.0;
                }
            }

			//Se loopea para tener las derivadas
			if (d1 <= 0 && d2 <= 0)
            {
                for (size_t i = 0; i < curveLength; ++i)
                {
                    _fwdWfDerivatives.at(i) = 0.0;
                }
            }
			else if (d1 <= 0 && d2 > 0)
            {
                for (size_t i = 0; i < curveLength; ++i)
                {
                    _fwdWfDerivatives.at(i) = dwf2 * der2.at(i);
                }
            }
			else
            {
                for (size_t i = 0; i < curveLength; ++i)
			    {
                    _fwdWfDerivatives.at(i) = pow(wf1, -2.0) *
                            (dwf2 * der2.at(i) * wf1 - wf2 * dwf1 * der1.at(i));
                }
            }

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
				_wfDerivatives.at(i) = -_dfDerivatives.at(i) * pow(wf, 2);
			}

			return 1 / wf;
		}

		double ZeroCouponCurve::dfDerivativeAt(unsigned int index)
		{
			return _dfDerivatives.at(index);
		}

        double ZeroCouponCurve::wfDerivativeAt(unsigned int index)
        {
            return _wfDerivatives.at(index);
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
