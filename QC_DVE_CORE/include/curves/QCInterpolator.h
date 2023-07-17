#ifndef QCINTERPOLATOR_H
#define QCINTERPOLATOR_H

#include "curves/QCCurve.h"
#include <memory>

//Esta es una clase base abstracta.

class QCInterpolator
{
public:
	/*!
	* Enumera los distintos tipos de interpolación de tasas de interés.
	*/
	enum QCTypeInterpolator
	{
		qcLinearInterpolator,	/*!< Interpolación lineal. Se usa con curvas de tasas.*/
		qcLogLinearInterpolator /*!< Interpolación loglineal. Se usa con curvas de factores de descuento.*/
	};

	/*!
	* Constructor por default.
	*/
	QCInterpolator();

	/*!
	* Constructor.
	* @param curve En este objeto de tipo QCCurve vienen los pares de abscisas y ordenadas sobre los cuales
	* se desea realizar interpolaciones. Notar que el template QCCurve está especificado con abscisa
	* que es un entero relativo (long).
	*/
	QCInterpolator(shared_ptr<QCCurve<long>> curve);

	/*!
	* Este método borra abscisas y ordenadas de la curva y vuelve a definir el tama�o de los
	* vectores de abscisa y ordenadas.
	* @param newSize nuevo tama�o de la curva
	*/
	void reset(unsigned long newSize);

	/*!
	* Este método actualiza el valor de un par ordenado de la curva. Si la abscisa corresponde
	* a un par ya existente, entonces sobre escribe los valores existentes. Si la abscisa no existe,
	* se inserta un nuevo par en la posici�n que corresponda (ordenado por abscisa ascendente).
	* @param x abscisa
	* @param y ordenada
	*/
	void setPair(long x, double y);

	/*!
	* M�todo que actualiza el valor de la ordenada en una posici�n dada.
	* @param pos
	* @param value
	*/
	void setOrdinateAtWithValue(unsigned long position, double value);

	/*!
	* M�todo que actualiza el valor de la abscisa en una posici�n dada.
	* @param pos
	* @param value
	*/
	void setAbscissaAtWithValue(unsigned long position, long value);

	/*!
	* Aplica el método de interpolación implementado. Además calcula las derivadas de la tasa
	* interpolada, respecto a todos las tasas de la curva. El resultado del cálculo de derivadas
	* se almacena internamente y puede consultarse con el método rateDerivativeAt(unsigned long rateIndex).
	* @param value abscisa en la que se debe interpolar.
	* @return valor interpolado.
	*/
	virtual double interpolateAt(long value) = 0;

	/*!
	* Retorna la derivada del valor interpolado con el m�todo implementado respecto al valor de interpolaci�n.
	* @param value abscissa en la que se debe interpolar y calcular la derivada.
	* @return valor de la derivada.
	*/
	virtual double derivativeAt(long value) = 0;

	/*!
	* Retorna la segunda derivada del valor interpolado con el m�todo implementado respecto
	* al valor de interpolaci�n.
	* @param value abscissa en la que se debe interpolar y calcular la segunda derivada.
	* @return valor de la segunda derivada.
	*/
	virtual double secondDerivativeAt(long value) = 0;

	/*!
	* Retorna el valor de la derivada del �ltimo valor interpolado, respecto a la tasa de la 
	* curva que est� en la posici�n rateIndex.
	* @param �ndice de la tasa cuya derivada se quiere conocer.
	* @return valor de la derivada.
	*/
	double rateDerivativeAt(unsigned long rateIndex);

	/*!
	* Retorna el largo (n�mero de elementos) de la curva.
	* @return largo de la curva.
	*/
	size_t getLength();

	/*!
	* Retorna el par plazo tasa de la curva en una posici�n.
	* @param position �ndice de la posici�n buscada.
	* @return par en la posici�n buscada.
	*/
	pair<long, double> getValuesAt(size_t position);

	/*!
	* Destructor.
	*/
	virtual ~QCInterpolator(void);

protected:
	/*!
	* Se mantiene una referencia al objeto QCCurve sobre el cual se quiere interpolar.
	*/
	shared_ptr<QCCurve<long>> _curve;

	/*!
	* Funci�n auxiliar que todo algoritmo de interpolaci�n va a necesitar.
	*/
	long _index(long arg);

	/*!
	* Se almacenan las derivadas de la tasa interpolada respecto a las tasas de la curva.
	*/
	vector<double> _derivatives;
};

#endif //QCINTERPOLATOR_H


