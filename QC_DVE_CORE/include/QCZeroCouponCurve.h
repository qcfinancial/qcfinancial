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

/*!
* @author Alvaro Díaz
* @brief Clase base abstracta para curvas cero cupón.
* @details Esta clase define varios métodos que las clases derivadas deben implementar.
*/
class QCZeroCouponCurve : public QCInterestRateCurve
{
public:
	/*!
	* Constructor
	* @param curve tenors y tasas de la curva
	* @param intRate convención de las tasas de la curva. El valor que venga dentro de este objeto es irrelevante.
	*/
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

};

#endif //QCZEROCOUPONCURVE_H

