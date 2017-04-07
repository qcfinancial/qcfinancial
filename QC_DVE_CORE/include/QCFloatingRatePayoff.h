#ifndef QCFLOATINGRATEPAYOFF_H
#define QCFLOATINGRATEPAYOFF_H

#include <iostream>
#include "QCInterestRatePayoff.h"

using namespace std;

/*!
* @brief QCFloatingRatePayoff hereda de QCInterestRatePayoff e implementa la estructura de un bono
* o pata a tasa flotante.
*/
class QCFloatingRatePayoff :
	public QCInterestRatePayoff
{
public:
	/*!
	* Constructor, se apoya en el constructor de la clase madre.
	* @param floatingRate valor y forma de la tasa flotante. El valor de la tasa no es importante.
	* El factor del último fixing se obtiene desde el parámtero fixingData.
	* @param additiveSpread spread en la misma convención de la tasa flotante que se suma a esta
	* última para calcular los intereses de un período.
	* @param multipSpread spread multiplicativo que se aplica la tasa flotante para calcular los
	* intereses de un período. Si un QCFloatingRatePayoff tiene spread aditivo (sa) y spread multiplicativo
	* sm, si flot es la tasa flotante de un período, los interes de ese período se calculan como
	* wf(flot*sm + sa, yf).
	* @param irLeg estructura de fechas y amortizaciones.
	* @param projectingCurve curva de proyección de las tasas flotantes (para la obtención del valor presente).
	* @param discountCurve curva de descuento (para la obtención del valor presente)
	* @param valueDate fecha de valorización.
	* @param fixingData contiene una serie histórica de valores de la tasa flotante. Sirve para fijar
	* el valor de la tasa del flujo de intereses corriente (según valueDate).
	*/
	QCFloatingRatePayoff(QCIntrstRtShrdPtr floatingRate,
		double additiveSpread,
		double multipSpread,
		shared_ptr<QCInterestRateLeg> irLeg,
		QCIntRtCrvShrdPtr projectingCurve,
		QCIntRtCrvShrdPtr discountCurve,
		QCDate valueDate,
		QCTimeSeriesShrdPtr fixingData
		);
	/*!
	* Sobrecarga del operador <. Se ordena por fecha final.
	* @param rhs otro QCFloatingRatePayoff para comparar (this < rhs)
	*/
	bool operator<(const QCFloatingRatePayoff& rhs);

	/*!
	* Similar al operador < pero actúa sobre shared_ptr<QCFloatingRatePayoff>.
	* @param lhs lado izquierdo de la comparación.
	* @param rhs lado derecho de la comparación.
	*/
	static bool lessThan(shared_ptr<QCFloatingRatePayoff> lhs, shared_ptr<QCFloatingRatePayoff> rhs);

	/*!
	* Retorna la tasa forward número n calculada para el payoff, considerando valueDate.
	* @param n posición de la tasa forward buscada.
	* @return valor de la tasa forward.
	*/
	double getForwardRateAt(size_t n);

	/*!
	* Destructor.
	*/
	virtual ~QCFloatingRatePayoff();

protected:
	/*!
	* Se fija la tasa a aplicar en cada período.
	*/
	virtual void _setAllRates() override;

	/*!
	* Variable que almacena los valores y características de la curva de proyección.
	*/
	QCIntRtCrvShrdPtr _projectingCurve;

	/*!
	* Variable que almacena el spread aditivo.
	*/
	double _additiveSpread;

	/*!
	* Variable que almacena el spread multiplicativo.
	*/
	double _multipSpread;

	/*!
	* Aqui se guardan todas las fijaciones futuras libres de spreads.
	*/
	vector<double> _forwardRates; 
};

#endif //QCFLOATINGRATEPAYOFF_H
