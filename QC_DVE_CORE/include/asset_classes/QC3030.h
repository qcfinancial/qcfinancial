//
// Created by Alvaro Diaz on 16-07-16.
//


#ifndef QC3030_H
#define QC3030_H


#include "QCYearFraction.h"

/*!
* @brief La clase QC30360 implementa el método 30/360.
* Hereda de la clase abstracta QCYearFraction
* @author Alvaro Díaz
*/
class QC3030 : public QCYearFraction
{
public:
    QC3030() = default;
	/*!
	* La funci�n yf devuelve la fracción de a�o entre dos fechas en convención 30/360.
	* @param firstDate es la fecha más antigua de las dos si se desea retornar un valor positivo
	* @param secondDate es la fecha más reciente de las dos si se desea retornar un valor positivo
	* @return un double con la fracción de año calculada
	*/
	double yf(const QCDate& firstDate, const QCDate& secondDate);

	/*!
	* La funci�n countDays devuelve el número de días entre firstDate y secondDate en 30/360.
	* Si se desea un número positivo firstDate debe ser menor que secondDate
	* @param firstDate es la fecha más antigua de las dos si se desea retornar un valor positivo
	* @param secondDate es la fecha más reciente de las dos si se desea retornar un valor positivo
	* @return un long con el número de días calculados
	*/
	long countDays(const QCDate& firstDate, const QCDate& secondDate);

	/*!
	* La función yf devuelve un proxy de la fracción de año cuando el argumento es un
	* número de dias.
	* @param days corresponde a un número de dias suponiendo que este número ya está bien calculado
	* @return un double con la fracción de año calculada
	*/
	virtual double yf(long days);

	/**
	* @fn	std::string QCAct360::description();
	*
	* @brief	Gets the description. In this case "30360"
	*
	* @author	Alvaro Díaz V.
	* @date	28/09/2017
	*
	* @return	A std::string.
	*/
	std::string description();

private:
	const double _basis = 30.0;
};


#endif //QC3030_H
