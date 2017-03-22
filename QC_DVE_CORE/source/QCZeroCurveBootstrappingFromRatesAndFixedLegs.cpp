#include <exception>
#include <algorithm>
#include <string>
#include <functional>

#include "QCZeroCurveBootstrappingFromRatesAndFixedLegs.h"

#define EPSILON 0.000000001

QCZeroCurveBootstrappingFromRatesAndFixedLegs::QCZeroCurveBootstrappingFromRatesAndFixedLegs(
	QCDate valueDate,
	vector<shared_ptr<QCTimeDepositPayoff>> inputRates,
	vector<shared_ptr<QCFixedRatePayoff>> inputFixedRateLegs,
	QCZrCpnCrvShrdPtr curve) :_valueDate(valueDate), _inputRates(inputRates),
	_inputFixedRateLegs(inputFixedRateLegs), _curve(curve)
{
	if (!_checkInputRates())
	{
		throw invalid_argument(_checkInputRatesMsg);
	}

	if (!_checkInputFixedRateLegs())
	{
		throw invalid_argument(_checkInputFixedRateLegsMsg);
	}
}

bool QCZeroCurveBootstrappingFromRatesAndFixedLegs::_checkInputRates()
{
	//Se ordena el vector de tasas iniciales por orden de fecha final ascendente
	//El orden está definido en la sobrecarga del operador < en QCTimeDepositPayoff
	sort(_inputRates.begin(),_inputRates.end(), QCTimeDepositPayoff::lessThan);

	//Se verifica que la fecha inicial de la primera tasa sea igual a valueDate. Esto implica que se supone
	//que vamos a construir una curva cero desde t + 0 = valueDate
	if (get<QCInterestRateLeg::intRtPrdElmntStartDate>(_inputRates.at(0)->getPeriodAt(0)) != _valueDate)
	{
		_checkInputRatesMsg = "La fecha inicial de la primera tasa debe ser igual a valueDate";
		return false;
	}

	//Primero se verifica si los endDate son estrictamente crecientes
	//Si no lo son se arroja una excepción.
	for (unsigned int i = 1; i < _inputRates.size(); ++i)
	{
		if (!(get<QCInterestRateLeg::intRtPrdElmntEndDate>(_inputRates.at(i - 1)->getPeriodAt(0)) <
			get<QCInterestRateLeg::intRtPrdElmntEndDate>(_inputRates.at(i)->getPeriodAt(0))))
		{
			_checkInputRatesMsg = "Las tasas iniciales deben tener fechas finales estrictamente crecientes";
			return false;
		}
	}

	//Luego se chequea si todos los start date son iguales
	bool todosStartDateSonValueDate = true;
	for (unsigned int i = 1; i < _inputRates.size(); ++i)
	{
		if (get<QCInterestRateLeg::intRtPrdElmntStartDate>(_inputRates.at(i)->getPeriodAt(0)) != _valueDate)
		{
			todosStartDateSonValueDate = false;
			break;
		}
	}

	//Si no lo son se verifica que los startDate y endDate estén en una sucesión admisible.
	//	staDate1	endDate1
	//	endDate1	endDate2
	//	endDate2	endDate3
	//	endDate3	endDate4
	//	endDate3	endDate5
	//	endDate3	endDate6 ...
	if (!todosStartDateSonValueDate)
	{
		for (unsigned int i = 1; i < _inputRates.size(); ++i)
		{
			//endDate1 = startDate2
			bool flag1 = get<QCInterestRateLeg::intRtPrdElmntEndDate>(_inputRates.at(i - 1)->getPeriodAt(0)) ==
				get<QCInterestRateLeg::intRtPrdElmntStartDate>(_inputRates.at(i)->getPeriodAt(0));

			//startDate2 = startDate1
			bool flag2 = get<QCInterestRateLeg::intRtPrdElmntStartDate>(_inputRates.at(i - 1)->getPeriodAt(0)) ==
				get<QCInterestRateLeg::intRtPrdElmntStartDate>(_inputRates.at(i)->getPeriodAt(0));

			//Se debe cumplir al menos una de las dos condiciones anteriores
			if (!(flag1 || flag2))
			{
				_checkInputRatesMsg = "Las tasas iniciales deben tener una sucesión de fechas de inicio ";
				_checkInputRatesMsg += "y final admisibles.";
				return false;
			}
		}
	}

	return true;
}

