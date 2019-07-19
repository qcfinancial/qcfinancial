/** class QCProjectingInterestRateCurve
* Clase que modela curvas de proyeccion modernas
* donde la tasa fwd se obtiene directamente como un punto interpolado de la curva
* sin usar factores de descuento fwd.
*/

#ifndef QCPROJECTINGINTERESTRATECURVE_H
#define QCPROJECTINGINTERESTRATECURVE_H

#include "QCInterestRateCurve.h"

class QCProjectingInterestRateCurve :
	public QCInterestRateCurve
{
public:
	QCProjectingInterestRateCurve(shared_ptr<QCInterpolator> curve,
		QCInterestRate intRate);

	virtual double getRateAt(long d) override;
	virtual double getDiscountFactorAt(long d) override;
	virtual double getForwardRate(QCInterestRate& intRate, long d1, long d2) override;
	virtual double getForwardRate(long d1, long d2) override;
	virtual double getForwardWf(long d1, long d2) override;
	virtual double dfDerivativeAt(unsigned int index) override;
	virtual double fwdWfDerivativeAt(unsigned int index) override;

	virtual ~QCProjectingInterestRateCurve();
};

#endif //QCPROJECTINGINTERESTRATECURVE_H
