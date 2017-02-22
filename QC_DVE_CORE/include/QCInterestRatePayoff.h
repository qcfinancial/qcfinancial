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

enum QCCashFlowLabel
{
	qcAccretion,
	qcInterest,
	qcAmortization
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
	* Calcula y retorna el numero de flujos de caja
	* @return (int) el numero de flujos
	*/
	int payoffSize();

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
	double presentValue();

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
	QCInterestRatePayoff(
		QCIntrstRtShrdPtr rate,
		shared_ptr<QCInterestRateLeg> irLeg,
		QCDate valueDate,
		QCIntRtCrvShrdPtr projectingCurve,
		QCIntRtCrvShrdPtr discountCurve,
		QCTimeSeriesShrdPtr fixingData);
	
	QCIntrstRtShrdPtr _rate;
	shared_ptr<QCInterestRateLeg> _irLeg;
	QCDate _valueDate;
	QCIntRtCrvShrdPtr _projectingCurve;
	QCIntRtCrvShrdPtr _discountCurve;
	QCTimeSeriesShrdPtr _fixingData;
	
	int _currentPeriod;
	vector<tuple<QCDate, QCCashFlowLabel, double>> _payoffs;

	virtual void _setAllRates();
	vector<double> _allRates;

	//Aqui se guarda el valor del cashflow correspondiente a _valueDate.
	//Si no existe ese cashflow se guarda un 0.
	double _valueDateCashflow;

	//Aqui se guardan las derivadas del valor presente respecto a cada vertice
	//de la curva de descuento.
	vector<double> _pvRateDerivatives;

	//Aqui se guardan las derivadas de las tasas de proyeccion respecto a
	//cada punto de la curva de proyeccion. Es una matriz porque por cada tasa
	//proyectada existen derivadas respecto a cada vertice de la curva de proyeccion.
	vector<vector<double>> _allRatesDerivatives;

	//Aqui se guardan las derivadas del valor presente respecto a
	//los vertices de la curva de proyeccion.
	vector<double> _pvProjCurveDerivatives;
};

#endif //QCINTERESTRATEPAYOFF_H

