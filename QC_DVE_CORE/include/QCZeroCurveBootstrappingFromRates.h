#ifndef QCZEROCURVEBOOTSTRAPPINGFROMRATESANDRATES_H
#define QCZEROCURVEBOOTSTRAPPINGFROMRATESANDRATES_H

#include <memory>

#include "curves/QCInterestRateCurveGenerator.h"
#include "curves/QCZeroCouponInterestRateCurve.h"
#include "QCTimeDepositPayoff.h"
#include "QCFixedRatePayoff.h"
#include "curves/QCInterpolator.h"
#include "asset_classes/QCInterestRate.h"
#include "QCDefinitions.h"

/*!
* @author Alvaro D�az
* @brief Clase base abstracta para todos los tipos de generadores de curvas de tasas de inter�s.
* @details Esta clase define varios m�todos que las clases derivadas deben implementar.
* Los distintos tipos de algoritmos de bootstrapping se derivan de �sta.
*/

using namespace std;

class QCZeroCurveBootstrappingFromRates : public QCInterestRateCurveGenerator
{
public:
	/*!
	* Constructor.
	* @param valueDate fecha a la cual se va a construir la curva.
	* @param inputRates vector<shared_ptr<QCTimeDepositPayoff>> que representan las tasas cero (dep�sitos
	* o Libor que se utilizan como datos inciales en el proceso.
	* @param inputFixedRateLegs vector<shared_ptr<QCInterestRatePayoff>> que representan las patas fijas de los
	* swaps que se usan como input del bootstrapping.
	* @param zeroCurve es un puntero a un objeto de tipo QCZeroCouponCurve que contiene los valores de curva
	* que se van a construir. La curva debe venir inicializada con las dimensiones apropiadas.
	*/
	QCZeroCurveBootstrappingFromRates(QCDate valueDate,
		vector<shared_ptr<QCTimeDepositPayoff>> inputRates,
		QCZrCpnCrvShrdPtr zeroCurve);

	/*!
	* Ejecuta el procedimiento que genera la curva.
	*/
	virtual void generateCurve() override;

	/*!
	* Ejecuta el procedimiento que genera la curva y calcula todas las derivadas.
	*/
	virtual void generateCurveAndDerivatives() override;

	/*!
	* Calcula el largo de la curva.
	* @return largo de la curva.
	*/
	virtual size_t getCurveLength() override;

	/*!
	* Devuelve la tasa en la posici�n i de la curva generada.
	* @param i posici�n de la tasa deseada.
	* @return valor de la tasa deseada.
	*/
	virtual double getRateAt(size_t index) override;

	/*!
	* Calculada la derivada de una tasa generada a partir de uno de los inputs.
	* @param rateIndex �ndice de la tasa cuya derivada se quiere calcular
	* @param derivativeIndex �ndice del input que se quiere usar para derivar.
	*/
	virtual void calculateDerivativesAt(size_t derivativeIndex) override;

	/*!
	* Devuelve la derivada de una tasa generada a partir de uno de los inputs.
	* @param rateIndex �ndice de la tasa cuya derivada se quiere calcular
	* @param derivativeIndex �ndice del input que se quiere usar para derivar.
	*/
	virtual double getDerivativeAt(size_t rateIndex, size_t derivativeIndex) override;

	/*!
	* Destructor
	*/
	virtual ~QCZeroCurveBootstrappingFromRates() override;

};

#endif //QCZEROCURVEBOOTSTRAPPINGFROMRATESANDRATES_H