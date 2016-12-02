#ifndef QCZEROCOUPONCURVE_H
#define QCZEROCOUPONCURVE_H

#include "QCInterpolator.h"
#include "QCWealthFactor.h"
#include "QCYearFraction.h"

class QCZeroCouponCurve
{
public:
	//Constructor
	QCZeroCouponCurve(shared_ptr<QCInterpolator> curva,
		shared_ptr<QCYearFraction> yf, shared_ptr<QCWealthFactor> wf);

	//Methods
	double getRateAt(double t);
	double getInstantForwardRateAt(double t);
	double getDerivInstantForwardRateAt(double t);
	double getDiscountFactorAt(double t);

	//Destructor
	~QCZeroCouponCurve(void);

private:
	shared_ptr<QCInterpolator> _curva;
	shared_ptr<QCYearFraction> _yf;
	shared_ptr<QCWealthFactor> _wf;
};


#endif //QCZEROCOUPONCURVE_H

