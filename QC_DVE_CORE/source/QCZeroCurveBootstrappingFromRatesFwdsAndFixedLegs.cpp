#include "QCZeroCurveBootstrappingFromRatesFwdsAndFixedLegs.h"

const double EPSILON = 0.00000000001;
const double BP = 0.0001;

QCZeroCurveBootstrappingFromRatesFwdsAndFixedLegs::QCZeroCurveBootstrappingFromRatesFwdsAndFixedLegs(
	QCDate valueDate,
	vector<shared_ptr<QCTimeDepositPayoff>> inputRates,
	vector<shared_ptr<QCFXForward>> inputForwards,
	unsigned int whichForwardLeg,
	vector<shared_ptr<QCFixedRatePayoff>> inputFixedRateLegs,
	QCZrCpnCrvShrdPtr newZeroCurve) : QCInterestRateCurveGenerator(
	valueDate,
	inputRates,
	inputFixedRateLegs,
	inputForwards,
	QCInterestRateCurveGenerator::emptyFloatingLegs(),
	QCInterestRateCurveGenerator::emptyBasisSwaps(),
	newZeroCurve)
{
	if (whichForwardLeg > 1)
	{
		throw invalid_argument("Number of Fx Forward leg must be 0 or 1.");
	}
	_whichForwardLeg = whichForwardLeg;
}

void QCZeroCurveBootstrappingFromRatesFwdsAndFixedLegs::generateCurve()
{
	//Loop sobre las tasas
	size_t rateCounter = -1; //Valor inicial del contador de tasas calculadas de la curva
	QCDate startDate{ get<QCInterestRateLeg::intRtPrdElmntStartDate>(
		_inputRates.at(0)->getPeriodAt(0)) }; //Fecha inicial de todas las tasas que vamos a calcular
	for (auto& td : _inputRates)
	{
		//Conteo de tasas
		rateCounter += 1;

		//Utiliza la tasa del payoff como punto inicial del c�lculo
		double rLast{ td->getRateValue() }; //valor inicial de la tasa a calcular
		_curve->setOrdinateAtWithValue(static_cast<unsigned long>(rateCounter), rLast); //Se modifica la curva con el valor inicial
		double rNext;
		double diff{ 1 };
		while (abs(diff) > EPSILON)
		{
			double f{ td->presentValue(true) };
			double df{ td->getPvRateDerivativeAt(static_cast<unsigned int>(rateCounter)) };
			rNext = rLast - f / df;
			_curve->setOrdinateAtWithValue(static_cast<unsigned long>(rateCounter), rNext);
			diff = rNext - rLast;
			rLast = rNext;
		}
	}

	rateCounter = _inputRates.size() - 1;
	QCDate fwdsStartDate{ get<QCInterestRateLeg::intRtPrdElmntStartDate>(
		_inputForwards.at(0)->getPeriod()) };
	long plazo{ _valueDate.dayDiff(fwdsStartDate) };
	for (auto& fr : _inputForwards)
	{
		//Conteo de tasas
		rateCounter += 1;

		//Utiliza la tasa del payoff como punto inicial del c�lculo
		double rLast{ .01 }; //valor inicial de la tasa a calcular
		_curve->setOrdinateAtWithValue(static_cast<unsigned long>(rateCounter), rLast); //Se modifica la curva con el valor inicial
		double rNext;
		double diff{ 1 };
		while (abs(diff) > EPSILON)
		{
			double f{ fr->marketValue() };
			double df{ fr->getPvRateDerivativeAtFromLeg(static_cast<unsigned int>(rateCounter), _whichForwardLeg) };
			rNext = rLast - f / df;
			_curve->setOrdinateAtWithValue(static_cast<unsigned long>(rateCounter), rNext);
			diff = rNext - rLast;
			rLast = rNext;
		}
	}

	rateCounter = _inputRates.size() + _inputForwards.size() - 1;
	QCDate swapsStartDate{ get<QCInterestRateLeg::intRtPrdElmntStartDate>(_inputFixedRateLegs.at(0)->getPeriodAt(0)) };
	plazo = _valueDate.dayDiff(swapsStartDate) ;
	double df{ _curve->getDiscountFactorAt(plazo) };
	for (auto& fr : _inputFixedRateLegs)
	{
		//Conteo de tasas
		rateCounter += 1;

		//Utiliza la tasa del payoff como punto inicial del c�lculo
		double rLast{ fr->getRateValue() }; //valor inicial de la tasa a calcular
		_curve->setOrdinateAtWithValue(static_cast<unsigned long>(rateCounter), rLast); //Se modifica la curva con el valor inicial
		double rNext;
		double diff{ 1 };
		while (abs(diff) > EPSILON)
		{
			double f{ fr->presentValue(true) - df };
			double df{ fr->getPvRateDerivativeAt(static_cast<unsigned int>(rateCounter)) };
			rNext = rLast - f / df;
			_curve->setOrdinateAtWithValue(static_cast<unsigned long>(rateCounter), rNext);
			diff = rNext - rLast;
			rLast = rNext;
		}
	}
	return;

}

