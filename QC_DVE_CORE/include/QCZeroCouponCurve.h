/** class QCInterestRateCurve
* @brief Clase base (abstracta) para todas las curvas de tasas de interes que son curvas cero cupon.
* @details Esta clase solo define metodos adicionales (obtener una tasas dado un plazo) que todas
* sus clases derivadas deben override.
*/

#ifndef QCZEROCOUPONCURVE_H
#define QCZEROCOUPONCURVE_H

#include "QCInterestRateCurve.h"
#include "QCInterestRate.h"
#include "QCDefinitions.h"
#include "QCInterpolator.h"
#include "QCWealthFactor.h"
#include "QCYearFraction.h"

class QCZeroCouponCurve : public QCInterestRateCurve
{
public:
	//Constructor
	QCZeroCouponCurve(shared_ptr<QCInterpolator> curve,
		QCInterestRate intRate);

	//Methods
	virtual double getRateAt(long d);
	virtual double getDiscountFactorAt(long d);
	virtual double getDiscountFactorFwd(long d1, long d2);
	virtual double getInstantForwardRateAt(long d);
	virtual double getDerivInstantForwardRateAt(long d);
	virtual double dfDerivativeAt(unsigned int index);

	//Destructor
	virtual ~QCZeroCouponCurve(void);

protected:
	vector<double> _dfDerivatives;	//Derivadas del factor de descuento interpolado
									//respecto a las tasas de la curva.

};

#endif //QCZEROCOUPONCURVE_H

