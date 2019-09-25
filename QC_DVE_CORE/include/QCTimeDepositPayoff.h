#ifndef QCTIMEDEPOSITPAYOFF_H
#define QCTIMEDEPOSITPAYOFF_H

#include "QCInterestRatePayoff.h"
#include "asset_classes/QCInterestRate.h"
#include "asset_classes/QCCurrencyConverter.h"

/*!
* @brief QCDiscountBondPayoff hereda de QCInterestRatePayoff e implementa la estructura de un bono
* a descuento, como un PDBC, T-Bill o pata de un fx forward.
*/

class QCTimeDepositPayoff : public QCInterestRatePayoff
{
public:
	/*!
	* Constructor, se apoya en el constructor de la clase madre.
	* @param rate valor y forma de la tasa fija.
	* @param irLeg estructura de fechas y amortizaciones. En este caso, irLeg debe consistir
	* de un �nico per�odo con amortizaci�n bullet que constituye flujo y disposici�n inicial
	* que constituye flujo.
	* @param discountCurve curva de descuento (para la obtenci�n del valor presente)
	* @param valueDate fecha de valorizaci�n.
	*/
	QCTimeDepositPayoff(QCIntrstRtShrdPtr rate,
		shared_ptr<QCInterestRateLeg> irLeg,
		QCDate valueDate,
		QCIntRtCrvShrdPtr discountCurve
		);

	/*!
	* Retorna el per�odo en el �ndice solicitado
	* @param n �ndice del per�odo
	* @return per�odo
	*/
	QCInterestRateLeg::QCInterestRatePeriod getPeriodAt(unsigned int n) const;

	/*!
	* Operator < sobrecarga
	* @param rhs
	* @return (bool)
	*/
	bool operator<(const QCTimeDepositPayoff& rhs) const;

	/*!
	* Funci�n similar al operador < pero que act�a sobre shared_ptr<QCTimeDeposit>
	* @param rhs
	* @return (bool)
	*/
	static bool lessThan(shared_ptr<QCTimeDepositPayoff> lhs, shared_ptr<QCTimeDepositPayoff> rhs);

	/*!
	* Devuelve el valor de la tasa del dep�sito
	* @return valor de la tasa
	*/
	double getRateValue();

	/*!
	* Destructor
	*/
	virtual ~QCTimeDepositPayoff();

protected:
	/*!
	* Se fija la tasa a aplicar en cada per�odo. En este caso se define rate en el �nico
	* pago del payoff.
	*/
	virtual void _setAllRates() override;
};

#endif //QCTIMEDEPOSITPAYOFF_H
