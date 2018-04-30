#ifndef QCZEROCURVEGENERATORFROMRATESFWDSANDFIXEDLEGS_H
#define QCZEROCURVEGENERATORFROMRATESFWDSANDFIXEDLEGS_H

#include "QCInterestRateCurveGenerator.h"
#include "QCTimeDepositPayoff.h"
#include "QCFixedRatePayoff.h"
#include "QCDefinitions.h"
#include "QCFXForward.h"

class QCZeroCurveBootstrappingFromRatesFwdsAndFixedLegs :
	public QCInterestRateCurveGenerator
{
public:

	/*!
	* Constructor.
	* @param valueDate fecha a la cual se va a construir la curva.
	* @param inputRates vector<shared_ptr<QCTimeDepositPayoff>> que representan las tasas cero
	* (depósitos o Libor) que se utilizan como datos inciales en el proceso.
	* @param inputForwards vector<shared_ptr<QCFXForward>> que representa las cotizaciones que se
	* usan como ipnut en el bootstrapping
	* @param inputFixedRateLegs vector<shared_ptr<QCInterestRatePayoff>> que representan las patas fijas de los
	* swaps que se usan como input del bootstrapping.
	* @param zeroCurve es un puntero a un objeto de tipo QCZeroCouponCurve que contiene los valores de curva
	* que se van a construir. La curva debe venir inicializada con las dimensiones apropiadas.
	*/
	QCZeroCurveBootstrappingFromRatesFwdsAndFixedLegs(QCDate valueDate,
		vector<shared_ptr<QCTimeDepositPayoff>> inputRates,
		vector<shared_ptr<QCFXForward>> inputForwards,
		unsigned int whichForwardLeg,
		vector<shared_ptr<QCFixedRatePayoff>> inputFixedRateLegs,
		QCZrCpnCrvShrdPtr newZeroCurve);

	/*!
	* Ejecuta el procedimiento que genera la curva.
	*/
	virtual void generateCurve();

	/*!
	* Ejecuta el procedimiento que genera la curva y calcula todas las derivadas.
	*/
	virtual void generateCurveAndDerivatives() override;

	/*!
	* Calcula el largo de la curva.
	* @return largo de la curva.
	*/
	virtual unsigned int getCurveLength() override;

	/*!
	* Devuelve la tasa en la posición i de la curva generada.
	* @param i posición de la tasa deseada.
	* @return valor de la tasa deseada.
	*/
	virtual double getRateAt(size_t index) override;

	/*!
	* Calculada la derivada de una tasa generada a partir de uno de los inputs.
	* @param rateIndex índice de la tasa cuya derivada se quiere calcular
	* @param derivativeIndex índice del input que se quiere usar para derivar.
	*/
	virtual void calculateDerivativesAt(size_t derivativeIndex) override;

	/*!
	* Devuelve la derivada de una tasa generada a partir de uno de los inputs.
	* @param rateIndex índice de la tasa cuya derivada se quiere calcular
	* @param derivativeIndex índice del input que se quiere usar para derivar.
	*/
	virtual double getDerivativeAt(size_t rateIndex, size_t derivativeIndex) override;

	virtual ~QCZeroCurveBootstrappingFromRatesFwdsAndFixedLegs();

protected:
	unsigned int _whichForwardLeg;
};

#endif //QCZEROCURVEGENERATORFROMRATESFWDSANDFIXEDLEGS_H
