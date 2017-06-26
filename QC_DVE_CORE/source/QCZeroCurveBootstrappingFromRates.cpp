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
	QCInterestRateCurveGenerator::emptyForward(), QCInterestRateCurveGenerator::emptyFloatingLegs(), curve)
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

	//Aqui guardamos todas las curvas bumpeadas
	vector<vector<double>> bumps;
	bumps.resize(numRates);

	vector<vector<double>> bumps2;
	bumps2.resize(numRates);

	vector<double> temp;
	temp.resize(numRates);

	vector<double> temp2;
	temp2.resize(numRates);

	for (size_t i = 0; i < numRates; ++i)
	{
		cout << "iteracion i = " << i << endl;
		//Bumpear el input que corresponde
		_inputRates.at(i)->addToRateValue(BP);

		//Ejecutar generateCurve() y guardar los valores
		for (size_t k = 0; k < numRates; ++k)
		{
			_curve->setOrdinateAtWithValue(k, 0.0);
		}

		generateCurve();

		cout << "curva +: " << i << " generada." << endl;
		for (size_t j = 0; j < numRates; ++j)
		{
			temp.at(j) = _curve->getRateAtIndex(j);
			//cout << "temp.at_" << j << " : " << temp.at(j) << endl;
		}
		bumps.at(i) = temp;

		//Bajar 1pip()
		_inputRates.at(i)->addToRateValue(-2.0 * BP);

		for (size_t k = 0; k < numRates; ++k)
		{
			_curve->setOrdinateAtWithValue(k, 0.0);
		}

		generateCurve();
		cout << "curva -: " << i << " generada." << endl;

		for (size_t j = 0; j < numRates; ++j)
		{
			temp2.at(j) = _curve->getRateAtIndex(j);
			//cout << "temp.at_" << j << " : " << temp2.at(j) << endl;
		}
		bumps2.at(i) = temp2;

		//Bumpear el input de vuelta a su nivel original
		_inputRates.at(i)->addToRateValue(BP);

	}

	//Se genera la curva con los inputs sin bumpear
	generateCurve();

	//Finalmente se calculan las derivadas
	_derivatives.resize(numRates);
	for (size_t i = 0; i < numRates; ++i)
	{
		//cout << "curva num: " << i << endl;
		_derivatives.at(i).resize(numRates);
		for (size_t j = 0; j < numRates; ++j)
		{
			_derivatives.at(i).at(j) = (bumps.at(j).at(i) - bumps2.at(j).at(i)) / 2.0;// -_curve->getRateAt(j);
			cout << "derivative: " << _derivatives.at(i).at(j) << endl;
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