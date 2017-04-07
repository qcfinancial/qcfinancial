#ifndef QCINTERESTRATECURVEGENERATOR_H
#define QCINTERESTRATECURVEGENERATOR_H

#include <exception>

#include "QCInterestRateCurve.h"
#include "QCTimeDepositPayoff.h"
#include "QCFixedRatePayoff.h"
#include "QCFloatingRatePayoff.h"
#include "QCFXForward.h"

/*!
* @author Alvaro Díaz
* @brief Clase base abstracta para todos los tipos de generadores de curvas de tasas de interés.
* @details Esta clase define varios métodos que las clases derivadas deben implementar.
* Los distintos tipos de algoritmos de bootstrapping se derivan de ésta.
*/
class QCInterestRateCurveGenerator
{
public:
	/*!
	* Constructor.
	* @param valueDate fecha a la cual se va a construir la curva.
	* @param inputRates vector<shared_ptr<QCTimeDepositPayoff>> que representan las tasas cero (depósitos
	* o Libor que se utilizan como datos inciales en el proceso.
	* @param inputFixedRateLegs vector<shared_ptr<QCInterestRatePayoff>> que representan las patas fijas de los
	* swaps que se usan como input del bootstrapping.
	* @param inputForwards vector<shared_ptr<QCFXForward>> que representa forwards usados como input
	* del bootstrapping.
	* @param zeroCurve es un puntero a un objeto de tipo QCZeroCouponCurve que contiene los valores de curva
	* que se van a construir. La curva debe venir inicializada con las dimensiones apropiadas.
	*/
	QCInterestRateCurveGenerator(QCDate valueDate,
		vector<shared_ptr<QCTimeDepositPayoff>> inputRates,
		vector<shared_ptr<QCFixedRatePayoff>> inputFixedRateLegs,
		vector<shared_ptr<QCFXForward>> inputForwards,
		vector<shared_ptr<QCFloatingRatePayoff>> inputFloatingRateLegs,
		QCZrCpnCrvShrdPtr curve) :_valueDate(valueDate), _inputRates(inputRates),
		_inputFixedRateLegs(inputFixedRateLegs), _inputForwards(inputForwards),
		_inputFloatingRateLegs(inputFloatingRateLegs), _curve(curve)
	{
		//Si este input se utiliza
		if (_inputRates.size() > 0)
		{	
			//verificar que es válido
			if (!_checkInputRates())
			{
				throw invalid_argument(_checkInputRatesMsg);
			}
		}

		//Si este input se utiliza
		if (_inputForwards.size() > 0)
		{
			//Verificar que es válido
			if (!_checkInputForwards())
			{
				throw invalid_argument(_checkInputForwardsMsg);
			}
		}

		//Si este input se utiliza
		if (_inputFixedRateLegs.size() > 0)
		{
			//Verificar que es válido
			if (!_checkInputFixedRateLegs())
			{
				throw invalid_argument(_checkInputFixedRateLegsMsg);
			}
		}

		//Si este input se utiliza
		if (_inputFloatingRateLegs.size() > 0)
		{
			//Verificar que es válido
			if (!_checkInputFloatingRateLegs())
			{
				throw invalid_argument(_checkInputFloatingRateLegsMsg);
			}
		}

	}

	/*!
	* Método utilitario que devuelve un vector vacío de shared_ptr<QCTimeDepositPayoff>. Se utilizará
	* en las subclases que no utilicen este input.
	* @return vector<shared_ptr<QCTimeDepositPayoff>> con size() = 0.
	*/
	static vector<shared_ptr<QCTimeDepositPayoff>> emptyRates()
	{
		vector<shared_ptr<QCTimeDepositPayoff>> result;
		return result;
	}

	/*!
	* Método utilitario que devuelve un vector vacío de shared_ptr<QCFXForward>. Se utilizará
	* en las subclases que no utilicen este input.
	* @return vector<shared_ptr<QCFXForward>> con size() = 0.
	*/
	static vector<shared_ptr<QCFXForward>> emptyForward()
	{
		vector<shared_ptr<QCFXForward>> result;
		return result;
	}

