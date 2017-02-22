//
// Created by Alvaro Diaz on 16-07-16.
//

#ifndef QCACTACT_H
#define QCACTACT_H


#include "QCYearFraction.h"

/*!
 * @brief La clase QCActAct implementa el método Act/Act.
 * Hereda de la clase abstracta QCYearFraction
 */

class QCActAct : public QCYearFraction
{
public:
    /*!
     * La función yf devuelve la fracción de año entre dos fechas en convención Act/Act.
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
     * La función countDays devuelve el número de días entre firstDate y secondDate en Act/Act.
     * Si se desea un número positivo firstDate debe ser menor que secondDate
     * @param firstDate es la fecha más antigua de las dos si se desea retornar un valor positivo
     * @param secondDate es la fecha más reciente de las dos si se desea retornar un valor positivo
     * @return un long con el número de días calculados
     */
    long countDays(const QCDate& firstDate, const QCDate& secondDate);
};

#endif //QCACTACT_H
