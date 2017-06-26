#include "QCZeroCurveBootstrappingFromRatesAndFloatingLegsProjCurve.h"

const double EPSILON = 0.00000000001;
const double BP = 0.0001;

QCZeroCurveBootstrappingFromRatesAndFloatingLegsProjCurve::QCZeroCurveBootstrappingFromRatesAndFloatingLegsProjCurve(
	QCDate valueDate,
	vector<shared_ptr<QCTimeDepositPayoff>> inputRates,
	vector<shared_ptr<QCInterestRateBasisSwap>> inputBasisSwaps,
	shared_ptr<QCZeroCouponCurve> newZeroCurve) : QCInterestRateCurveGenerator(
	valueDate,
	inputRates,
	QCInterestRateCurveGenerator::emptyFixedLegs(),
	QCInterestRateCurveGenerator::emptyForward(),
	QCInterestRateCurveGenerator::emptyFloatingLegs(),
	inputBasisSwaps,
	newZeroCurve)
{

}

void QCZeroCurveBootstrappingFromRatesAndFloatingLegsProjCurve::generateCurve()
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

	//Loop sobre las patas flotantes

	rateCounter = _inputRates.size() - 1;
	QCDate swapsStartDate{ get<QCInterestRateLeg::intRtPrdElmntStartDate>(
		_inputFloatingRateLegs.at(0)->getPeriodAt(0)) };
	auto plazo = _valueDate.dayDiff(swapsStartDate);
	double df{ _curve->getDiscountFactorAt(plazo) };
	for (auto& fr : _inputFloatingRateLegs)
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

void QCZeroCurveBootstrappingFromRatesAndFloatingLegsProjCurve::generateCurveAndDerivatives()
{
	size_t numRates{ _inputRates.size() };
	size_t numSwaps{ _inputFloatingRateLegs.size() };

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
		cout << "iteracion i = " << i << endl;
		//Bumpear el input que corresponde
		if (i < numRates)
		{
			_inputRates.at(i)->addToRateValue(BP);
		}
		else
		{
			_inputFloatingRateLegs.at(i - numRates)->addToAdditiveSpreadValue(BP);
		}

		//Ejecutar generateCurve() y guardar los valores
		for (size_t k = 0; k < numRates + numSwaps; ++k)
		{
			_curve->setOrdinateAtWithValue(k, 0.0);
		}

		generateCurve();

		cout << "curva +: " << i << " generada." << endl;
		for (size_t j = 0; j < numRates + numSwaps; ++j)
		{
			temp.at(j) = _curve->getRateAtIndex(j);
			//cout << "temp.at_" << j << " : " << temp.at(j) << endl;
		}
		bumps.at(i) = temp;

		//Bajar 1pip()
		if (i < numRates)
		{
			_inputRates.at(i)->addToRateValue(-2.0 * BP);
		}
		else
		{
			_inputFloatingRateLegs.at(i - numRates)->addToAdditiveSpreadValue(-2.0 * BP);
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
		else if (i == numRates || i < numRates)
		{
		}
		else
		{
			_inputFloatingRateLegs.at(i - numRates)->addToAdditiveSpreadValue(BP);
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
			_derivatives.at(i).at(j) = (bumps.at(j).at(i) - bumps2.at(j).at(i)) / 2.0;// -_curve->getRateAt(j);
			cout << "derivative: " << _derivatives.at(i).at(j) << endl;
		}
	}

}

unsigned int QCZeroCurveBootstrappingFromRatesAndFloatingLegsProjCurve::getCurveLength()
{
	return _curve->getLength();
}

double QCZeroCurveBootstrappingFromRatesAndFloatingLegsProjCurve::getRateAt(unsigned int index)
{
	return _curve->getRateAt(index);
}

void QCZeroCurveBootstrappingFromRatesAndFloatingLegsProjCurve::calculateDerivativesAt(
	size_t derivativeIndex)
{
	return;
}

double QCZeroCurveBootstrappingFromRatesAndFloatingLegsProjCurve::getDerivativeAt(
	unsigned int rateIndex, unsigned int derivativeIndex)
{
	return _derivatives.at(rateIndex).at(derivativeIndex);
}

QCZeroCurveBootstrappingFromRatesAndFloatingLegsProjCurve::~QCZeroCurveBootstrappingFromRatesAndFloatingLegsProjCurve()
{
}
