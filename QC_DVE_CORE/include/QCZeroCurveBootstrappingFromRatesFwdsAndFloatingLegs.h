#ifndef QCZEROCURVEGENERATORFROMRATESFWDSANDFLOATINGLEGS_H
#define QCZEROCURVEGENERATORFROMRATESFWDSANDFLOATINGLEGS_H

#include "QCInterestRateCurveGenerator.h"
#include "QCTimeDepositPayoff.h"
#include "QCFloatingRatePayoff.h"
#include "QCDefinitions.h"
#include "QCFXForward.h"

/*!
* @brief Esta clase implementa un bootstrapping de curva cero cupón a partir de tasas cero,
* cotizaciones de forwards y patas flotantes. Se construye la curva de descuento de las patas flotantes
* que debe coincidir con la curva incógnita de los forwards.
*/
class QCZeroCurveBootstrappingFromRatesFwdsAndFloatingLegs :
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
	* @param inputFixedRateLegs vector<shared_ptr<QCFloatingRatePayoff>> que representan las patas flotantes de los
	* swaps que se usan como input del bootstrapping.
	* @param zeroCurve es un puntero a un objeto de tipo QCZeroCouponCurve que contiene los valores de curva
	* que se van a construir. La curva debe venir inicializada con las dimensiones apropiadas.
	*/
	QCZeroCurveBootstrappingFromRatesFwdsAndFloatingLegs(QCDate valueDate,
		vector<shared_ptr<QCTimeDepositPayoff>> inputRates,
		vector<shared_ptr<QCFXForward>> inputForwards,
		unsigned int whichForwardLeg,
		vector<shared_ptr<QCFloatingRatePayoff>> inputFloatingRateLegs,
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
	virtual unsigned int getCurveLength();

	/*!
	* Devuelve la tasa en la posición i de la curva generada.
	* @param i posición de la tasa deseada.
	* @return valor de la tasa deseada.
	*/
	virtual double getRateAt(unsigned int index);

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
	virtual double getDerivativeAt(unsigned int rateIndex, unsigned int derivativeIndex);

	virtual ~QCZeroCurveBootstrappingFromRatesFwdsAndFloatingLegs();

protected:
	unsigned int _whichForwardLeg;
};

#endif //QCZEROCURVEGENERATORFROMRATESFWDSANDFLOATINGLEGS_H
