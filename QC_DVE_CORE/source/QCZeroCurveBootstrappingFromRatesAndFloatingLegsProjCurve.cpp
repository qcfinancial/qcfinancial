#include "QCZeroCurveBootstrappingFromRatesAndFloatingLegsProjCurve.h"

const double EPSILON = 0.00000000001;
const double BP = 0.0001;

QCZeroCurveBootstrappingFromRatesAndFloatingLegsProjCurve::QCZeroCurveBootstrappingFromRatesAndFloatingLegsProjCurve(
	QCDate valueDate,
	vector<shared_ptr<QCTimeDepositPayoff>> inputRates,
	vector<shared_ptr<QCInterestRateBasisSwap>> inputBasisSwaps,
	unsigned int whichBasisLeg,
	QCZrCpnCrvShrdPtr newZeroCurve) : QCInterestRateCurveGenerator(valueDate,
	inputRates,
	QCInterestRateCurveGenerator::emptyFixedLegs(),
	QCInterestRateCurveGenerator::emptyForward(),
	QCInterestRateCurveGenerator::emptyFloatingLegs(),
	inputBasisSwaps, newZeroCurve)
{
	if (whichBasisLeg > 1)
		whichBasisLeg = 1;
	_whichBasisLeg = whichBasisLeg;
}

void QCZeroCurveBootstrappingFromRatesAndFloatingLegsProjCurve::generateCurve()
{
	//cout << "Enter QCZeroCurveBootstrappingFromRatesAndFloatingLegsProjCurve::generateCurve()" << endl;
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

	//cout << "bootstrapping: tasas calibrados" << endl;

	//Loop sobre los basis swaps
	for (auto& fr : _inputBasisSwaps)
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
			double df{ fr->getProjRateDerivativeAtFromLeg(rateCounter, _whichBasisLeg) };
			rNext = rLast - f / df;
			_curve->setOrdinateAtWithValue(rateCounter, rNext);
			diff = rNext - rLast;
			rLast = rNext;
		}
	}

	//cout << "bootstrapping: basis swaps calibrados" << endl;

	return;
}

void QCZeroCurveBootstrappingFromRatesAndFloatingLegsProjCurve::generateCurveAndDerivatives()
{
	size_t numSwaps{ _inputBasisSwaps.size() };
	size_t numRates{ _inputRates.size() };

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
		//cout << "iteracion i = " << i << endl;
		//Bumpear solamente los basis
		if (i < numRates)
		{
			_inputRates.at(i)->addToRateValue(BP);
		}
		else
		{
			_inputBasisSwaps.at(i - numRates)->getPayoff(_whichBasisLeg)->addToAdditiveSpreadValue(BP);
		}

		//Ejecutar generateCurve() y guardar los valores
		for (size_t k = 0; k < numRates + numSwaps; ++k)
		{
			_curve->setOrdinateAtWithValue(static_cast<unsigned long>(k), 0.0);
		}
		generateCurve();

		//cout << "curva+: " << i << " generada." << endl;
		for (size_t j = 0; j < numRates + numSwaps; ++j)
		{
			temp.at(j) = _curve->getRateAtIndex(j);
		}
		bumps.at(i) = temp;

		//Bajar 1pip()
		if (i < numRates)
		{
			_inputRates.at(i)->addToRateValue(-2.0 * BP);
		}
		else
		{
			_inputBasisSwaps.at(i - numRates)->getPayoff(_whichBasisLeg)->addToAdditiveSpreadValue(-2.0 * BP);
		}

		for (size_t k = 1; k < numRates + numSwaps; ++k)
		{
			_curve->setOrdinateAtWithValue(static_cast<unsigned long>(k), 0.0);
		}

		generateCurve();
		//cout << "curva -: " << i << " generada." << endl;

		for (size_t j = 0; j < numRates + numSwaps; ++j)
		{
			temp2.at(j) = _curve->getRateAtIndex(j);
		}
		bumps2.at(i) = temp2;

		//Bumpear el input de vuelta a su nivel original
		if (i < numRates)
		{
			_inputRates.at(i)->addToRateValue(BP);
		}
		else
		{
			_inputBasisSwaps.at(i - numRates)->getPayoff(_whichBasisLeg)->addToAdditiveSpreadValue(BP);
		}
	}

	generateCurve();
	//cout << "curva sin bumpear generada." << endl;

	//Finalmente se calculan las derivadas
	_derivatives.resize(numRates + numSwaps);
	for (size_t i = 0; i < numRates + numSwaps; ++i)
	{
		//cout << "curva num: " << i << endl;
		_derivatives.at(i).resize(numRates + numSwaps);
		for (size_t j = 0; j < numRates + numSwaps; ++j)
		{
			_derivatives.at(i).at(j) = (bumps.at(j).at(i) - bumps2.at(j).at(i)) / 2.0;
			//cout << "derivative: " << _derivatives.at(i).at(j) << endl;
		}
	}
}

size_t QCZeroCurveBootstrappingFromRatesAndFloatingLegsProjCurve::getCurveLength()
{
	return _curve->getLength();
}

double QCZeroCurveBootstrappingFromRatesAndFloatingLegsProjCurve::getRateAt(size_t index)
{
	return _curve->getRateAt(static_cast<long>(index));
}

void QCZeroCurveBootstrappingFromRatesAndFloatingLegsProjCurve::calculateDerivativesAt(
	size_t derivativeIndex)
{
	return;
}

double QCZeroCurveBootstrappingFromRatesAndFloatingLegsProjCurve::getDerivativeAt(
	size_t rateIndex, size_t derivativeIndex)
{
	return _derivatives.at(rateIndex).at(derivativeIndex);
}

QCZeroCurveBootstrappingFromRatesAndFloatingLegsProjCurve::~QCZeroCurveBootstrappingFromRatesAndFloatingLegsProjCurve()
{
}
