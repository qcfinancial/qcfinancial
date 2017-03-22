#ifndef QCTIMEDEPOSITPAYOFF_H
#define QCTIMEDEPOSITPAYOFF_H

#include "QCInterestRatePayoff.h"
#include "QCInterestRate.h"
#include "QCCurrencyConverter.h"

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
	* de un único período con amortización bullet que constituye flujo y disposición inicial
	* que constituye flujo.
	* @param discountCurve curva de descuento (para la obtención del valor presente)
	* @param valueDate fecha de valorización.
	*/
	QCTimeDepositPayoff(QCIntrstRtShrdPtr rate,
		shared_ptr<QCInterestRateLeg> irLeg,
		QCDate valueDate,
		QCIntRtCrvShrdPtr discountCurve
		);

	/*!
	* Retorna el período en el índice solicitado
	* @param n índice del período
	* @return período
	*/
	QCInterestRateLeg::QCInterestRatePeriod getPeriodAt(unsigned int n) const;

	/*!
	* Operator < sobrecarga
	* @param rhs
	* @return (bool)
	*/
	bool operator<(const QCTimeDepositPayoff& rhs) const;

	/*!
	* Función similar al operador < pero que actúa sobre shared_ptr<QCTimeDeposit>
	* @param rhs
	* @return (bool)
	*/
	static bool lessThan(shared_ptr<QCTimeDepositPayoff> lhs, shared_ptr<QCTimeDepositPayoff> rhs);

	/*!
	* Devuelve el valor de la tasa del depósito
	* @return valor de la tasa
	*/
	double getRateValue();

	/*!
	* Destructor
	*/
	virtual ~QCTimeDepositPayoff();

protected:
	/*!
	* Se fija la tasa a aplicar en cada período. En este caso se define rate en el único
	* pago del payoff.
	*/
	virtual void _setAllRates() override;
};

#endif //QCTIMEDEPOSITPAYOFF_H
