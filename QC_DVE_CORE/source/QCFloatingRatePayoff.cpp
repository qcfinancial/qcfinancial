#include "QCFloatingRatePayoff.h"

QCFloatingRatePayoff::QCFloatingRatePayoff(QCIntrstRtShrdPtr floatingRate,
	double additiveSpread,
	double multipSpread,
	shared_ptr<QCInterestRateLeg> irLeg,
	QCIntRtCrvShrdPtr projectingCurve,
	QCIntRtCrvShrdPtr discountCurve,
	QCDate valueDate,
	QCTimeSeriesShrdPtr fixingData) :
	QCInterestRatePayoff(floatingRate, irLeg, valueDate, discountCurve, fixingData),
	_additiveSpread(additiveSpread), _multipSpread(multipSpread), _projectingCurve(projectingCurve)
{
	_setAllRates();
}

double QCFloatingRatePayoff::getForwardRateAt(int n)
{
	return _forwardRates.at(n);
}

void QCFloatingRatePayoff::_setAllRates()
{
	//Loopea sobre los periods de interest rate leg a partir del current period
	int numPeriods = _irLeg->size();
	_allRates.resize(numPeriods);
	_forwardRates.resize(numPeriods);
	unsigned int tempCurrentPeriod;
	if (_currentPeriod == -1)
	{
		tempCurrentPeriod = 0;
	}
	else
	{
		tempCurrentPeriod = _currentPeriod;
	}
	for (int i = tempCurrentPeriod; i < numPeriods; ++i)
	{
		auto per = _irLeg->getPeriodAt(i);
		//Si el fixing date es menor que _valueDate lo va a calcular con fixingData
		QCDate temp = get<QCInterestRateLeg::intRtPrdElmntFxngInitDate>(per);
		if (_valueDate >= temp)
		{
			_allRates.at(i) = (_fixingData->at(get<QCInterestRateLeg::intRtPrdElmntFxngDate>(per))
				+ _additiveSpread) * _multipSpread;
		}
		else
		{
			//Si no calcula el factor de descuento fwd segun los parametros del periodo
			QCDate date1 = get<QCInterestRateLeg::intRtPrdElmntFxngInitDate>(per);
			QCDate date2 = get<QCInterestRateLeg::intRtPrdElmntFxngEndDate>(per);
			long d1 = _valueDate.dayDiff(date1);
			long d2 = _valueDate.dayDiff(date2);
			
			//La tasa fwd se expresa en la convencion en que se construyo
			//la curva de proyeccion. Al fabricar el payoff se debe poner atencion
			//a que esta coincida con las caracteristicas del swap.
			double tasaFwd = _projectingCurve->getForwardRate(d1, d2);

			//Cada tasa fwd (o fijacion anterior) se guarda en _forwardRates
			_forwardRates.at(i) = tasaFwd;

			//Se aplican los spreads y se guarda la tasa en _allRates
			_allRates.at(i) = (tasaFwd + _additiveSpread) * _multipSpread;
		}
	}
}

QCFloatingRatePayoff::~QCFloatingRatePayoff()
{
}
