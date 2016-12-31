/** class QCInterestRateCurve
* brief Clase base (abstracta) para todas las curvas de tasas de interes
* details Esta clase solo define un metodo (obtener una tasas dado un plazo).
* Los distintos tipos de curvas (descuento, proyeccion, por factores o tasas)
* se derivan de esta.
*/

#ifndef QCINTERESTRATECURVE_H
#define QCINTERESTRATECURVE_H

#include "QCInterestRate.h"
#include "QCInterpolator.h"
#include "QCDefinitions.h"

class QCInterestRateCurve
{
public:
	enum QCTypeInterestRateCurve
	{
		qcProjectingCurve,
		qcZeroCouponCurve,
		qcDiscountFactorCurve
	};

	QCInterestRateCurve(shared_ptr<QCInterpolator> curve,
		QCInterestRate intRate) : _curve(curve), _intRate(intRate)
	{
	}
	virtual double getRateAt(long d) = 0;
	virtual double getDiscountFactorAt(long d) = 0;
	virtual double getForwardRate(long d1, long d2) = 0;
	virtual double getForwardWf(long d1, long d2) = 0;
	virtual double dfDerivativeAt(unsigned int index) = 0;


	virtual ~QCInterestRateCurve(){}

	long getLength() { return _curve->getLength(); }

protected:
	shared_ptr<QCInterpolator> _curve;
	QCInterestRate _intRate;
	vector<double> _dfDerivatives;	//Derivadas del factor de descuento interpolado
									//respecto a las tasas de la curva.
};

#endif //QCINTERESTRATECURVE_H

