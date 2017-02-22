#ifndef QCINTERESTRATECURVE_H
#define QCINTERESTRATECURVE_H

#include "QCInterestRate.h"
#include "QCInterpolator.h"
#include "QCDefinitions.h"

/*!
* @author Alvaro Díaz
* @brief Clase base abstracta para todas las curvas de tasas de interés.
* @details Esta clase define varios métodos que las clases derivadas deben implementar.
* Los distintos tipos de curvas (descuento, proyección, por factores o tasas)
* se derivan de ésta.
*/
class QCInterestRateCurve
{
public:
	/*!
	* Enumera los distintos tipos de curvas de tasas de interés.
	*/
	enum QCTypeInterestRateCurve
	{
		qcProjectingCurve,	/*!< Curva de proyección. Al interpolar se obtiene una tasa forward de un
							tenor predefinido.
							*/
		qcZeroCouponCurve,	/*!< Curva cero cupón. Se especifica entregando los plazos en días
							y valores de las tasas.
							*/
		qcDiscountFactorCurve /*!< Curva de factores de descuento. Se especifica entregando los plazos 
							  en días y los valores de los factores de descuento.
							  */
	};

	/*!
	* Constructor de la clase.
	* @param curve plazos y valores (tasas o factores de descuento) de la curva junto con el
	* método de interpolación seleccionado.
	* @param intRate contiene el tipo de las tasas de interés de la curva.
	*/
	QCInterestRateCurve(shared_ptr<QCInterpolator> curve,
		QCInterestRate intRate) : _curve(curve), _intRate(intRate)
	{
	}
	/*!
	* Retorna la tasa interpolada al plazo d.
	* @param d plazo a interpolar
	* @return valor de la tasa interpolada en la convención de las tasas de la curva.
	*/
	virtual double getRateAt(long d) = 0;

	/*!
	* Retorna el factor de descuento interpolado al plazo d.
	* @param d plazo a interpolar
	* @return valor del factor de descuento interpolado.
	*/
	virtual double getDiscountFactorAt(long d) = 0;

	/*!
	* Retorna la tasa forward entre los plazos d1 y d2 en la convención de intRate.
	* @param intRate convención de la tasa forward que se debe calcular.
	* @param d1 plazo más corto de la tasa forward.
	* @param d2 plazo más largo de la tasa forward.
	* @return valor de la tasa forward calculada.
	* Probablemente este método puede mejorarse haciendo que retorne void y el valor de la
	* tasa forward calculada se almacene dentro de la variable intRate.
	*/
	virtual double getForwardRate(QCInterestRate& intRate, long d1, long d2) = 0;

	/*!
	* Retorna la tasa forward entre los plazos d1 y d2 en la convención de las tasas de la curva.
	* @param d1 plazo más corto de la tasa forward.
	* @param d2 plazo más largo de la tasa forward.
	* @return valor de la tasa forward calculada.
	*/
	virtual double getForwardRate(long d1, long d2) = 0;

	/*!
	* Retorna la factor de capitalización forward entre los plazos d1 y d2.
	* @param d1 plazo más corto del factor forward.
	* @param d2 plazo más largo del fcator forward.
	* @return valor del factor forward calculado.
	*/
	virtual double getForwardWf(long d1, long d2) = 0;

	/*!
	* Este método es un getter que retorna la derivada del último factor de capitalización calculado.
	* @return valor de la derivada.
	*/
	virtual double dfDerivativeAt(unsigned int index) = 0;

	/*!
	* Este método es un getter que retorna la derivada del último factor de capitalización
	* forward calculado.
	* @return valor de la derivada.
	*/
	virtual double fwdWfDerivativeAt(unsigned int index) = 0;

	/*!
	* Este método es un getter que retorna el largo de la curva
	* @return largo de la curva.
	*/
	long getLength()
	{
		return _curve->getLength();
	}

	/*!
	* Destructor de la clase.
	*/
	virtual ~QCInterestRateCurve(){}


protected:
	shared_ptr<QCInterpolator> _curve;	/*!< Plazos y valores de la curva.*/
	QCInterestRate _intRate;			/*!> Tipo de tasa de interés de la curva*/
	
	vector<double> _dfDerivatives;		/*!< Derivadas del factor de descuento interpolado
										respecto a las tasas de la curva.*/
	vector<double> _fwdWfDerivatives;	/*!< Derivadas del factor de capitalizacion forward
										respecto a las tasas de la curva.*/
};

#endif //QCINTERESTRATECURVE_H