void QCZeroCurveBootstrappingFromRatesFwdsAndFixedLegs::generateCurveAndDerivatives()
{
	size_t numRates{ _inputRates.size() };
	size_t numFwds{ _inputForwards.size() };
	size_t numSwaps{ _inputFixedRateLegs.size() };

	//Aqui guardamos todas las curvas bumpeadas
	vector<vector<double>> bumps;
	bumps.resize(numRates + numFwds + numSwaps);
	
	vector<vector<double>> bumps2;
	bumps2.resize(numRates + numFwds + numSwaps);
	
	vector<double> temp;
	temp.resize(numRates + numFwds + numSwaps);
	
	vector<double> temp2;
	temp2.resize(numRates + numFwds + numSwaps);
	for (size_t i = 0; i < numRates + numFwds + numSwaps; ++i)
	{
		// cout << "iteracion i = " << i << endl;
		//Bumpear el input que corresponde
		if (i < numRates)
		{
			_inputRates.at(i)->addToRateValue(BP);
		}
		else if (i == numRates || i < numRates + numFwds)
		{
			// cout << "Estoy en el fwd: " << i << endl;
		}
		else
		{
			// cout << "Estoy aqui? : " << i << endl;
			_inputFixedRateLegs.at(i - numRates - numFwds)->addToRateValue(BP);
		}

		//Ejecutar generateCurve() y guardar los valores
		for (size_t k = 0; k < numRates + numFwds + numSwaps; ++k)
		{
			_curve->setOrdinateAtWithValue(static_cast<unsigned long>(k), 0.0);
		}
		
		generateCurve();

		// cout << "curva +: " << i << " generada." << endl;
		for (size_t j = 0; j < numRates + numFwds + numSwaps; ++j)
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
		else if (i == numRates || i < numRates + numFwds)
		{
		}
		else
		{
			_inputFixedRateLegs.at(i - numRates - numFwds)->addToRateValue(-2.0 * BP);
		}
		for (size_t k = 0; k < numRates + numFwds + numSwaps; ++k)
		{
			_curve->setOrdinateAtWithValue(static_cast<unsigned long>(k), 0.0);
		}
		
		generateCurve();
		// cout << "curva -: " << i << " generada." << endl;

		for (size_t j = 0; j < numRates + numFwds + numSwaps; ++j)
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
		else if (i == numRates || i < numRates + numFwds)
		{
		}
		else
		{
			_inputFixedRateLegs.at(i - numRates - numFwds)->addToRateValue(BP);
		}

	}

	//Se genera la curva con los inputs sin bumpear
	generateCurve();

	//Finalmente se calculan las derivadas
	_derivatives.resize(numRates + numFwds + numSwaps);
	for (size_t i = 0; i < numRates + numFwds + numSwaps; ++i)
	{
		//cout << "curva num: " << i << endl;
		_derivatives.at(i).resize(numRates + numFwds + numSwaps);
		for (size_t j = 0; j < numRates + numFwds + numSwaps; ++j)
		{
			_derivatives.at(i).at(j) = (bumps.at(j).at(i) - bumps2.at(j).at(i)) / 2.0;// -_curve->getRateAt(j);
			// cout << "derivative: " << _derivatives.at(i).at(j) << endl;
		}
	}


}

size_t QCZeroCurveBootstrappingFromRatesFwdsAndFixedLegs::getCurveLength()
{
	return 0;
}

double QCZeroCurveBootstrappingFromRatesFwdsAndFixedLegs::getRateAt(size_t index)
{
	return 0.0;
}

void QCZeroCurveBootstrappingFromRatesFwdsAndFixedLegs::calculateDerivativesAt(
	size_t derivativeIndex)
{
	return;
}

double QCZeroCurveBootstrappingFromRatesFwdsAndFixedLegs::getDerivativeAt(
	size_t rateIndex, size_t derivativeIndex)
{
	return _derivatives.at(rateIndex).at(derivativeIndex);
}

QCZeroCurveBootstrappingFromRatesFwdsAndFixedLegs::~QCZeroCurveBootstrappingFromRatesFwdsAndFixedLegs()
{
}
