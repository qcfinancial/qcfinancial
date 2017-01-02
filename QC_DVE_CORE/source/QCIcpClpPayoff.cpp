#include "QCIcpClpPayoff.h"
#include <cmath>

QCIcpClpPayoff::QCIcpClpPayoff(QCIntrstRtShrdPtr floatingRate,
	double additiveSpread,
	double multipSpread,
	shared_ptr<QCInterestRateLeg> irLeg,
	QCIntRtCrvShrdPtr projectingCurve,
	QCIntRtCrvShrdPtr discountCurve, //QCZrCpnCrvShrdPtr
	QCDate valueDate,
	QCTimeSeriesShrdPtr fixingData) :
	QCInterestRatePayoff(floatingRate, irLeg, valueDate, discountCurve, fixingData),
	_additiveSpread(additiveSpread), _multipSpread(multipSpread), _projectingCurve(projectingCurve)
{
	_setAllRates();
}

double QCIcpClpPayoff::getForwardRateAt(int n)
{
	return _forwardRates.at(n);
}

void QCIcpClpPayoff::_setAllRates()
{
	//Loopea sobre los periods de interest rate leg a partir del current period
	int numPeriods = _irLeg->size();
	_allRates.resize(numPeriods);
	_forwardRates.resize(numPeriods);
	_allRatesDerivatives.resize(numPeriods);
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
		QCDate startDate = get<QCInterestRateLeg::intRtPrdElmntStartDate>(per); //Fecha inicial del periodo
		if (_valueDate >= startDate)
		{
			//Trae ICP a startDate
			double icpStart = _fixingData->at(startDate);
			
			//Trae ICP _valueDate
			double icpValue = _fixingData->at(_valueDate);

			//Calcula la TNA con el redondeo. Sea pTNA el plazo desde el inicio del cupon
			//hasta _valueDate. El redondeo se aplica para que al final del periodo 
			//el cashflow coincida con el de contrato.
			int pTNA = startDate.dayDiff(_valueDate);
			double TNA;
			if (pTNA == 0)
			{
				TNA = 0;
			}
			else
			{
				TNA = round((icpValue / icpStart - 1)*360.0 / (double)pTNA * 10000) / 10000.0;
			}
			
			//Calcula el plazo pZ desde valueDate hasta endDate
			QCDate endDate = get<QCInterestRateLeg::intRtPrdElmntEndDate>(per);
			int pZ = _valueDate.dayDiff(endDate);

			//Calcula tasa z a pZ y se guarda en _forwardRates
			double wfZ = _projectingCurve->getForwardWf(0, pZ);
			double z = _rate->getRateFromWf(wfZ, pZ);
			_forwardRates.at(i) = z;

			//Calcula wf = (1+TNA*pTNA/360)*(1+z*pZ/360)
			_rate->setValue(TNA);
			double wfTNA = _rate->wf(pTNA);

			//Se calculan y guardan las derivadas de este factor Fwd
			vector<double> tempDer;
			tempDer.resize(_projectingCurve->getLength());
			for (unsigned int j = 0; j < _projectingCurve->getLength(); ++j)
			{
				tempDer.at(j) = _projectingCurve->fwdWfDerivativeAt(j) * wfTNA * 360 / pZ;
			}
			_allRatesDerivatives.at(i) = tempDer;

			//Calcula tasa asociada a este wf y guardar en _allRates
			_allRates.at(i) = (_rate->getRateFromWf(wfTNA * wfZ, pZ + pTNA)
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
			double wfFwd = _projectingCurve->getForwardWf(d1, d2);

			//Cada tasa fwd (o fijacion anterior) se guarda en _forwardRates
			_forwardRates.at(i) = _rate->getRateFromWf(wfFwd, d2 - d1);

			//Se calculan y guardan las derivadas de este factor Fwd
			vector<double> tempDer;
			tempDer.resize(_projectingCurve->getLength());
			for (unsigned int j = 0; j < _projectingCurve->getLength(); ++j)
			{
				tempDer.at(j) = _projectingCurve->fwdWfDerivativeAt(j) * 360.0 / (d2 - d1);
			}
			_allRatesDerivatives.at(i) = tempDer;

			//Se aplican los spreads y se guarda la tasa en _allRates
			_allRates.at(i) = (_forwardRates.at(i) + _additiveSpread) * _multipSpread;
		}
	}
}

QCIcpClpPayoff::~QCIcpClpPayoff()
{
}
