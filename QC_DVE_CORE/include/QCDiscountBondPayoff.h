#ifndef QCDISCOUNTBONDPAYOFF_H
#define QCDISCOUNTBONDPAYOFF_H

#include "QCInterestRatePayoff.h"
#include "QCInterestRate.h"
#include "QCCurrencyConverter.h"

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
	* de un único período con amortización bullet que constituye flujo.
	* @param discountCurve curva de descuento (para la obtención del valor presente)
	* @param valueDate fecha de valorización.
	*/
	QCDiscountBondPayoff(shared_ptr<QCInterestRateLeg> irLeg,
		QCIntRtCrvShrdPtr discountCurve,
		QCDate valueDate,
		QCTimeSeriesShrdPtr fxFixingData,
		QCCurrencyConverter::QCCurrency notionalCurrency,
		QCCurrencyConverter::QCFxRate fxRate
		);

	/*!
	* Este override de presentValue() llama el método de la clase madre y luego ajusta el valor
	* del cashflow en value date transformándolo a la moneda de liquidación.
	*/
	double presentValue();

	/*!
	* Retorna la moneda del nocional
	* @return moneda del nocional
	*/
	QCCurrencyConverter::QCCurrency getNotionalCurrency();

	/*!
	* Entrega el período de tasa de interés
	* @return único QCInterestRatePeriod de este payoff.
	*/
	QCInterestRateLeg::QCInterestRatePeriod getPeriod() const;

	/*!
	* Destructor
	*/
	virtual ~QCDiscountBondPayoff();

protected:
	/*!
	* Se fija la tasa a aplicar en cada período. En este caso se define 0 en el único
	* pago del payoff.
	*/
	virtual void _setAllRates() override;

	/*!
	* Variable donde se almacena la moneda del nocional de la pata
	*/
	QCCurrencyConverter::QCCurrency _notionalCurrency;

	/*!
	* Variable donde se almacena el tipo de fx rate que se usa para convertir.
	* Con tipo nos referimos a la descripcion MX1MX2 donde MXi es el código de una divisa.
	*/
	QCCurrencyConverter::QCFxRate _fxRate;
};

#endif //QCDISCOUNTBONDPAYOFF_H
