#include "QCIcpClfPayoff.h"
#include <cmath>

QCIcpClfPayoff::QCIcpClfPayoff(QCIntrstRtShrdPtr floatingRate,
	double additiveSpread,
	double multipSpread,
	shared_ptr<QCInterestRateLeg> irLeg,
	QCIntRtCrvShrdPtr projectingCurve,
	QCIntRtCrvShrdPtr discountCurve,
	QCDate valueDate,
	QCTimeSeriesShrdPtr fixingData, //En este map deben venir los ICP
	QCTimeSeriesShrdPtr fixingDataUF) :
	QCInterestRatePayoff(floatingRate, irLeg, valueDate, discountCurve, fixingData),
	_additiveSpread(additiveSpread), _multipSpread(multipSpread), _projectingCurve(projectingCurve),
	_fixingDataUF(fixingDataUF)
{
	_setAllRates();
}

double QCIcpClfPayoff::getForwardRateAt(int n)
{
	return _forwardRates.at(n);
}

void QCIcpClfPayoff::_setAllRates()
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
		QCDate startDate = get<QCInterestRateLeg::intRtPrdElmntStartDate>(per); //Fecha inicial del periodo
		if (_valueDate >= startDate)
		{
			//Trae ICP y UF a startDate
			double icpStart = _fixingData->at(startDate);
			double ufStart = _fixingDataUF->at(startDate);

			//Trae ICP _valueDate
			double icpValue = _fixingData->at(_valueDate);
			double ufValue = _fixingDataUF->at(_valueDate);

			//Calcula la TRA con el redondeo. Sea pTRA el plazo desde el inicio del cupon
			//hasta _valueDate. El redondeo se aplica para que al final del periodo 
			//el cashflow coincida con el de contrato.
			int pTRA = startDate.dayDiff(_valueDate);
			double TRA;
			if (pTRA == 0)
			{
				TRA = 0;
			}
			else
			{
				TRA = round(ufStart / ufValue * (icpValue / icpStart - 1) 
					* 360.0 / (double)pTRA * 10000) / 10000.0;
			}

			//Calcula el plazo pZ desde valueDate hasta endDate
			QCDate endDate = get<QCInterestRateLeg::intRtPrdElmntEndDate>(per);
			int pZ = _valueDate.dayDiff(endDate);

			//Calcula tasa z a pZ y se guarda en _forwardRates
			double wfZ = _projectingCurve->getForwardWf(0, pZ);
			double z = _rate->getRateFromWf(wfZ, pZ);
			_forwardRates.at(i) = z;

			//Calcula wf = (1+TRA*pTNA/360)*(1+z*pZ/360)
			_rate->setValue(TRA);
			double wfTNA = _rate->wf(pTRA);

			//Calcula tasa asociada a este wf y guardar en _allRates
			_allRates.at(i) = (_rate->getRateFromWf(wfTNA * wfZ, pZ + pTRA)
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

			//Se aplican los spreads y se guarda la tasa en _allRates
			_allRates.at(i) = (_forwardRates.at(i) + _additiveSpread) * _multipSpread;
		}
	}
}

QCIcpClfPayoff::~QCIcpClfPayoff()
{
}
