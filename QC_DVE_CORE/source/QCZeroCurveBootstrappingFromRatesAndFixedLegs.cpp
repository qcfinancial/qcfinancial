#include <exception>
#include <algorithm>
#include <string>
#include <functional>

#include "QCZeroCurveBootstrappingFromRatesAndFixedLegs.h"

const double EPSILON = 0.00000000001;
const double BP = 0.0001;

QCZeroCurveBootstrappingFromRatesAndFixedLegs::QCZeroCurveBootstrappingFromRatesAndFixedLegs(
	QCDate valueDate,
	vector<shared_ptr<QCTimeDepositPayoff>> inputRates,
	vector<shared_ptr<QCFixedRatePayoff>> inputFixedRateLegs,
	QCZrCpnCrvShrdPtr curve) : QCInterestRateCurveGenerator(
	valueDate,inputRates, inputFixedRateLegs,
	QCInterestRateCurveGenerator::emptyForward(), QCInterestRateCurveGenerator::emptyFloatingLegs(), curve)
{

}

void QCZeroCurveBootstrappingFromRatesAndFixedLegs::generateCurve()
{
	cout << "Enter QCZeroCurveBootstrappingFromRatesAndFixedLegs::generateCurve()" << endl;

	//Loop sobre las tasas
	int rateCounter = -1; //Valor inicial del contador de tasas calculadas de la curva
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
		}
	}
	
	cout << "Rate convergence Ok" << endl;
	cout << "Rate counter: " << rateCounter << endl;

	//rateCounter = _inputRates.size() - 1;
	QCDate swapsStartDate{get<QCInterestRateLeg::intRtPrdElmntStartDate>( _inputFixedRateLegs.at(0)->getPeriodAt(0))};
	long plazo{ _valueDate.dayDiff(swapsStartDate) };
	double df{ _curve->getDiscountFactorAt(plazo) };
	for (auto& fr : _inputFixedRateLegs)
	{
		//Conteo de tasas
		rateCounter += 1;

		//Utiliza la tasa del payoff como punto inicial del cálculo
		double rLast{ fr->getRateValue() }; //valor inicial de la tasa a calcular
		cout << "valor tasa inicial: " << rLast << endl;
		_curve->setOrdinateAtWithValue(rateCounter, rLast); //Se modifica la curva con el valor inicial
		double rNext;
		double diff{ 1 };
		double dd;
		while (abs(diff) > EPSILON)
		{
			double f{ fr->presentValue(true) - df };
			double derf{ fr->getPvRateDerivativeAt(rateCounter) };
			dd = derf;
			rNext = rLast - f / derf;
			_curve->setOrdinateAtWithValue(rateCounter, rNext);
			diff = rNext - rLast;
			rLast = rNext;
		}
		cout << "valor tasa final: " << rLast << endl;
		cout << "valor derivada final: " << dd << endl;
	}

	cout << "Fixed Rate Leg convergence Ok" << endl;
	cout << "rateCounter: " << rateCounter << endl;
	return;
}

