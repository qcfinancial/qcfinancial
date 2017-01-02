#ifndef QCICPCLPPAYOFF_H
#define QCICPCLPPAYOFF_H

#include "QCInterestRatePayoff.h"
class QCIcpClpPayoff :
	public QCInterestRatePayoff
{
public:
	QCIcpClpPayoff(QCIntrstRtShrdPtr floatingRate,
		double additiveSpread,
		double multipSpread,
		shared_ptr<QCInterestRateLeg> irLeg,
		QCIntRtCrvShrdPtr projectingCurve,
		QCIntRtCrvShrdPtr discountCurve,
		QCDate valueDate,
		QCTimeSeriesShrdPtr fixingData
		);
	double getForwardRateAt(int n);
	virtual ~QCIcpClpPayoff();

protected:
	virtual void _setAllRates() override; 
	QCIntRtCrvShrdPtr _projectingCurve;
	double _additiveSpread;
	double _multipSpread;
	
	//Aqui se guardan todas las fijaciones libres de spreads
	vector<double> _forwardRates;			
	
	//Aqui se guardan las derivadas de las tasas de proyeccion respecto a
	//cada punto de la curva de proyeccion. Es una matriz porque por cada tasa
	//proyectada existen derivadas respecto a cada vertice de la curva de proyeccion.
	vector<vector<double>> _allRatesDerivatives;
	
	//Aqui se guardan las derivadas del valor presente respecto a
	//los vertices de la curva de proyeccion.
	vector<double> _pvProjCurveDerivatives;	
};

#endif //QCICPCLPPAYOFF_H
