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
     * @param [in] firstDate es la fecha más antigua de las dos si se desea retornar un valor positivo
     * @param [in] secondDate es la fecha más reciente de las dos si se desea retornar un valor positivo
     * @return un double con la fracción de año calculada
     */
    double yf(const QCDate& firstDate, const QCDate& secondDate);

    /*!
     * La función countDays devuelve el número de días entre firstDate y secondDate en Act/360.
     * Si se desea un número positivo firstDate debe ser menor que secondDate
     * @param [in] firstDate es la fecha más antigua de las dos si se desea retornar un valor positivo
     * @param [in] secondDate es la fecha más reciente de las dos si se desea retornar un valor positivo
     * @return un long con el número de días calculados
     */
    long countDays(const QCDate& firstDate, const QCDate& secondDate);

private:
    const double _basis = 360.0;
};


#endif //QCACT360_H
