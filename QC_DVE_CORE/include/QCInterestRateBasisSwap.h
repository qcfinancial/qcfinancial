#ifndef QCINTERESTRATEBASISSWAP_H
#define QCINTERESTRATEBASISSWAP_H

#include "QCOperation.h"
#include "QCFloatingRatePayoff.h"

class QCInterestRateBasisSwap : public QCOperation
{
public:
	/*!
	* Constructor
	*/
	QCInterestRateBasisSwap(vector<shared_ptr<QCFloatingRatePayoff>> basisSwaps);
	
	/*!
	* Retorna el valor de mercado de la operación.
	*/
	virtual double marketValue() override;

	/*!
	* Ordena por fecha de vencimiento.
	*/
	bool operator<(const QCInterestRateBasisSwap& other);

	/*!
	* Devuelve la menor fecha de vencimiento considerando las dos patas.
	*/
	QCDate getStartDate() const;

	/*!
	* Devuelve la mayor fecha de vencimiento considerando las dos patas.
	*/
	QCDate getEndDate() const;

	/*!
	* Similar al operador < pero actúa sobre shared_ptr<QCInterestRateBasisSwap>.
	* @param lhs lado izquierdo de la comparación.
	* @param rhs lado derecho de la comparación.
	*/
	static bool lessThan(shared_ptr<QCInterestRateBasisSwap> lhs, shared_ptr<QCInterestRateBasisSwap> rhs);

	/*!
	* Devuelve uno de los dos payoffs del forward
	* @param numPayoff número del payoff que se quiere observar. Si es mayor a 2 se devuelve el 0.
	*/
	shared_ptr<QCFloatingRatePayoff> getPayoff(unsigned int numPayoff = 0);

	/*!
	* Devuelve la derivada del valor presente respecto al vértice index de la curva de descuento
	* y el payoff numPayoff.
	* El monto de la derivada está en la misma moneda en que se devuelve el marketValue.
	* @index número del vértice de la curva.
	* @numPayoff número del payoff cuya derivada se busca.
	*/
	double getPvRateDerivativeAtFromLeg(unsigned int index, unsigned int numPayoff);

	/*!
	* Devuelve la derivada del valor presente respecto al vértice index de la curva de proyeccion
	* y el payoff numPayoff.
	* El monto de la derivada está en la misma moneda en que se devuelve el marketValue.
	* @index número del vértice de la curva.
	* @numPayoff número del payoff cuya derivada se busca.
	*/
	double getProjRateDerivativeAtFromLeg(unsigned int index, unsigned int numPayoff);

	/*!
	* Destructor.
	*/
	virtual ~QCInterestRateBasisSwap();

private:
	vector<shared_ptr<QCFloatingRatePayoff>> _basisSwaps;

};

#endif //QCINTERESTRATEBASISSWAP_H