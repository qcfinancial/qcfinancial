#ifndef QCZEROCOUPONDISCOUNTFACTORCURVE_H
#define QCZEROCOUPONDISCOUNTFACTORCURVE_H

#include "QCZeroCouponCurve.h"
#include "QCInterestRate.h"

class QCZeroCouponDiscountFactorCurve :
	public QCZeroCouponCurve
{
public:
	QCZeroCouponDiscountFactorCurve(shared_ptr<QCInterpolator> curve,
		QCInterestRate intRate);
	virtual double getRateAt(long d) override;
	virtual double getDiscountFactorAt(long d) override;
	virtual double getDiscountFactorFwd(long d1, long d2) override;
	virtual double getForwardRate(QCInterestRate& intRate, long d1, long d2) override;
	virtual double getForwardRate(long d1, long d2) override;
	virtual double getForwardWf(long d1, long d2) override;
	virtual double getInstantForwardRateAt(long d) override;
	virtual double getDerivInstantForwardRateAt(long d) override;
	virtual double fwdWfDerivativeAt(unsigned int index) override;

	virtual ~QCZeroCouponDiscountFactorCurve();
};

#endif //QCZEROCOUPONDISCOUNTFACTORCURVE_H