void QCZeroCurveBootstrappingFromRatesAndFixedLegs::generateCurveAndDerivatives()
{
	size_t numRates{ _inputRates.size() };
	size_t numSwaps{ _inputFixedRateLegs.size() };

	//Aqui guardamos todas las curvas bumpeadas
	vector<vector<double>> bumps;
	bumps.resize(numRates + numSwaps);
	vector<vector<double>> bumps2;
	bumps2.resize(numRates + numSwaps);
	vector<double> temp;
	temp.resize(numRates + numSwaps);
	vector<double> temp2;
	temp2.resize(numRates + numSwaps);
	for (size_t i = 0; i < numRates + numSwaps; ++i)
	{
		//Bumpear el input que corresponde
		if (i < numRates)
		{
			_inputRates.at(i)->addToRateValue(BP);
		}
		else
		{
			_inputFixedRateLegs.at(i - numRates)->addToRateValue(BP);
		}

		//Ejecutar generateCurve() y guardar los valores
		for (size_t k = 0; k < numRates + numSwaps; ++k)
		{
			_curve->setOrdinateAtWithValue(k, 0.0);
		}
		generateCurve();

		cout << "curva: " << i << endl;
		for (size_t j = 0; j < numRates + numSwaps; ++j)
		{
			temp.at(j) = _curve->getRateAtIndex(j);
			cout << "temp.at_" << j << " : " << temp.at(j) << endl;
		}
		bumps.at(i) = temp;
		
		//Bajar 1pip()
		if (i < numRates)
		{
			_inputRates.at(i)->addToRateValue(-2.0 * BP);
		}
		else
		{
			_inputFixedRateLegs.at(i - numRates)->addToRateValue(-2.0 * BP);
		}
		for (size_t k = 0; k < numRates + numSwaps; ++k)
		{
			_curve->setOrdinateAtWithValue(k, 0.0);
		}
		generateCurve();

		for (size_t j = 0; j < numRates + numSwaps; ++j)
		{
			temp2.at(j) = _curve->getRateAtIndex(j);
			//cout << "temp.at_" << j << " : " << temp2.at(j) << endl;
		}
		bumps2.at(i) = temp2;

		//Bumpear el input de vuelta a su nivel original
		if (i < numRates)
		{
			_inputRates.at(i)->addToRateValue(BP);
		}
		else
		{
			_inputFixedRateLegs.at(i - numRates)->addToRateValue(BP);
		}

	}
	
	//Se genera la curva con los inputs sin bumpear
	generateCurve();

	//Finalmente se calculan las derivadas
	_derivatives.resize(numRates + numSwaps);
	for (size_t i = 0; i < numRates + numSwaps; ++i)
	{
		//cout << "curva num: " << i << endl;
		_derivatives.at(i).resize(numRates + numSwaps);
		for (size_t j = 0; j < numRates + numSwaps; ++j)
		{
			_derivatives.at(i).at(j) = (bumps.at(j).at(i) - bumps2.at(j).at(i))/2.0;// -_curve->getRateAt(j);
			cout << "der: " << _derivatives.at(i).at(j) << endl;
		}
	}
}

unsigned int QCZeroCurveBootstrappingFromRatesAndFixedLegs::getCurveLength()
{
	return _curve->getLength();
}

double QCZeroCurveBootstrappingFromRatesAndFixedLegs::getRateAt(size_t index)
{
	return _curve->getRateAt(index);
}

void QCZeroCurveBootstrappingFromRatesAndFixedLegs::calculateDerivativesAt(size_t derivativeIndex)
{
	//Deducir a que input corresponde el índice y sumar 1pip al índice que corresponde
	size_t numRates{ _inputRates.size() };
	size_t numSwaps{ _inputFixedRateLegs.size() };
	if (derivativeIndex < numRates + 1)
	{
		_inputRates.at(derivativeIndex)->addToRateValue(BP);
	}
	else
	{
		_inputFixedRateLegs.at(derivativeIndex - numRates)->addToRateValue(BP);
	}

	//Ejecutar generateCurve() y guardar los valores
	generateCurve();
	vector<double> temp;
	temp.resize(numRates + numSwaps);
	for (size_t i = 0; i < numRates + numSwaps; ++i)
	{
		temp.at(i) = _curve->getRateAt(i);
	}
	
	//Devolver el 1pip y volver a ejecutar generateCurve()
	if (derivativeIndex < numRates + 1)
	{
		_inputRates.at(derivativeIndex)->addToRateValue(-BP);
	}
	else
	{
		_inputFixedRateLegs.at(derivativeIndex - numRates)->addToRateValue(-BP);
	}
	generateCurve();

	//Sacar la diferencia entre _curve y los valores guardados y guardar esas diferencias en _derivatives
	for (size_t i = 0; i < numRates + numSwaps; ++i)
	{
		_derivatives.at(i).at(derivativeIndex) = (temp.at(i) - _curve->getRateAt(i)) / BP;
	}
	return;
}

double QCZeroCurveBootstrappingFromRatesAndFixedLegs::getDerivativeAt(
	size_t rateIndex, size_t derivativeIndex)
{
	return _derivatives.at(rateIndex).at(derivativeIndex);
}

QCZeroCurveBootstrappingFromRatesAndFixedLegs::~QCZeroCurveBootstrappingFromRatesAndFixedLegs()
{

}