	/*!
	* Método utilitario que devuelve un vector vacío de shared_ptr<QCFixedRatePayoff>. Se utilizará
	* en las subclases que no utilicen este input.
	* @return vector<shared_ptr<QCFixedRatePayoff>> con size() = 0.
	*/
	static vector<shared_ptr<QCFixedRatePayoff>> emptyFixedLegs()
	{
		vector<shared_ptr<QCFixedRatePayoff>> result;
		return result;
	}

	/*!
	* Método utilitario que devuelve un vector vacío de shared_ptr<QCFloatingRatePayoff>. Se utilizará
	* en las subclases que no utilicen este input.
	* @return vector<shared_ptr<QCFloatingRatePayoff>> con size() = 0.
	*/
	static vector<shared_ptr<QCFloatingRatePayoff>> emptyFloatingLegs()
	{
		vector<shared_ptr<QCFloatingRatePayoff>> result;
		return result;
	}

	/*!
	* Ejecuta el procedimiento que genera la curva.
	*/
	virtual void generateCurve() = 0;

	/*!
	* Ejecuta el procedimiento que genera la curva y calcula todas las derivadas.
	*/
	virtual void generateCurveAndDerivatives() = 0;

	/*!
	* Calcula el largo de la curva.
	* @return largo de la curva.
	*/
	virtual unsigned int getCurveLength() = 0;

	/*!
	* Devuelve la tasa en la posición i de la curva generada.
	* @param i posición de la tasa deseada.
	* @return valor de la tasa deseada.
	*/
	virtual double getRateAt(size_t index) = 0;

	/*!
	* Calculada la derivada de una tasa generada a partir de uno de los inputs.
	* @param rateIndex índice de la tasa cuya derivada se quiere calcular
	* @param derivativeIndex índice del input que se quiere usar para derivar.
	*/
	virtual void calculateDerivativesAt(size_t derivativeIndex) = 0;

	/*!
	* Devuelve el valor la derivada de una tasa generada a partir de uno de los inputs.
	* No calcula el valor, solo devuelve el valor si ya ha sido calculada (por ejemplo con
	* calculateDerivativeAt( ... ).
	* @param rateIndex índice de la tasa cuya derivada se quiere calcular
	* @param derivativeIndex índice del input que se quiere usar para derivar.
	*/
	virtual double getDerivativeAt(size_t rateIndex, size_t derivativeIndex) = 0;

	/*!
	* Destructor.
	*/
	virtual ~QCInterestRateCurveGenerator() {};

protected:
	/*!
	* Variable donde se almacena la fecha a la cual se construye la curva.
	*/
	QCDate _valueDate;

	/*!
	* Se almacenan las tasas cero cupón utilizadas como input
	*/
	vector<shared_ptr<QCTimeDepositPayoff>> _inputRates;

	/*!
	* Se almacenan las patas fijas utilizadas como input.
	*/
	vector<shared_ptr<QCFixedRatePayoff>> _inputFixedRateLegs;

	/*!
	* Se almacenan los Fx Forwards utilizados como input.
	*/
	vector<shared_ptr<QCFXForward>> _inputForwards;

	/*!
	* Se almacenan las patas flotantes utilizadas como input.
	*/
	vector<shared_ptr<QCFloatingRatePayoff>> _inputFloatingRateLegs;

	/*!
	* Se almacena los curva que se va a construir.
	*/
	QCZrCpnCrvShrdPtr _curve;

