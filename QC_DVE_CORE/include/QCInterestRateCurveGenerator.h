#ifndef QCINTERESTRATECURVEGENERATOR_H
#define QCINTERESTRATECURVEGENERATOR_H

#include "QCInterestRateCurve.h"

/*!
* @author Alvaro Díaz
* @brief Clase base abstracta para todos los tipos de generadores de curvas de tasas de interés.
* @details Esta clase define varios métodos que las clases derivadas deben implementar.
* Los distintos tipos de algoritmos de bootstrapping se derivan de ésta.
*/
class QCInterestRateCurveGenerator
{
public:
	/*!
	* Constructor.
	*/
	QCInterestRateCurveGenerator() {};

	/*!
	* Ejecuta el procedimiento que genera la curva.
	*/
	virtual void generateCurve() = 0;

	/*!
	* Calcula el largo de la curva.
	* @return largo de la curva.
	*/
	virtual unsigned int getCurveLength() = 0;

	/*!
	* Devuelve la tasa en la posición i de la curva generada.
	* @param i posición de la tasa deseada.
	* @return valor de la tasa deseada.
	*/
	virtual double getRateAt(unsigned int index) = 0;

	/*!
	* Devuelve la derivada de una tasa generada a partir de uno de los inputs.
	* @param rateIndex índice de la tasa cuya derivada se quiere calcular
	* @param derivativeIndex índice del input que se quiere usar para derivar.
	*/
	virtual double getDerivativeAt(unsigned int rateIndex, unsigned int derivativeIndex) = 0;

	/*!
	* Destructor.
	*/
	virtual ~QCInterestRateCurveGenerator() {};
	
};

#endif //QCINTERESTRATECURVEGENERATOR_H