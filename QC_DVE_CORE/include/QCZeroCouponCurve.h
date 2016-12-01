#ifndef QCZEROCOUPONCURVE_H
#define QCZEROCOUPONCURVE_H

#include "QCInterpolator.h"

class QCZeroCouponCurve
{
public:
	//Constructor
	QCZeroCouponCurve(shared_ptr<QCInterpolator> curva) : _curva(curva) {};

	//Methods
	double getRateAt(double t);
	double getInstantForwardRateAt(double t);
	double getDerivInstantForwardRateAt(double t);
	double getDiscountFactorAt(double t);

	//Destructor
	~QCZeroCouponCurve(void);

private:
	shared_ptr<QCInterpolator> _curva;
};


#endif //QCZEROCOUPONCURVE_H