bool QCZeroCurveBootstrappingFromRatesAndFixedLegs::_checkInputFixedRateLegs()
{
	//Se ordena el vector de patas fijas iniciales por orden de fecha final ascendente
	//El orden está definido en la sobrecarga del operador < en QCFixedRatePayoff
	sort(_inputFixedRateLegs.begin(), _inputFixedRateLegs.end(), QCFixedRatePayoff::lessThan);

	//Primero se verifica si los endDate son estrictamente crecientes
	//Si no lo son se retorna false .
	for (unsigned int i = 1; i < _inputFixedRateLegs.size(); ++i)
	{
		unsigned int lastPeriodIndex1 = _inputFixedRateLegs.at(i - 1)->getLastPeriodIndex();
		unsigned int lastPeriodIndex2 = _inputFixedRateLegs.at(i)->getLastPeriodIndex();

		if (!(get<QCInterestRateLeg::intRtPrdElmntEndDate>(_inputFixedRateLegs.at(i - 1)->getPeriodAt(lastPeriodIndex1)) <
			get<QCInterestRateLeg::intRtPrdElmntEndDate>(_inputFixedRateLegs.at(i)->getPeriodAt(lastPeriodIndex2))))
		{
			_checkInputFixedRateLegsMsg = "Los swaps iniciales deben tener fechas finales estrictamente crecientes";
			return false;
		}
	}

	//Luego se chequea si todos los start date son iguales.
	//Si no lo son se retorna falso.
	for (unsigned int i = 1; i < _inputFixedRateLegs.size(); ++i)
	{
		if (get<QCInterestRateLeg::intRtPrdElmntStartDate>(_inputFixedRateLegs.at(i - 1)->getPeriodAt(0)) != 
			get<QCInterestRateLeg::intRtPrdElmntStartDate>(_inputFixedRateLegs.at(i)->getPeriodAt(0)))
		{
			_checkInputFixedRateLegsMsg = "Las fechas iniciales de los swaps no son iguales.";
			return false;
		}
	}

	//Se verifica que el start date de los swaps sea <= que el end date de la tasa más larga.
	unsigned int temp = _inputRates.size();
	QCDate fecha1 = get<QCInterestRateLeg::intRtPrdElmntStartDate>(_inputFixedRateLegs.at(0)->getPeriodAt(0));
	QCDate fecha2 = get<QCInterestRateLeg::intRtPrdElmntEndDate>(_inputRates.at(temp - 1)->getPeriodAt(0));
	if (fecha1 > fecha2)
	{
		_checkInputFixedRateLegsMsg = "El start date de los swaps es mayor que el end date de la ";
		_checkInputFixedRateLegsMsg += "tasa mas larga.";
		return false;
	}

	//Finalmente se verifica que la fecha final del swap más corto sea superior a la fecha final
	//de la tasa más larga.
	fecha1 = get<QCInterestRateLeg::intRtPrdElmntEndDate>(_inputRates.at(temp - 1)->getPeriodAt(0));
	unsigned int lastPeriod = _inputFixedRateLegs.at(0)->getLastPeriodIndex();
	fecha2 = get<QCInterestRateLeg::intRtPrdElmntEndDate>(_inputFixedRateLegs.at(0)->getPeriodAt(lastPeriod));
	if (fecha2 <= fecha1)
	{
		_checkInputFixedRateLegsMsg = "La end date del swap más corto es inferior a la end date ";
		_checkInputFixedRateLegsMsg += "de la tasa más larga.";
		return false;
	}
	return true;
}

void QCZeroCurveBootstrappingFromRatesAndFixedLegs::generateCurve()
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
		while (diff > EPSILON)
		{
			double f{ td->presentValue(true) };
			double df{ td->getPvRateDerivativeAt(rateCounter) };
			rNext = rLast - f / df;
			_curve->setOrdinateAtWithValue(rateCounter, rNext);
			diff = rNext - rLast;
			rLast = rNext;
		}
	}
	
	rateCounter = _inputRates.size() - 1;
	QCDate swapsStartDate{get<QCInterestRateLeg::intRtPrdElmntStartDate>( _inputFixedRateLegs.at(0)->getPeriodAt(0))};
	long plazo{ _valueDate.dayDiff(swapsStartDate) };
	double df{ _curve->getDiscountFactorAt(plazo) };
	for (auto& fr : _inputFixedRateLegs)
	{
		//Conteo de tasas
		rateCounter += 1;

		//Utiliza la tasa del payoff como punto inicial del cálculo
		double rLast{ fr->getRateValue() }; //valor inicial de la tasa a calcular
		_curve->setOrdinateAtWithValue(rateCounter, rLast); //Se modifica la curva con el valor inicial
		double rNext;
		double diff{ 1 };
		while (diff > EPSILON)
		{
			double f{ fr->presentValue(true) - df };
			double df{ fr->getPvRateDerivativeAt(rateCounter) };
			rNext = rLast - f / df;
			_curve->setOrdinateAtWithValue(rateCounter, rNext);
			diff = rNext - rLast;
			rLast = rNext;
		}
	}
	return;
}

unsigned int QCZeroCurveBootstrappingFromRatesAndFixedLegs::getCurveLength()
{
	return 0;
}

double QCZeroCurveBootstrappingFromRatesAndFixedLegs::getRateAt(unsigned int index)
{
	return 0.0;
}

double QCZeroCurveBootstrappingFromRatesAndFixedLegs::getDerivativeAt(
	unsigned int rateIndex, unsigned int derivativeIndex)
{
	return 0.0;
}

QCZeroCurveBootstrappingFromRatesAndFixedLegs::~QCZeroCurveBootstrappingFromRatesAndFixedLegs()
{

}