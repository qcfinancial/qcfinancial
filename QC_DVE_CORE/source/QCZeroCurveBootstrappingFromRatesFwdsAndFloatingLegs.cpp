#include "QCZeroCurveBootstrappingFromRatesFwdsAndFloatingLegs.h"

#define EPSILON .0000001
#define PB .0001

QCZeroCurveBootstrappingFromRatesFwdsAndFloatingLegs::QCZeroCurveBootstrappingFromRatesFwdsAndFloatingLegs(
	QCDate valueDate,
	vector<shared_ptr<QCTimeDepositPayoff>> inputRates,
	vector<shared_ptr<QCFXForward>> inputForwards,
	unsigned int whichForwardLeg,
	vector<shared_ptr<QCFloatingRatePayoff>> inputFloatingRateLegs,
	shared_ptr<QCZeroCouponCurve> newZeroCurve) : QCInterestRateCurveGenerator(valueDate,
	inputRates, QCInterestRateCurveGenerator::emptyFixedLegs(), inputForwards,
	inputFloatingRateLegs, newZeroCurve)
{
	if (whichForwardLeg > 1)
	{
		throw invalid_argument("Number of Fx Forward leg must be 0 or 1.");
	}
	_whichForwardLeg = whichForwardLeg;
}

void QCZeroCurveBootstrappingFromRatesFwdsAndFloatingLegs::generateCurve()
{
	//Loop sobre las tasas
	unsigned int rateCounter = -1; //Valor inicial del contador de tasas calculadas de la curva
	QCDate startDate{ get<QCInterestRateLeg::intRtPrdElmntStartDate>(
		_inputRates.at(0)->getPeriodAt(0)) }; //Fecha inicial de todas las tasas que vamos a calcular
	for (auto& td : _inputRates)
	{
		//Conteo de tasas
		rateCounter += 1;

		//Utiliza la tasa del payoff como punto inicial del cálculo
		double rLast{ td->getRateValue() }; //valor inicial de la tasa a calcular
		_curve->setOrdinateAtWithValue(rateCounter, rLast); //Se modifica la curva con el valor inicial
		double rNext;
		double diff{ 1 };
		while (abs(diff) > EPSILON)
		{
			double f{ td->presentValue(true) };
			double df{ td->getPvRateDerivativeAt(rateCounter) };
			rNext = rLast - f / df;
			_curve->setOrdinateAtWithValue(rateCounter, rNext);
			diff = rNext - rLast;
			rLast = rNext;
			cout << rLast << endl;
		}
	}
	cout << "bootstrapping: tasas calibradas " << endl;

	//Loop sobre los forwards
	rateCounter = _inputRates.size() - 1;
	QCDate fwdsStartDate{ get<QCInterestRateLeg::intRtPrdElmntStartDate>(
		_inputForwards.at(0)->getPeriod()) };
	long plazo{ _valueDate.dayDiff(fwdsStartDate) };
	for (auto& fr : _inputForwards)
	{
		//Conteo de tasas
		rateCounter += 1;

		//Utiliza la tasa del payoff como punto inicial del cálculo
		double rLast{ .01 }; //valor inicial de la tasa a calcular
		_curve->setOrdinateAtWithValue(rateCounter, rLast); //Se modifica la curva con el valor inicial
		double rNext;
		double diff{ 1 };
		while (abs(diff) > EPSILON)
		{
			double f{ fr->marketValue() };
			double df{ fr->getPvRateDerivativeAtFromLeg(rateCounter, _whichForwardLeg) };
			rNext = rLast - f / df;
			_curve->setOrdinateAtWithValue(rateCounter, rNext);
			diff = rNext - rLast;
			rLast = rNext;
		}
	}
	
	cout << "bootstrapping: forwards calibrados" << endl;

	//Loop sobre las patas flotantes
	
	rateCounter = _inputRates.size() + _inputForwards.size() - 1;
	QCDate swapsStartDate{ get<QCInterestRateLeg::intRtPrdElmntStartDate>(_inputFloatingRateLegs.at(0)->getPeriodAt(0)) };
	plazo = _valueDate.dayDiff(swapsStartDate);
	double df{ _curve->getDiscountFactorAt(plazo) };
	for (auto& fr : _inputFloatingRateLegs)
	{
		//Conteo de tasas
		rateCounter += 1;

		//Utiliza la tasa del payoff como punto inicial del cálculo
		double rLast{ .01}; //valor inicial de la tasa a calcular
		_curve->setOrdinateAtWithValue(rateCounter, rLast); //Se modifica la curva con el valor inicial
		double rNext;
		double diff{ 1 };
		while (abs(diff) > EPSILON)
		{
			double f{ fr->presentValue(true) - df };
			double df{ fr->getPvRateDerivativeAt(rateCounter) };
			rNext = rLast - f / df;
			_curve->setOrdinateAtWithValue(rateCounter, rNext);
			diff = rNext - rLast;
			rLast = rNext;
		}
	}
	
	cout << "bootstrapping: patas flotantes calibradas" << endl;

	return;
}

void QCZeroCurveBootstrappingFromRatesFwdsAndFloatingLegs::generateCurveAndDerivatives()
{

}

unsigned int QCZeroCurveBootstrappingFromRatesFwdsAndFloatingLegs::getCurveLength()
{
	return 0;
}

double QCZeroCurveBootstrappingFromRatesFwdsAndFloatingLegs::getRateAt(unsigned int index)
{
	return 0.0;
}

void QCZeroCurveBootstrappingFromRatesFwdsAndFloatingLegs::calculateDerivativesAt(
	size_t derivativeIndex)
{
	return;
}

double QCZeroCurveBootstrappingFromRatesFwdsAndFloatingLegs::getDerivativeAt(
	unsigned int rateIndex, unsigned int derivativeIndex)
{
	return 0.0;
}

QCZeroCurveBootstrappingFromRatesFwdsAndFloatingLegs::~QCZeroCurveBootstrappingFromRatesFwdsAndFloatingLegs()
{
}
