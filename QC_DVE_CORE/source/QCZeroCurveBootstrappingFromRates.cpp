#include <exception>
#include <algorithm>
#include <string>
#include <functional>

#include "QCZeroCurveBootstrappingFromRates.h"

const double EPSILON = 0.0000000001;
const double BP = 0.0001;

QCZeroCurveBootstrappingFromRates::QCZeroCurveBootstrappingFromRates(
	QCDate valueDate,
	vector<shared_ptr<QCTimeDepositPayoff>> inputRates,
	QCZrCpnCrvShrdPtr curve) : QCInterestRateCurveGenerator(
	valueDate, inputRates, QCInterestRateCurveGenerator::emptyFixedLegs(),
	QCInterestRateCurveGenerator::emptyForward(), QCInterestRateCurveGenerator::emptyFloatingLegs(),
	curve)
{

}

void QCZeroCurveBootstrappingFromRates::generateCurve()
{
	cout << "Enter QCZeroCurveBootstrappingFromRates::generateCurve()" << endl;

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

	return;
}

void QCZeroCurveBootstrappingFromRates::generateCurveAndDerivatives()
{
	size_t numRates{ _inputRates.size() };
	size_t numSwaps{ _inputFixedRateLegs.size() };

	//Aqui guardamos todas las curvas bumpeadas
	vector<vector<double>> bumps;
	bumps.resize(numRates + numSwaps);

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
		vector<double> temp;
		temp.resize(numRates + numSwaps);
		for (size_t j = 0; j < numRates + numSwaps; ++j)
		{
			temp.at(j) = _curve->getRateAtIndex(j);
		}
		bumps.at(i) = temp;

		//Devolver el 1pip()
		if (i < numRates)
		{
			_inputRates.at(i)->addToRateValue(-BP);
		}
		else
		{
			_inputFixedRateLegs.at(i - numRates)->addToRateValue(-BP);
		}
	}

	//Se genera la curva con los inputs sin bumpear
	generateCurve();

	//Finalmente se calculan las derivadas
	_derivatives.resize(numRates + numSwaps);
	for (size_t i = 0; i < numRates + numSwaps; ++i)
	{
		_derivatives.at(i).resize(numRates + numSwaps);
		for (size_t j = 0; j < numRates + numSwaps; ++j)
		{
			_derivatives.at(i).at(j) = bumps.at(j).at(i) - _curve->getRateAt(i);
		}
	}
}

unsigned int QCZeroCurveBootstrappingFromRates::getCurveLength()
{
	return _curve->getLength();
}

double QCZeroCurveBootstrappingFromRates::getRateAt(size_t index)
{
	return _curve->getRateAt(index);
}

void QCZeroCurveBootstrappingFromRates::calculateDerivativesAt(size_t derivativeIndex)
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

double QCZeroCurveBootstrappingFromRates::getDerivativeAt(
	size_t rateIndex, size_t derivativeIndex)
{
	return _derivatives.at(rateIndex).at(derivativeIndex);
}

QCZeroCurveBootstrappingFromRates::~QCZeroCurveBootstrappingFromRates()
{

}