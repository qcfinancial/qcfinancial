#ifndef QCFXFORWARD_H
#define QCFXFORWARD_H

#include "QCOperation.h"
#include "QCDiscountBondPayoff.h"
#include "QCCurrencyConverter.h"

class QCFXForward : public QCOperation
{
public:
	QCFXForward(vector<shared_ptr<QCDiscountBondPayoff>> legs,
		QCCurrencyConverter::QCCurrencyEnum marketValueCurrency,
		shared_ptr<map<QCCurrencyConverter::QCFxRateEnum, double>> fxRates);

	virtual double marketValue() override;

	/*!
	* Ordena por fecha de vencimiento.
	*/
	bool operator<(const QCFXForward& other);

	/*!
	* Similar al operador < pero actúa sobre shared_ptr<QCFXForward>.
	* @param lhs lado izquierdo de la comparación.
	* @param rhs lado derecho de la comparación.
	*/
	static bool lessThan(shared_ptr<QCFXForward> lhs, shared_ptr<QCFXForward> rhs);

	/*!
	Retorna el único período del forward
	*/
	QCInterestRateLeg::QCInterestRatePeriod getPeriod(unsigned int numLeg = 0);

	/*!
	* Retorna la fecha de vencimiento del forward.
	*/
	QCDate getEndDate() const;

	/*!
	* Devuelve uno de los dos payoffs del forward
	* @param numPayoff número del payoff que se quiere observar. Si es mayor a 2 se devuelve el 0.
	*/
	shared_ptr<QCDiscountBondPayoff> getPayoff(unsigned int numPayoff = 0);

	/*!
	* Devuelve la derivada del valor presente respecto al vértice index y el payoff numPayoff.
	* El monto de la derivada está en la misma moneda en que se devuelve el marketValue.
	* @index número del vértice de la curva.
	* @numPayoff número del payoff cuya derivada se busca.
	*/
	double getPvRateDerivativeAtFromLeg(unsigned int index, unsigned int numPayoff);

	/*!
	* Destructor.
	*/
	virtual ~QCFXForward();

protected:
	vector<shared_ptr<QCDiscountBondPayoff>> _legs;
	QCCurrencyConverter::QCCurrencyEnum _marketValueCurrency;
	shared_ptr<map<QCCurrencyConverter::QCFxRateEnum, double>> _fxRates;

	QCDate _endDate;

};

#endif //QCFXFORWARD_H

