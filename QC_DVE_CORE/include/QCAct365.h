//
// Created by Alvaro Diaz on 16-07-16.
//

#ifndef QCACT365_H
#define QCACT365_H

#include "QCYearFraction.h"

/*!
 * @brief La clase QCAct360 implementa el método Act/365.
 * Hereda de la clase abstracta QCYearFraction
 */

class QCAct365 : public QCYearFraction
{
public:
    /*!
     * La función yf devuelve la fracción de año entre dos fechas en convención Act/365.
     * @param [in] firstDate es la fecha más antigua de las dos si se desea retornar un valor positivo
     * @param [in] secondDate es la fecha más reciente de las dos si se desea retornar un valor positivo
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
     * La función countDays devuelve el número de días entre firstDate y secondDate en Act/365.
     * Si se desea un número positivo firstDate debe ser menor que secondDate
     * @param [in] firstDate es la fecha más antigua de las dos si se desea retornar un valor positivo
     * @param [in] secondDate es la fecha más reciente de las dos si se desea retornar un valor positivo
     * @return un long con el número de días calculados
     */
    long countDays(const QCDate& firstDate, const QCDate& secondDate);

private:
    const double _basis = 365.0;
};


#endif //QCACT365_H