	/*!
	* Método privado que verifica si el parámetro inputRates pasado al constructor
	* es admisible.
	* @return verdadero o falso
	*/
	bool _checkInputRates()
	{
		//Se ordena el vector de tasas iniciales por orden de fecha final ascendente
		//El orden está definido en la sobrecarga del operador < en QCTimeDepositPayoff
		sort(_inputRates.begin(), _inputRates.end(), QCTimeDepositPayoff::lessThan);

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
	
	/*!
	* Eventual mensaje de error que genera _checkInputRates()
	*/
	string _checkInputRatesMsg;

	/*!
	* Método privado que verifica si el parámetro inputFixedRateLegs pasado al constructor
	* es admisible.
	* @return verdadero o falso
	*/
	bool _checkInputFixedRateLegs()
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

	/*!
	* Eventual mensaje de error que genera _checkInputRates()
	*/
	string _checkInputFixedRateLegsMsg;

	/*!
	* Método privado que verifica si el parámetro inputFixedRateLegs pasado al constructor
	* es admisible.
	* @return verdadero o falso
	*/
	bool _checkInputForwards()
	{
		//Se ordena el vector de forwards por orden de fecha final ascendente
		//El orden está definido en la sobrecarga del operador < en QCFixedRatePayoff
		sort(_inputForwards.begin(), _inputForwards.end(), QCFXForward::lessThan);

		//Primero se verifica si los endDate son estrictamente crecientes
		//Si no lo son se retorna false .
		for (unsigned int i = 1; i < _inputForwards.size(); ++i)
		{

			if (!(get<QCInterestRateLeg::intRtPrdElmntEndDate>(_inputForwards.at(i - 1)->getPeriod()) <
				get<QCInterestRateLeg::intRtPrdElmntEndDate>(_inputForwards.at(i)->getPeriod())))
			{
				_checkInputForwardsMsg = "Los forwards iniciales deben tener fechas finales estrictamente crecientes";
				return false;
			}
		}

		//Luego se chequea si todos los start date son iguales.
		//Si no lo son se retorna falso.
		for (unsigned int i = 1; i < _inputForwards.size(); ++i)
		{
			if (get<QCInterestRateLeg::intRtPrdElmntStartDate>(_inputForwards.at(i - 1)->getPeriod()) !=
				get<QCInterestRateLeg::intRtPrdElmntStartDate>(_inputForwards.at(i)->getPeriod()))
			{
				_checkInputForwardsMsg = "Las fechas iniciales de los forwards no son iguales.";
				return false;
			}
		}

		//Se verifica que el start date de los forwards sea <= que el end date de la tasa más larga.
		if (_inputRates.size() > 0)
		{
			unsigned int temp = _inputRates.size();
			QCDate fecha1 = get<QCInterestRateLeg::intRtPrdElmntStartDate>(
				_inputForwards.at(0)->getPeriod());
			QCDate fecha2 = get<QCInterestRateLeg::intRtPrdElmntEndDate>(
				_inputRates.at(temp - 1)->getPeriodAt(0));
			if (fecha1 > fecha2)
			{
				_checkInputForwardsMsg = "El start date de los forwards es mayor que el end date de la ";
				_checkInputForwardsMsg += "tasa mas larga.";
				return false;
			}

			//Finalmente se verifica que la fecha final del forward más corto sea superior a la fecha final
			//de la tasa más larga.
			fecha1 = get<QCInterestRateLeg::intRtPrdElmntEndDate>(_inputRates.at(temp - 1)->getPeriodAt(0));
			fecha2 = get<QCInterestRateLeg::intRtPrdElmntEndDate>(
				_inputForwards.at(0)->getPeriod());
			if (fecha2 <= fecha1)
			{
				_checkInputForwardsMsg = "La end date del forward más corto es inferior a la end date ";
				_checkInputForwardsMsg += "de la tasa más larga.";
				return false;
			}
		}

		return true;
	}

	/*!
	* Eventual mensaje de error que genera _checkInputRates()
	*/
	string _checkInputForwardsMsg;

	/*!
	* Método privado que verifica si el parámetro inputFloatingRateLegs pasado al constructor
	* es admisible.
	* @return verdadero o falso
	*/
	bool _checkInputFloatingRateLegs()
	{
		//Se ordena el vector de patas flotantes por orden de fecha final ascendente
		//El orden está definido en la sobrecarga del operador < en QCFloatingRatePayoff
		sort(_inputFloatingRateLegs.begin(), _inputFloatingRateLegs.end(), QCFloatingRatePayoff::lessThan);

		//Primero se verifica si los endDate son estrictamente crecientes
		//Si no lo son se retorna false .
		for (size_t i = 1; i < _inputFloatingRateLegs.size(); ++i)
		{

			if (!(get<QCInterestRateLeg::intRtPrdElmntEndDate>(
				_inputFloatingRateLegs.at(i - 1)->getPeriodAt(_inputFloatingRateLegs.at(i - 1)->getLastPeriodIndex())) <
				get<QCInterestRateLeg::intRtPrdElmntEndDate>(
				_inputFloatingRateLegs.at(i)->getPeriodAt(_inputFloatingRateLegs.at(i)->getLastPeriodIndex()))))
			{
				_checkInputFloatingRateLegsMsg = "Las patas flotantes iniciales deben tener fechas finales estrictamente crecientes";
				return false;
			}
		}

		//Luego se chequea si todos los start date son iguales.
		//Si no lo son se retorna falso.
		for (size_t i = 1; i < _inputFloatingRateLegs.size(); ++i)
		{
			if (get<QCInterestRateLeg::intRtPrdElmntStartDate>(_inputFloatingRateLegs.at(i - 1)->getPeriodAt(0)) !=
				get<QCInterestRateLeg::intRtPrdElmntStartDate>(_inputFloatingRateLegs.at(i)->getPeriodAt(0)))
			{
				_checkInputFloatingRateLegsMsg = "Las fechas iniciales de las patas flotantes no son iguales.";
				return false;
			}
		}

		//Se verifica que el start date de las patas flotantes sea <= que el end date de la tasa más larga.
		if (_inputRates.size() > 0)
		{
			unsigned int temp = _inputRates.size();
			/*
			QCDate fecha1 = get<QCInterestRateLeg::intRtPrdElmntStartDate>(
				_inputFloatingRateLegs.at(0)->getPeriodAt(0));
			QCDate fecha2 = get<QCInterestRateLeg::intRtPrdElmntEndDate>(
				_inputRates.at(temp - 1)->getPeriodAt(0));
			if (fecha1 > fecha2)
			{
				_checkInputFloatingRateLegsMsg = "El start date de las patas flotantes es mayor que el end date de la ";
				_checkInputFloatingRateLegsMsg += "tasa mas larga.";
				return false;
			}
			*/
			//Finalmente se verifica que la fecha final de la pata flotante más corta sea superior a la fecha final
			//de la tasa más larga.
			QCDate fecha1 = get<QCInterestRateLeg::intRtPrdElmntEndDate>(_inputRates.at(temp - 1)->getPeriodAt(0));
			QCDate fecha2 = get<QCInterestRateLeg::intRtPrdElmntEndDate>(
				_inputFloatingRateLegs.at(0)->getPeriodAt(0));
			if (fecha2 <= fecha1)
			{
				_checkInputFloatingRateLegsMsg = "La end date de la pata flotante más corta es inferior a la end date ";
				_checkInputFloatingRateLegsMsg += "de la tasa más larga.";
				return false;
			}
		}

		return true;
	}

	/*!
	* Eventual mensaje de error que genera _checkInputRates()
	*/
	string _checkInputFloatingRateLegsMsg;

	/*!
	* Matriz con las derivadas de cada tasa cero cupón respecto a cada uno de los inputs
	* de la curva. Si i es el índice de tasa cero cupón y j es el índice de input de curva,
	* el elemento d(i,j) de la matriz (fila i columna j) tiene la derivada de la tasa cero i
	* respecto al input j.
	*/
	vector<vector<double>> _derivatives;
};

#endif //QCINTERESTRATECURVEGENERATOR_H