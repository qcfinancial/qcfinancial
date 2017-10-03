#ifndef QWEALTHFACTOR_H
#define QWEALTHFACTOR_H

#include<string>

/*!
* @author Alvaro Díaz
* @brief La clase QCWealthFactor es una clase base. Al subclasearla se implementa
* un cálculo de factor de capitalización específico como compuesto o lineal.
* Esta clase es virtual pura.
*/

class QCWealthFactor
{
public:
	/*!
	* Constructor por default de la clase
	*/
	QCWealthFactor();

	/*!
	* La función wf devuelve el valor del factor de capitalización dado el valor de una tasa y
	* su fracción de año asociada. Cuando se invoca este método, también se calcula la derivada
	* del factor de capitalización respecto al parámetro rate. Este valor se almacena en la variable
	* protected _dwf
	* @param rate es el valor de la tasa
	* @param yf es el valor de la fracción de año
	* @return (double) valor del factor de capitalización
	*/
	virtual double wf(double rate, double yf);

	/*!
	* La función rate devuelve el valor de la tasa asociada al valor del factor de capitalización
	* y su fracción de año asociada.
	* @param wf es el valor del factor de capitalización
	* @param yf es el valor de la fracción de año
	* @return (double) valor de la tasa asociada
	*/
	virtual double rate(double wf, double yf);

	/*!
	* La función dwf es un getter que devuelve el valor de la derivada del último factor de capitalización
	* calculado, respecto a la tasa. El cálculo de la derivada se realiza al invocar el método wf.
	* @return (double) valor de la derivada respecto a la tasa
	*/
	double dwf();

	/**
	 * @fn	virtual std::string QCWealthFactor::description();
	 *
	 * @brief	Gets the description. "Com", "Lin", ...
	 *
	 * @author	Alvaro Díaz V.
	 * @date	28/09/2017
	 *
	 * @return	A std::string.
	 */
	virtual std::string description();

	/*!
	* Destructor de la clase
	*/
	virtual ~QCWealthFactor();

protected:
	/*!
	* Variable protected donde se almacena el valor de la derivada del último factor de
	* capitalización calculado.
	*/
	double _dwf;
};

#endif //QWEALTHFACTOR_H

