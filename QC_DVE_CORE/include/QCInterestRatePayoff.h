#ifndef QCINTERESTRATEPAYOFF_H
#define QCINTERESTRATEPAYOFF_H

#include <map>
#include <tuple>
#include <array>

#include "QCDefinitions.h"
#include "QCDate.h"
#include "QCInterestRateLeg.h"
#include "QCZeroCouponCurve.h"
#include "QCInterestRate.h"

/*!
* @brief QCInterestRatePayoff define una interfaz general para estructura de flujos de caja
* de un instrumento de tasa de interés. Es una clase que no se puede instanciar directamente.
*/

/*!
* Enumera los tipos de flujos de caja que un payoff puede contener:
* disposición (accretion), interés y amortización.
*/
enum QCCashFlowLabel
{
	qcAccretion,	/*!< Disposición */
	qcInterest,		/*!< Interés */
	qcAmortization	/*!< Amortización */
};

class QCInterestRatePayoff
{
public:
	/*!
	* Genera los flujos de caja y los guarda dentro de la clase
	* @return (void)
	*/
	void payoff();

	/*!
	* Suma el valor mov al valor de la tasa de interés del payoff
	* @param mov valor a sumar al valor de la tasa.
	*/
	void addToRateValue(double mov);

	/*!
	* Calcula y retorna el numero de flujos de caja
	* @return (int) el numero de flujos
	*/
	int payoffSize() const;

	/*!
	* Retorna el índice del último flujo
	* @return índice
	*/
	unsigned int getLastPeriodIndex() const;

	/*!
	* Entrega período de tasa de interés en la posición n-sima
	* @param n índice de la posición
	* @return QCInterestRatePeriod en la posición n-sima.
	*/
	QCInterestRateLeg::QCInterestRatePeriod getPeriodAt(size_t n) const;

	/*!
	* Retorna el valor del flujo de caja de _valueDate
	* @return (double) el valor del cashflow
	*/
	double getValueDateCashflow();

	/*!
	* Calcula y retorna el valor presente de la pata descontando con la curva de descuento.
	* Dentro de este proceso se calculan tambien las derivadas del valor presente respecto
	* a cada punto de las curvas de descuento y proyeccion.
	* @return (double) el valor presente
	*/
	double presentValue(bool includeFirstCashflow = false);

	/*!
	* Retorna la derivada del valor presente respecto a la componente index de la
	* curva de descuento (se cuenta a partir de 0).
	* @param index el indice solicitado
	* @return (double) la derivada solicitada
	*/
	double getPvRateDerivativeAt(unsigned int index);

	/*!
	* Retorna la derivada del valor presente respecto a la componente index de la
	* curva de proyeccion (se cuenta a partir de 0).
	* @param index el indice solicitado
	* @return (double) la derivada solicitada
	*/
	double getPvProjRateDerivativeAt(unsigned int index);

	/*!
	* Retorna el largo (numero de vertices) de la curva de descuento.
	* @return (unsigned long) largo de la curva.
	*/
	unsigned long discountCurveLength();

	/*!
	* Retorna el largo (numero de vertices) de la curva de proyeccion.
	* @return (unsigned long) largo de la curva.
	*/
	unsigned long projectingCurveLength();

	/*!
	* Retorna la tupla que describe el cashflow en el indice n del vector
	* donde se registran los cashflows del payoff. Cada cashflow se forma con una fecha
	* (fecha del cashflow), una etiqueta que indica que tipo de casflow es (interes, disposicion
	* o amortizacion) y el monto del cashflow.
	* @param n indice del cashflow solicitado.
	* @return (tuple<QCDate, QCCashFlowLabel, double>) cashflow.
	*/
	tuple<QCDate, QCCashFlowLabel, double> getCashflowAt(unsigned int n);

	/*!
	* Destructor
	*/
	virtual ~QCInterestRatePayoff();

protected:
	/*! Constructor de la clase. El hecho que sea protected impide que esta clase pueda
	* ser instanciada de forma directa. Sólo se pueden instanciar sus clases derivadas.
	* @param rate guarda el valor y el tipo de tasa qu se utiliza para el cálculo de intereses.
	* @param irLeg estructura de fechas, nocionales vigentes, amortizaciones y disposiciones.
	* @param valueDate fecha a la cual se calculará el valor presente del payoff cuando se ejecue
	* el método presentValue().
	* @param projectingCurve curva de proyección de tasas forward.
	* @param curva de descuento.
	* @fixingData información necesaria para tasas conocidas a valueDate que intervengan en el
	* proceso de cálculo de intereses.
	*/
	QCInterestRatePayoff(
		QCIntrstRtShrdPtr rate,
		shared_ptr<QCInterestRateLeg> irLeg,
		QCDate valueDate,
		QCIntRtCrvShrdPtr projectingCurve,
		QCIntRtCrvShrdPtr discountCurve,
		QCTimeSeriesShrdPtr fixingData);
	
	/*! Aqui se almacena la variable rate usada en el constructor.*/
	QCIntrstRtShrdPtr _rate;

	/*! Aqui se almacena la variable irLeg usada en el constructor.*/
	shared_ptr<QCInterestRateLeg> _irLeg;

	/*! Aqui se almacena la variable valueDate usada en el constructor.*/
	QCDate _valueDate;

	/*! Aqui se almacena la variable projectingCurve usada en el constructor.*/
	QCIntRtCrvShrdPtr _projectingCurve;

	/*! Aqui se almacena la variable discountCurve usada en el constructor.*/
	QCIntRtCrvShrdPtr _discountCurve;

	/*! Aqui se almacena la variable fixingData usada en el constructor.*/
	QCTimeSeriesShrdPtr _fixingData;
	
	/*! Aquí se almacena el período vigente considerando valueDate.*/
	int _currentPeriod;

	/* Aquí se almacenan los flujos calculados con el método payoff().*/
	vector<tuple<QCDate, QCCashFlowLabel, double>> _payoffs;

	/*! Este método calcula o determina todas las tasas necesarias para el cálculo
	* de intereses. */
	virtual void _setAllRates();

	/*! Aquí se almacenan las tasas obtenidas con _setAllRates().*/
	vector<double> _allRates;

	/*! Aqui se guarda el valor del cashflow correspondiente a _valueDate.
	* Si no existe ese cashflow se guarda un 0.*/
	double _valueDateCashflow;

	/*! Aqui se guardan las derivadas del valor presente respecto a cada vértice
	* de la curva de descuento.*/
	vector<double> _pvRateDerivatives;

	/*! Aqui se guardan las derivadas de las tasas de proyección respecto a
	* cada punto de la curva de proyeccion. Es una matriz porque por cada tasa
	* proyectada existen derivadas respecto a cada vértice de la curva de proyección.*/
	vector<vector<double>> _allRatesDerivatives;

	/*! Aqui se guardan las derivadas del valor presente respecto a
	* los vértices de la curva de proyección.*/
	vector<double> _pvProjCurveDerivatives;
};

#endif //QCINTERESTRATEPAYOFF_H

