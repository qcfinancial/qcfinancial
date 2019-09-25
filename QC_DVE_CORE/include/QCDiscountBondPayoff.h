#ifndef QCDISCOUNTBONDPAYOFF_H
#define QCDISCOUNTBONDPAYOFF_H

#include "QCInterestRatePayoff.h"
#include "asset_classes/QCInterestRate.h"
#include "asset_classes/QCCurrencyConverter.h"

/*!
* @brief QCDiscountBondPayoff hereda de QCInterestRatePayoff e implementa la estructura de un bono
* a descuento, como un PDBC, T-Bill o pata de un fx forward.
*/

class QCDiscountBondPayoff : public QCInterestRatePayoff
{
public:
	/*!
	* Constructor, se apoya en el constructor de la clase madre.
	* @param fixedRate valor y forma de la tasa fija.
	* @param irLeg estructura de fechas y amortizaciones. En este caso, irLeg debe consistir
	* de un �nico per�odo con amortizaci�n bullet que constituye flujo.
	* @param discountCurve curva de descuento (para la obtenci�n del valor presente)
	* @param valueDate fecha de valorizaci�n.
	*/
	QCDiscountBondPayoff(shared_ptr<QCInterestRateLeg> irLeg,
		QCIntRtCrvShrdPtr discountCurve,
		QCDate valueDate,
		QCTimeSeriesShrdPtr fxFixingData,
		QCCurrencyConverter::QCCurrencyEnum notionalCurrency,
		QCCurrencyConverter::QCFxRateEnum fxRate
		);

	/*!
	* Este override de presentValue() llama el m�todo de la clase madre y luego ajusta el valor
	* del cashflow en value date transform�ndolo a la moneda de liquidaci�n.
	*/
	double presentValue();

	/*!
	* Retorna la moneda del nocional
	* @return moneda del nocional
	*/
	QCCurrencyConverter::QCCurrencyEnum getNotionalCurrency();

	/*!
	* Entrega el per�odo de tasa de inter�s
	* @return �nico QCInterestRatePeriod de este payoff.
	*/
	QCInterestRateLeg::QCInterestRatePeriod getPeriod() const;

	/*!
	* Destructor
	*/
	virtual ~QCDiscountBondPayoff();

protected:
	/*!
	* Se fija la tasa a aplicar en cada per�odo. En este caso se define 0 en el �nico
	* pago del payoff.
	*/
	virtual void _setAllRates() override;

	/*!
	* Variable donde se almacena la moneda del nocional de la pata
	*/
	QCCurrencyConverter::QCCurrencyEnum _notionalCurrency;

	/*!
	* Variable donde se almacena el tipo de fx rate que se usa para convertir.
	* Con tipo nos referimos a la descripcion MX1MX2 donde MXi es el c�digo de una divisa.
	*/
	QCCurrencyConverter::QCFxRateEnum _fxRate;
};

#endif //QCDISCOUNTBONDPAYOFF_H
