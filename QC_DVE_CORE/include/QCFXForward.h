#ifndef QCFXFORWARD_H
#define QCFXFORWARD_H

#include "QCOperation.h"
#include "QCDiscountBondPayoff.h"
#include "asset_classes/QCCurrencyConverter.h"

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
	* Similar al operador < pero act�a sobre shared_ptr<QCFXForward>.
	* @param lhs lado izquierdo de la comparaci�n.
	* @param rhs lado derecho de la comparaci�n.
	*/
	static bool lessThan(shared_ptr<QCFXForward> lhs, shared_ptr<QCFXForward> rhs);

	/*!
	Retorna el �nico per�odo del forward
	*/
	QCInterestRateLeg::QCInterestRatePeriod getPeriod(unsigned int numLeg = 0);

	/*!
	* Retorna la fecha de vencimiento del forward.
	*/
	QCDate getEndDate() const;

	/*!
	* Devuelve uno de los dos payoffs del forward
	* @param numPayoff n�mero del payoff que se quiere observar. Si es mayor a 2 se devuelve el 0.
	*/
	shared_ptr<QCDiscountBondPayoff> getPayoff(unsigned int numPayoff = 0);

	/*!
	* Devuelve la derivada del valor presente respecto al v�rtice index y el payoff numPayoff.
	* El monto de la derivada est� en la misma moneda en que se devuelve el marketValue.
	* @index n�mero del v�rtice de la curva.
	* @numPayoff n�mero del payoff cuya derivada se busca.
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

