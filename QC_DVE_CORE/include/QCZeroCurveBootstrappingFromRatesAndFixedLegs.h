#ifndef QCZEROCURVEBOOTSTRAPPINGFROMRATESANDFIXEDLEGS_H
#define QCZEROCURVEBOOTSTRAPPINGFROMRATESANDFIXEDLEGS_H

#include <memory>

#include "QCInterestRateCurveGenerator.h"
#include "QCZeroCouponInterestRateCurve.h"
#include "QCTimeDepositPayoff.h"
#include "QCFixedRatePayoff.h"
#include "QCInterpolator.h"
#include "QCInterestRate.h"
#include "QCDefinitions.h"

/*!
* @author Alvaro Díaz
* @brief Clase base abstracta para todos los tipos de generadores de curvas de tasas de interés.
* @details Esta clase define varios métodos que las clases derivadas deben implementar.
* Los distintos tipos de algoritmos de bootstrapping se derivan de ésta.
*/

using namespace std;

class QCZeroCurveBootstrappingFromRatesAndFixedLegs : public QCInterestRateCurveGenerator
{
public:
	/*!
	* Constructor.
	* @param valueDate fecha a la cual se va a construir la curva.
	* @param inputRates vector<shared_ptr<QCTimeDepositPayoff>> que representan las tasas cero (depósitos
	* o Libor que se utilizan como datos inciales en el proceso.
	* @param inputFixedRateLegs vector<shared_ptr<QCInterestRatePayoff>> que representan las patas fijas de los
	* swaps que se usan como input del bootstrapping.
	* @param zeroCurve es un puntero a un objeto de tipo QCZeroCouponCurve que contiene los valores de curva
	* que se van a construir. La curva debe venir inicializada con las dimensiones apropiadas.
	*/
	QCZeroCurveBootstrappingFromRatesAndFixedLegs(
		QCDate valueDate,
		vector<shared_ptr<QCTimeDepositPayoff>> inputRates,
		vector<shared_ptr<QCFixedRatePayoff>> inputFixedRateLegs,
		QCZrCpnCrvShrdPtr zeroCurve);

	/*!
	* Ejecuta el procedimiento que genera la curva.
	*/
	virtual void generateCurve() override;

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
	virtual double getRateAt(unsigned int index) override;

	/*!
	* Devuelve la derivada de una tasa generada a partir de uno de los inputs.
	* @param rateIndex índice de la tasa cuya derivada se quiere calcular
	* @param derivativeIndex índice del input que se quiere usar para derivar.
	*/
	virtual double getDerivativeAt(unsigned int rateIndex, unsigned int derivativeIndex) override;

	/*!
	* Destructor
	*/
	virtual ~QCZeroCurveBootstrappingFromRatesAndFixedLegs() override;

private:
	QCDate _valueDate;
	vector<shared_ptr<QCTimeDepositPayoff>> _inputRates;
	vector<shared_ptr<QCFixedRatePayoff>> _inputFixedRateLegs;
	QCZrCpnCrvShrdPtr _curve;
	
	/*!
	* Método privado que verifica si el parámetro inputRates pasado al constructor
	* es admisible.
	* @return verdadero o falso
	*/
	bool _checkInputRates();

	/*!
	* Eventual mensaje de error que genera _checkInputRates()
	*/
	string _checkInputRatesMsg;

	/*!
	* Método privado que verifica si el parámetro inputFixedRateLegs pasado al constructor
	* es admisible.
	* @return verdadero o falso
	*/
	bool _checkInputFixedRateLegs();

	/*!
	* Eventual mensaje de error que genera _checkInputRates()
	*/
	string _checkInputFixedRateLegsMsg;
};

#endif //QCZEROCURVEBOOTSTRAPPINGFROMRATESANDFIXEDLEGS_H