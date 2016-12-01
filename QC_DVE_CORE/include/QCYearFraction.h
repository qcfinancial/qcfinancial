//
// Created by Alvaro Diaz on 16-07-16.
//
#ifndef QCYEARFRACTION_H
#define QCYEARFRACTION_H

class QCDate;

/*!
 * @brief La clase QCYearFraction es una clase base. Al subclasearla se implementa
 * un cálculo de fracción de año específico como Act/365 o 30/360.
 * Esta clase no se debe instanciar ya que los sus metodos retornan cero.
 */
class QCYearFraction
{
public:
    /*!
     * La función yf devuelve la fracción de año entre dos fechas.
     * @param firstDate es la fecha más antigua de las dos si se desea retornar un valor positivo
     * @param secondDate es la fecha más reciente de las dos si se desea retornar un valor positivo
     * @return un double con la fracción de año calculada
     */
	virtual double yf(const QCDate& firstDate, const QCDate& secondDate) { return 0.0; };
    /*!
     * La función countDays devuelve el número de días entre firstDate y secondDate.
     * Si se desea un número positivo firstDate debe ser menor que secondDate
     * @param firstDate es la fecha más antigua de las dos si se desea retornar un valor positivo
     * @param secondDate es la fecha más reciente de las dos si se desea retornar un valor positivo
     * @return un long con el número de días calculados
     */
	virtual long countDays(const QCDate& firstDate, const QCDate& secondDate) { return 0; };
};


#endif //QCYEARFRACTION_H
