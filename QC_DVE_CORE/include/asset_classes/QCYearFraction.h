#ifndef QCYEARFRACTION_H
#define QCYEARFRACTION_H

#include <string>

#include "time/QCDate.h"

/*!
* @author Alvaro Díaz
* @brief La clase QCYearFraction es una clase base. Al subclasearla se implementa
* un cálculo de fracción de año específico como Act/365 o 30/360.
* Esta clase no se debe instanciar ya que todos sus métodos retornan cero.
*/

class QCYearFraction
{
public:
    /*!
     * La función yf devuelve la fracción de año entre dos fechas.
     * @param firstDate es la fecha más antigua de las dos si se desea retornar un valor positivo
     * @param secondDate es la fecha más reciente de las dos si se desea retornar un valor positivo
     * @return (double) con la fracción de año calculada
     */
	virtual double yf(const QCDate& firstDate, const QCDate& secondDate)
	{
		return 0.0;
	};

    /*!
     * La función countDays devuelve el número de días entre firstDate y secondDate.
     * Si se desea un número positivo firstDate debe ser menor que secondDate
     * @param firstDate es la fecha más antigua de las dos si se desea retornar un valor positivo
     * @param secondDate es la fecha más reciente de las dos si se desea retornar un valor positivo
     * @return (long) con el número de días calculados
     */
	virtual long countDays(const QCDate& firstDate, const QCDate& secondDate)
	{
		return 0;
	};
	/*!
	* Esta sobrecarga de la función yf devuelve un proxy de la fracción de año cuando el argumento es un
	* numero de dias.
	* @param days
	* @return (double) con la fracción de año calculada
	*/
	virtual double yf(long days)
	{
		return 0.0;
	};

	/**
	 * @fn	virtual std::string QCYearFraction::description()
	 *
	 * @brief	Gets the description ("Act360", "30360" ...)
	 *
	 * @author	Alvaro Díaz V.
	 * @date	28/09/2017
	 *
	 * @return	A std::string.
	 */
	virtual std::string description()
	{
		return "";
	}

    static long countDays30360(const QCDate &firstDate, const QCDate &secondDate)
    {
        int firstDay = firstDate.day();
        int firstMonth = firstDate.month();
        int firstYear = firstDate.year();

        int secondDay = secondDate.day();
        int secondMonth = secondDate.month();
        int secondYear = secondDate.year();

        if (firstDay == 31)
            firstDay = 30;

        if (secondDay == 31 && firstDay == 30)
            secondDay = 30;

        long result = (secondDay - firstDay) + 30 * (secondMonth - firstMonth)
                      + 360 * (secondYear - firstYear);
        return result;
    }

    static long countDaysAct(const QCDate &firstDate, const QCDate &secondDate)
    {
        return firstDate.dayDiff(secondDate);
    }
};


#endif //QCYEARFRACTION_H
