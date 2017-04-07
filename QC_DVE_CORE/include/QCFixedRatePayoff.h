#ifndef QCFIXEDRATEPAYOFF_H
#define QCFIXEDRATEPAYOFF_H

#include "QCInterestRatePayoff.h"
#include "QCInterestRate.h"

/*!
* @brief QCFixedRatePayoff hereda de QCInterestRatePayoff e implementa la estructura de un bono
* o pata a tasa fija..
*/

class QCFixedRatePayoff :
	public QCInterestRatePayoff
{
public:
	/*!
	* Constructor, se apoya en el constructor de la clase madre.
	* @param fixedRate valor y forma de la tasa fija.
	* @param irLeg estructura de fechas y amortizaciones.
	* @param discountCurve curva de descuento (para la obtención del valor presente)
	* @param valueDate fecha de valorización.
	* @param fixingData se informa un valor NULO. Servirá para implementar patas a tasa
	* fija variable en el tiempo. O sea tasas fijas conocidas desde el inicio de la estructura.
	*/
	QCFixedRatePayoff(QCIntrstRtShrdPtr fixedRate,
		shared_ptr<QCInterestRateLeg> irLeg,
		QCIntRtCrvShrdPtr discountCurve,
		QCDate valueDate,
		QCTimeSeriesShrdPtr fixingData);

	/*!
	* Sobrecarga del operador <. Se ordena por fecha final.
	* @param rhs otro QCFixedRatePayoff para comparar (this < rhs)
	*/
	bool operator<(const QCFixedRatePayoff& rhs);

	/*!
	* Similar al operador < pero actúa sobre shared_ptr<QCFixedRatePayoff>.
	* @param lhs lado izquierdo de la comparación.
	* @param rhs lado derecho de la comparación.
	*/
	static bool lessThan(shared_ptr<QCFixedRatePayoff> lhs, shared_ptr<QCFixedRatePayoff> rhs);

	/*!
	* Devuelve el valor de la tasa fija
	* @return valor de la tasa
	*/
	double getRateValue();

	/*!
	* Destructor
	*/
	virtual ~QCFixedRatePayoff();

protected:
	/*!
	* Se fija la tasa a aplicar en cada período.
	*/
	virtual void _setAllRates() override;

private:

};

#endif //QCFIXEDRATEPAYOFF_H

