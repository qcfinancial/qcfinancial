//
// Created by Alvaro Diaz on 16-07-16.
//

#ifndef QCACT360_H
#define QCACT360_H


#include "QCYearFraction.h"

/*!
 * @brief La clase QCAct360 implementa el método Act/360.
 * Hereda de la clase abstracta QCYearFraction
 */
class QCAct360 : public QCYearFraction
{
public:
    /*!
     * La función yf devuelve la fracción de año entre dos fechas en convención Act/360.
     * @param firstDate es la fecha más antigua de las dos si se desea retornar un valor positivo
     * @param secondDate es la fecha más reciente de las dos si se desea retornar un valor positivo
     * @return un double con la fracción de año calculada
     */
    double yf(const QCDate& firstDate, const QCDate& secondDate);

	/*!
	* La función yf devuelve un proxy de la fracción de año cuando el argumento es un
	* numero de dias.
	* @param days
	* @return un double con la fracción de año calculada
	*/
	virtual double yf(long days);

    /*!
     * La función countDays devuelve el número de días entre firstDate y secondDate en Act/360.
     * Si se desea un número positivo firstDate debe ser menor que secondDate
     * @param firstDate es la fecha más antigua de las dos si se desea retornar un valor positivo
     * @param secondDate es la fecha más reciente de las dos si se desea retornar un valor positivo
     * @return un long con el número de días calculados
     */
    long countDays(const QCDate& firstDate, const QCDate& secondDate);

private:
	/*!
	* En esta variable se guarda la base de la tasa que, en este caso, es un número fijo.
	*/
    const double _basis = 360.0;
};


#endif //QCACT360_H
