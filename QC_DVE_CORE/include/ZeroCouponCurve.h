#ifndef ZEROCOUPONCURVE_H
#define ZEROCOUPONCURVE_H

#include "InterestRateCurve.h"
#include "QCInterestRate.h"

namespace QCode
{
	namespace Financial
	{
		class ZeroCouponCurve : public InterestRateCurve
		{
		public:
			ZeroCouponCurve(shared_ptr<QCInterpolator> curve, QCInterestRate intRate);
			double getRateAt(long d) override;
			double getDiscountFactorAt(long d) override;
			double getForwardRateWithRate(QCInterestRate& intRate, long d1, long d2) override;
			double getForwardRate(long d1, long d2) override;
			double getForwardWf(long d1, long d2) override;
			double dfDerivativeAt(unsigned int index) override;
			double fwdWfDerivativeAt(unsigned int index) override;
			virtual ~ZeroCouponCurve();

		};
	}
}

#endif //ZEROCOUPONCURVE_H
