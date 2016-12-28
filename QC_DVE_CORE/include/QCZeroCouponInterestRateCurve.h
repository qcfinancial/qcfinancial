#ifndef QCZEROCOUPONINTERESTRATECURVE_H
#define QCZEROCOUPONINTERESTRATECURVE_H

#include "QCZeroCouponCurve.h"
#include "QCInterestRate.h"

class QCZeroCouponInterestRateCurve :
	public QCZeroCouponCurve
{
public:
	QCZeroCouponInterestRateCurve(shared_ptr<QCInterpolator> curve,
		QCInterestRate intRate);
	virtual double getRateAt(long d) override;
	virtual double getDiscountFactorAt(long d) override;
	virtual double getDiscountFactorFwd(long d1, long d2) override;
	virtual double getForwardRate(long d1, long d2) override;
	virtual double getForwardWf(long d1, long d2) override;
	virtual double getInstantForwardRateAt(long d) override;
	virtual double getDerivInstantForwardRateAt(long d) override;
	virtual double dfDerivativeAt(unsigned int index) override;
	virtual ~QCZeroCouponInterestRateCurve();

};

#endif //QCZEROCOUPONINTERESTRATECURVE_H
