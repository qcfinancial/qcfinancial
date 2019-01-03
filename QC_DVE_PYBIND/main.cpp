#include <exception>
#include <memory>
#include <string>
#include <map>
#include <iostream>

#ifdef _DEBUG
#define _DEBUG_WAS_DEFINED 1
#undef _DEBUG
#endif

#include <Python.h>

#ifdef _DEBUG_WAS_DEFINED
#define _DEBUG 1
#endif

#include "include.h"
#include "QCCurrencyConverter.h"
#include "QCDate.h"
#include "QCDvePyBindHelperFunctions.h"
#include "QCZeroCouponCurve.h"
#include "QCHelperFunctions.h"
#include "QCFactoryFunctions.h"
#include "QCFixedRatePayoff.h"
#include "QCFloatingRatePayoff.h"
#include "QCIcpClfPayoff.h"
#include "QCIcpClpPayoff.h"
#include "QCDiscountBondPayoff.h"
#include "QCTimeDepositPayoff.h"
#include "QCZeroCurveBootstrappingFromRatesAndFixedLegs.h"
#include "QCZeroCurveBootstrappingFromRatesFwdsAndFixedLegs.h"
#include "QCZeroCurveBootstrappingFromRatesFwdsAndFloatingLegs.h"
#include "QCZeroCurveBootstrappingFromRates.h"
#include "QCZeroCurveBootstrappingFromRatesAndFloatingLegsProjCurve.h"

using namespace std;
#define BASIS_POINT .0001

static PyObject* qcDveError;

//PyObject define un objeto Python o utilizable por python
PyObject* some_function(PyObject* self, PyObject* args)
{
	int i;
	int j;


	//PyArg_ParseTuple obtiene los argumentos Python (argumentos que
	//le entregamos a la funcion desde python) y las tranforma a objetos C
	//(son los valores alojados como referencia en la función).
	//El retorno es TRUE si todos los argumentos estan bien identificados como 
	//objetos C
	//
	//i dice que la entrada es un int y si pongo ii implica que la funcion tiene
	//dos argumentos y que los dos son int
	//
	//f: float
	//d: double
	//i: int
	//I: unsignedInt
	//c: char
	//s: (string or Unicode) [const char *]
	//S: (string)[PyStringObject *]
	//Extrañamente no encuentro BOOLEAN
	//|: el resto de los argumentos son opcionales
	//(items): tuple
	//[items]: list
	//{items}: dictionary
	//... Hay más https://docs.python.org/2/c-api/arg.html#c.PyArg_ParseTuple

	if (!PyArg_ParseTuple(args, "ii", &i, &j))
	{
		goto error;
	}

	//PyInt_FromLong transforma un valor C en un int Python
	//Py_BuildValue("i", i + j); que no es utilizado acá realiza la misma acción 
	// que la funcion anterior solo que en este caso es mas generica ya 
	// que utiliza los mismo identificadores de variables que PyArg_ParseTuple
	//
	//Si la funcion fuera void se debe realizar:
	//Py_INCREF(Py_None);
	//return Py_None;

	return PyInt_FromLong(i + j);

error:
	return 0;
}

//

PyObject* cashflow_fixed_mortgages(PyObject* self, PyObject*  args)
{
	char* fecha;
	PyObject* holidays;
	PyObject* curveValues;
	PyObject* curveCharacteristics;
	PyObject* legCharacteristics;

	if (!PyArg_ParseTuple(args,
		"sO!O!O!O!",
		&fecha,
		&PyList_Type, &holidays,
		&PyList_Type, &curveValues,
		&PyList_Type, &curveCharacteristics,
		&PyList_Type, &legCharacteristics))
	{
		return NULL;
	}
	try
	{
		map<string, vector<QCDate>> mapHolidays;
		QCDvePyBindHelperFunctions::buildHolidays(holidays, mapHolidays);

		map<string, pair<vector<long>, vector<double>>> crvValues;
		QCDvePyBindHelperFunctions::buildCurveValues(curveValues, crvValues);

		map<string, QCDvePyBindHelperFunctions::string4> crvChars;
		QCDvePyBindHelperFunctions::buildCurveCharacteristics(curveCharacteristics, crvChars);

		map<string, shared_ptr<QCZeroCouponCurve>> allCurves;

		//Loopeo sobre los keys de crvValues
		for (const auto &curva : crvValues)
		{
			vector<long> tmpLng{ curva.second.first };
			vector<double> tmpDbl{ curva.second.second };

			string wf = get<1>(crvChars.at(curva.first));
			QCHelperFunctions::lowerCase(wf);

			string yf = get<2>(crvChars.at(curva.first));
			QCHelperFunctions::lowerCase(yf);

			shared_ptr<QCZeroCouponCurve> tmpCrv = QCFactoryFunctions::zrCpnCrvShrdPtr(
				tmpLng,
				tmpDbl,
				get<0>(crvChars.at(curva.first)),
				wf, yf, get<3>(crvChars.at(curva.first)));
			allCurves.insert(pair <string, shared_ptr<QCZeroCouponCurve>>(curva.first, tmpCrv));
		}

		//Ahora hay que construir QCInterestRatePayoff para cada operacion
		//Se requiere:
		//	shared_ptr<QCInterestRate>
		//	shared_ptr<QCInterestRateLeg>
		//	QCDate (valueDate)
		//	shared_ptr<QCZeroCouponCurve> (la curva de descuento)
		//	shared_ptr<QCTimeSeries> (los fixings)

		//La info esta en la list legCharacteristics:
		//	0:	id_leg
		//	1:	rec_pay
		//	2:	start_date
		//	3:	end_date
		//	4:	settlement_calendar
		//	5:	settlement_lag
		//	6:	stub_period
		//	7:	periodicity
		//	8:	end_date_adjustment
		//	9:	amortization
		//	10: fixed_rate
		//	11: notional
		//	12: wealth_factor
		//	13: year_fraction
		//	14: discount_curve

		string strDate{ fecha };
		QCDate allValueDate{ strDate };

		map <long, shared_ptr<QCInterestRatePayoff>> payoffs;
		for (unsigned long i = 0; i < PyList_Size(legCharacteristics); ++i)
		{
			string wf = string(PyString_AsString(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 12)));
			QCHelperFunctions::lowerCase(wf);
			string yf = string(PyString_AsString(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 13)));
			QCHelperFunctions::lowerCase(yf);
			shared_ptr<QCInterestRate> tmpIntRate = QCFactoryFunctions::intRateSharedPtr(
				PyFloat_AsDouble(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 10)),
				yf, wf);
			long numOp = (long)PyFloat_AsDouble(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 0));
			vector<tuple<QCDate, double, double>> amortIfCustom;
			QCInterestRateLeg tmpIntRateLeg = QCFactoryFunctions::buildFrenchFixedRateLeg(
				allValueDate,								//value date
				string(PyString_AsString(PyList_GetItem(
				PyList_GetItem(legCharacteristics, i), 1))),//receive or pay
				QCDate{ string(PyString_AsString(PyList_GetItem(
				PyList_GetItem(legCharacteristics, i), 2))) }, //start date
				QCDate{ string(PyString_AsString(PyList_GetItem(
				PyList_GetItem(legCharacteristics, i), 3))) }, //end date
				mapHolidays.at(string(PyString_AsString(
				PyList_GetItem(PyList_GetItem(legCharacteristics, i), 4)))),  //settlement calendar
				PyInt_AsLong(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 5)), //settlement lag
				QCHelperFunctions::stringToQCStubPeriod(
				string(PyString_AsString(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 6)))), //stub period
				string(PyString_AsString(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 7))), //periodicity
				QCHelperFunctions::stringToQCBusDayAdjRule(
				string(PyString_AsString(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 8)))),//end date adjustment
				QCHelperFunctions::stringToQCAmortization(
				string(PyString_AsString(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 9)))), //amortization
				amortIfCustom,										//amortization and notional by end date
				PyFloat_AsDouble(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 11)),	//notional
				tmpIntRate			//interest rate
				);
			shared_ptr<QCInterestRatePayoff> tmpIntRatePayoff = shared_ptr<QCInterestRatePayoff>(
				new QCFixedRatePayoff{ tmpIntRate, make_shared<QCInterestRateLeg>(tmpIntRateLeg),
				allCurves.at(string(PyString_AsString(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 14)))),
				allValueDate, nullptr });
			payoffs.insert(pair<long, shared_ptr<QCInterestRatePayoff>>(numOp, tmpIntRatePayoff));
		}

		//Loopear sobre cada payoff y guardar los cashflows
		vector<pair<long, vector<tuple<QCDate, QCCashFlowLabel, double>>>> result;
		long numPayoffs = payoffs.size();
		result.resize(numPayoffs);
		cout << "Numero de hipotecas: " << numPayoffs << endl;
		cout << "Inicio generacion de flujos de hipotecas" << endl;
		long counter = 0;
		for (const auto& payoff : payoffs)
		{
			vector<tuple<QCDate, QCCashFlowLabel, double>> tempFlows;
			payoff.second->payoff();
			long numFlows = payoff.second->payoffSize();
			tempFlows.resize(numFlows);
			for (long i = 0; i < numFlows; ++i)
			{
				tempFlows.at(i) = payoff.second->getCashflowAt(i);
			}
			result.at(counter) = make_pair(payoff.first, tempFlows);
			++counter;
		}
		
		PyObject* allMortgages = PyList_New(numPayoffs);
		for (long i = 0; i < numPayoffs; ++i)
		{
			int numFlows = result.at(i).second.size();
			PyObject* thisMortgage = PyList_New(numFlows);
			int success;
			for (int j = 0; j < numFlows; ++j)
			{
				PyObject* oneFlow = PyList_New(4);
				
				success = PyList_SetItem(oneFlow, 0, PyInt_FromLong(result.at(i).first));
				
				success = PyList_SetItem(oneFlow, 1, PyString_FromString(
					get<0>(result.at(i).second.at(j)).description().c_str()));
				
				success = PyList_SetItem(oneFlow, 2, PyInt_FromLong(
					get<1>(result.at(i).second.at(j))));
				
				success = PyList_SetItem(oneFlow, 3, PyFloat_FromDouble(
					get<2>(result.at(i).second.at(j))));
				
				success = PyList_SetItem(thisMortgage, j, oneFlow);
			}
			success = PyList_SetItem(allMortgages, i, thisMortgage);
		}

		cout << "Fin generacion de flujos de hipotecas." << endl;
		return allMortgages;
	}
	catch (exception& e)
	{
		string msg = "Error en las hipotecas. " + string(e.what());
		PyErr_SetString(qcDveError, msg.c_str());
		return NULL;
	}
}

PyObject* cashflow_fixed_swaps(PyObject* self, PyObject*  args)
{
	char* fecha;
	PyObject* holidays;
	PyObject* curveValues;
	PyObject* curveCharacteristics;
	PyObject* legCharacteristics;

	if (!PyArg_ParseTuple(args,
		"sO!O!O!O!",
		&fecha,
		&PyList_Type, &holidays,
		&PyList_Type, &curveValues,
		&PyList_Type, &curveCharacteristics,
		&PyList_Type, &legCharacteristics))
	{
		return NULL;
	}
	try
	{
		map<string, vector<QCDate>> mapHolidays;
		QCDvePyBindHelperFunctions::buildHolidays(holidays, mapHolidays);

		map<string, pair<vector<long>, vector<double>>> crvValues;
		QCDvePyBindHelperFunctions::buildCurveValues(curveValues, crvValues);

		map<string, QCDvePyBindHelperFunctions::string4> crvChars;
		QCDvePyBindHelperFunctions::buildCurveCharacteristics(curveCharacteristics, crvChars);

		map<string, shared_ptr<QCZeroCouponCurve>> allCurves;

		//Loopeo sobre los keys de crvValues
		for (const auto &curva : crvValues)
		{
			vector<long> tmpLng{ curva.second.first };
			vector<double> tmpDbl{ curva.second.second };

			string wf = get<1>(crvChars.at(curva.first));
			QCHelperFunctions::lowerCase(wf);

			string yf = get<2>(crvChars.at(curva.first));
			QCHelperFunctions::lowerCase(yf);

			shared_ptr<QCZeroCouponCurve> tmpCrv = QCFactoryFunctions::zrCpnCrvShrdPtr(
				tmpLng,
				tmpDbl,
				get<0>(crvChars.at(curva.first)),
				wf, yf, get<3>(crvChars.at(curva.first)));
			allCurves.insert(pair <string, shared_ptr<QCZeroCouponCurve>>(curva.first, tmpCrv));
		}

		//Ahora hay que construir QCInterestRatePayoff para cada operacion
		//Se requiere:
		//	shared_ptr<QCInterestRate>
		//	shared_ptr<QCInterestRateLeg>
		//	QCDate (valueDate)
		//	shared_ptr<QCZeroCouponCurve> (la curva de descuento)
		//	shared_ptr<QCTimeSeries> (los fixings)

		//La info esta en la list legCharacteristics:
		//	0:	id_leg
		//	1:	rec_pay
		//	2:	start_date
		//	3:	end_date
		//	4:	settlement_calendar
		//	5:	settlement_lag
		//	6:	stub_period
		//	7:	periodicity
		//	8:	end_date_adjustment
		//	9:	amortization
		//	10: fixed_rate
		//	11: notional
		//	12: wealth_factor
		//	13: year_fraction
		//	14: discount_curve
		//	15: currency

		string strDate{ fecha };
		QCDate allValueDate{ strDate };

		map <long, shared_ptr<QCInterestRatePayoff>> payoffs;
		map <long, string> currencies;
		map <long, string> receivePay;
		for (unsigned long i = 0; i < PyList_Size(legCharacteristics); ++i)
		{
			string wf = string(PyString_AsString(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 12)));
			QCHelperFunctions::lowerCase(wf);
			string yf = string(PyString_AsString(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 13)));
			QCHelperFunctions::lowerCase(yf);
			shared_ptr<QCInterestRate> tmpIntRate = QCFactoryFunctions::intRateSharedPtr(
				PyFloat_AsDouble(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 10)),
				yf, wf);
			long numOp = (long)PyFloat_AsDouble(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 0));
			vector<tuple<QCDate, double, double>> amortIfCustom;
			QCInterestRateLeg tmpIntRateLeg = QCFactoryFunctions::buildFixedRateLeg3(
				allValueDate,								//value date
				string(PyString_AsString(PyList_GetItem(
				PyList_GetItem(legCharacteristics, i), 1))),//receive or pay
				QCDate{ string(PyString_AsString(PyList_GetItem(
				PyList_GetItem(legCharacteristics, i), 2))) }, //start date
				QCDate{ string(PyString_AsString(PyList_GetItem(
				PyList_GetItem(legCharacteristics, i), 3))) }, //end date
				mapHolidays.at(string(PyString_AsString(
				PyList_GetItem(PyList_GetItem(legCharacteristics, i), 4)))),  //settlement calendar
				PyInt_AsLong(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 5)), //settlement lag
				QCHelperFunctions::stringToQCStubPeriod(
				string(PyString_AsString(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 6)))), //stub period
				string(PyString_AsString(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 7))), //periodicity
				QCHelperFunctions::stringToQCBusDayAdjRule(
				string(PyString_AsString(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 8)))),//end date adjustment
				QCHelperFunctions::stringToQCAmortization(
				string(PyString_AsString(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 9)))), //amortization
				amortIfCustom,										//amortization and notional by end date
				PyFloat_AsDouble(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 11)),	//notional
				tmpIntRate			//interest rate
				);
			shared_ptr<QCInterestRatePayoff> tmpIntRatePayoff = shared_ptr<QCInterestRatePayoff>(
				new QCFixedRatePayoff{ tmpIntRate, make_shared<QCInterestRateLeg>(tmpIntRateLeg),
				allCurves.at(string(PyString_AsString(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 14)))),
				allValueDate, nullptr });
			payoffs.insert(pair<long, shared_ptr<QCInterestRatePayoff>>(numOp, tmpIntRatePayoff));
			currencies.insert(pair<long, string>(numOp, string(PyString_AsString(PyList_GetItem(
				PyList_GetItem(legCharacteristics, i), 15)))));
			receivePay.insert(pair<long, string>(numOp, string(PyString_AsString(PyList_GetItem(
				PyList_GetItem(legCharacteristics, i), 1)))));
		}

		//Loopear sobre cada payoff y guardar los cashflows
		vector<pair<long, vector<tuple<QCDate, QCCashFlowLabel, double>>>> result;
		long numPayoffs = payoffs.size();
		result.resize(numPayoffs);
		cout << "Numero de patas fijas de swaps: " << numPayoffs << endl;
		cout << "Inicio generacion de flujos de patas fijas." << endl;
		long counter = 0;
		for (const auto& payoff : payoffs)
		{
			vector<tuple<QCDate, QCCashFlowLabel, double>> tempFlows;
			payoff.second->payoff();
			long numFlows = payoff.second->payoffSize();
			tempFlows.resize(numFlows);
			for (long i = 0; i < numFlows; ++i)
			{
				tempFlows.at(i) = payoff.second->getCashflowAt(i);
			}
			result.at(counter) = make_pair(payoff.first, tempFlows);
			++counter;
		}

		PyObject* allMortgages = PyList_New(numPayoffs);
		for (long i = 0; i < numPayoffs; ++i)
		{
			int numFlows = result.at(i).second.size();
			PyObject* thisMortgage = PyList_New(numFlows);
			int success;
			string currency = currencies.at(result.at(i).first);
			string rp = receivePay.at(result.at(i).first);
			for (int j = 0; j < numFlows; ++j)
			{
				PyObject* oneFlow = PyList_New(6);

				success = PyList_SetItem(oneFlow, 0, PyInt_FromLong(result.at(i).first));

				success = PyList_SetItem(oneFlow, 1, PyString_FromString(currency.c_str()));

				success = PyList_SetItem(oneFlow, 2, PyString_FromString(rp.c_str()));

				success = PyList_SetItem(oneFlow, 3, PyString_FromString(
					get<0>(result.at(i).second.at(j)).description().c_str()));

				success = PyList_SetItem(oneFlow, 4, PyInt_FromLong(
					get<1>(result.at(i).second.at(j))));

				success = PyList_SetItem(oneFlow, 5, PyFloat_FromDouble(
					get<2>(result.at(i).second.at(j))));

				success = PyList_SetItem(thisMortgage, j, oneFlow);
			}
			success = PyList_SetItem(allMortgages, i, thisMortgage);
		}
		cout << "Fin generacion de flujos de patas fijas." << endl;
		return allMortgages;
	}
	catch (exception& e)
	{
		string msg = "Error en las patas fijas de swaps. " + string(e.what());
		PyErr_SetString(qcDveError, msg.c_str());
		return NULL;
	}
}

PyObject* cashflow_icp_clp_rate_legs(PyObject* self, PyObject*  args)
{
	char* fecha;
	PyObject* holidays;
	PyObject* curveValues;
	PyObject* curveCharacteristics;
	PyObject* legCharacteristics;
	PyObject* icpFixings;

	if (!PyArg_ParseTuple(args,
		"sO!O!O!O!O!",
		&fecha,
		&PyList_Type, &holidays,
		&PyList_Type, &curveValues,
		&PyList_Type, &curveCharacteristics,
		&PyList_Type, &legCharacteristics,
		&PyList_Type, &icpFixings))
	{
		return NULL;
	}

	try
	{
		map<string, vector<QCDate>> mapHolidays;
		QCDvePyBindHelperFunctions::buildHolidays(holidays, mapHolidays);

		map<string, pair<vector<long>, vector<double>>> crvValues;
		QCDvePyBindHelperFunctions::buildCurveValues(curveValues, crvValues);

		map<string, QCDvePyBindHelperFunctions::string4> crvChars;
		QCDvePyBindHelperFunctions::buildCurveCharacteristics(curveCharacteristics, crvChars);

		map<string, shared_ptr<QCZeroCouponCurve>> allCurves;

		//Loopeo sobre los keys de crvValues
		for (const auto &curva : crvValues)
		{
			vector<long> tmpLng{ curva.second.first };
			vector<double> tmpDbl{ curva.second.second };

			string wf = get<1>(crvChars.at(curva.first));
			QCHelperFunctions::lowerCase(wf);

			string yf = get<2>(crvChars.at(curva.first));
			QCHelperFunctions::lowerCase(yf);

			shared_ptr<QCZeroCouponCurve> tmpCrv = QCFactoryFunctions::zrCpnCrvShrdPtr(
				tmpLng,
				tmpDbl,
				get<0>(crvChars.at(curva.first)),
				wf, yf, get<3>(crvChars.at(curva.first)));
			allCurves.insert(pair <string, shared_ptr<QCZeroCouponCurve>>(curva.first, tmpCrv));
		}

		//Metemos los fixings de ICP en esta estructura
		map<QCDate, double> mapIcpFixings;
		QCDvePyBindHelperFunctions::buildFixings(icpFixings, mapIcpFixings);

		//Ahora hay que construir QCInterestRatePayoff para cada operacion
		//Se requiere:
		//	shared_ptr<QCInterestRate>
		//	shared_ptr<QCInterestRateLeg>
		//	QCDate (valueDate)
		//	shared_ptr<QCZeroCouponCurve> (la curva de proyeccion)
		//	shared_ptr<QCZeroCouponCurve> (la curva de descuento)
		//	shared_ptr<QCTimeSeries> (los fixings)

		//La info esta en la CellMatrix legCharacteristics:
		//0:	id_leg					long
		//1:	rec_pay					string
		//2:	start_date				QCDate
		//3:	end_date				QCDate
		//4:	settlement_calendar		string
		//5:	settlement_lag			int
		//6:	stub_period				string
		//7:	periodicity				string
		//8:	end_date_adjustment		string
		//9:	amortization			string
		//10:	interest_rate_index		string
		//11:	rate_value				double
		//12:	spread					double
		//13:	notional_currency		string
		//14:	notional				double
		//15:	wealth_factor			string
		//16:	year_fraction			string
		//17:	projecting_curve		string
		//18:	discount_curve			string
		//19:	fixing_calendar			string

		string strDate{ fecha };
		QCDate allValueDate{ strDate };

		map <long, shared_ptr<QCInterestRatePayoff>> payoffs;
		map <long, string> currencies;
		map <long, string> receivePay;

		for (unsigned long i = 0; i < PyList_Size(legCharacteristics); ++i)
		{
			string wf = PyString_AsString(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 15));
			QCHelperFunctions::lowerCase(wf);
			string yf = PyString_AsString(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 16));
			QCHelperFunctions::lowerCase(yf);
			shared_ptr<QCInterestRate> tmpIntRate = QCFactoryFunctions::intRateSharedPtr(
				PyFloat_AsDouble(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 11)), yf, wf);

			long numOp = PyInt_AsLong(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 0));
			vector<tuple<QCDate, double, double>> amortIfCustom;
			QCInterestRateLeg tmpIntRateLeg = QCFactoryFunctions::buildIcpLeg3(
				allValueDate,
				PyString_AsString(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 1)),	//receive or pay
				QCDate{ string(PyString_AsString(PyList_GetItem(
				PyList_GetItem(legCharacteristics, i), 2))) },	//start date
				QCDate{ string(PyString_AsString(PyList_GetItem(
				PyList_GetItem(legCharacteristics, i), 3))) },	//end date
				mapHolidays.at(PyString_AsString(PyList_GetItem(
				PyList_GetItem(legCharacteristics, i), 4))),		//settlement calendar
				PyInt_AsLong(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 5)),		//settlement lag
				QCHelperFunctions::stringToQCStubPeriod(
				PyString_AsString(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 6))),//stub period
				PyString_AsString(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 7)),//periodicity
				QCHelperFunctions::stringToQCBusDayAdjRule(
				PyString_AsString(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 8))),//end date adjustment
				QCHelperFunctions::stringToQCAmortization(
				PyString_AsString(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 9))),//amortization
				PyFloat_AsDouble(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 14))	//notional
				);
			shared_ptr<QCInterestRatePayoff> tmpIntRatePayoff = shared_ptr<QCInterestRatePayoff>(
				new QCIcpClpPayoff{ tmpIntRate, PyFloat_AsDouble(PyList_GetItem(
				PyList_GetItem(legCharacteristics, i), 12)), 1.0,
				make_shared<QCInterestRateLeg>(tmpIntRateLeg),
				allCurves.at(PyString_AsString(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 17))),
				allCurves.at(PyString_AsString(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 18))),
				allValueDate,
				make_shared<map<QCDate, double>>(mapIcpFixings) });
			payoffs.insert(pair<long, shared_ptr<QCInterestRatePayoff>>(numOp, tmpIntRatePayoff));
			currencies.insert(pair<long, string>(numOp, PyString_AsString(PyList_GetItem(
				PyList_GetItem(legCharacteristics, i), 13))));
			receivePay.insert(pair<long, string>(numOp, PyString_AsString(PyList_GetItem(
				PyList_GetItem(legCharacteristics, i), 1))));

		}

		//Loopear sobre cada payoff y guardar los cashflows
		vector<pair<long, vector<tuple<QCDate, QCCashFlowLabel, double>>>> result;
		long numPayoffs = payoffs.size();
		result.resize(numPayoffs);
		cout << "Numero de patas ICPCLP de swaps: " << numPayoffs << endl;
		cout << "Inicio generacion de flujos patas ICPCLP." << endl;
		long counter = 0;
		for (const auto& payoff : payoffs)
		{
			vector<tuple<QCDate, QCCashFlowLabel, double>> tempFlows;
			payoff.second->payoff();
			long numFlows = payoff.second->payoffSize();
			tempFlows.resize(numFlows);
			for (long i = 0; i < numFlows; ++i)
			{
				tempFlows.at(i) = payoff.second->getCashflowAt(i);
			}
			result.at(counter) = make_pair(payoff.first, tempFlows);
			++counter;
		}

		PyObject* allMortgages = PyList_New(numPayoffs);
		for (long i = 0; i < numPayoffs; ++i)
		{
			int numFlows = result.at(i).second.size();
			PyObject* thisMortgage = PyList_New(numFlows);
			int success;
			string currency = currencies.at(result.at(i).first);
			string rp = receivePay.at(result.at(i).first);
			for (int j = 0; j < numFlows; ++j)
			{
				PyObject* oneFlow = PyList_New(6);

				success = PyList_SetItem(oneFlow, 0, PyInt_FromLong(result.at(i).first));

				success = PyList_SetItem(oneFlow, 1, PyString_FromString(currency.c_str()));

				success = PyList_SetItem(oneFlow, 2, PyString_FromString(rp.c_str()));

				success = PyList_SetItem(oneFlow, 3, PyString_FromString(
					get<0>(result.at(i).second.at(j)).description().c_str()));

				success = PyList_SetItem(oneFlow, 4, PyInt_FromLong(
					get<1>(result.at(i).second.at(j))));

				success = PyList_SetItem(oneFlow, 5, PyFloat_FromDouble(
					get<2>(result.at(i).second.at(j))));

				success = PyList_SetItem(thisMortgage, j, oneFlow);
			}
			success = PyList_SetItem(allMortgages, i, thisMortgage);
		}
		cout << "Fin generacion de flujos patas ICPCLP." << endl;
		return allMortgages;
	}
	catch (exception& e)
	{
		string msg = "Error en las patas icp_clp. " + string(e.what());
		PyErr_SetString(qcDveError, msg.c_str());
		return NULL;
	}
}

PyObject* cashflow_floating_rate_legs(PyObject* self, PyObject*  args)
{
	char* fecha;
	PyObject* holidays;
	PyObject* curveValues;
	PyObject* curveCharacteristics;
	PyObject* legCharacteristics;
	PyObject* fixings;
	PyObject* intRateIndexChars;

	if (!PyArg_ParseTuple(args,
		"sO!O!O!O!O!O!",
		&fecha,
		&PyList_Type, &holidays,
		&PyList_Type, &curveValues,
		&PyList_Type, &curveCharacteristics,
		&PyList_Type, &legCharacteristics,
		&PyList_Type, &fixings,
		&PyList_Type, &intRateIndexChars))
	{
		return NULL;
	}

	try
	{
		map<string, vector<QCDate>> mapHolidays;
		QCDvePyBindHelperFunctions::buildHolidays(holidays, mapHolidays);

		map<string, pair<vector<long>, vector<double>>> crvValues;
		QCDvePyBindHelperFunctions::buildCurveValues(curveValues, crvValues);

		map<string, QCDvePyBindHelperFunctions::string4> crvChars;
		QCDvePyBindHelperFunctions::buildCurveCharacteristics(curveCharacteristics, crvChars);

		map<string, shared_ptr<QCZeroCouponCurve>> allCurves;

		//Loopeo sobre los keys de crvValues
		for (const auto &curva : crvValues)
		{
			vector<long> tmpLng{ curva.second.first };
			vector<double> tmpDbl{ curva.second.second };

			string wf = get<1>(crvChars.at(curva.first));
			QCHelperFunctions::lowerCase(wf);

			string yf = get<2>(crvChars.at(curva.first));
			QCHelperFunctions::lowerCase(yf);

			shared_ptr<QCZeroCouponCurve> tmpCrv = QCFactoryFunctions::zrCpnCrvShrdPtr(
				tmpLng,
				tmpDbl,
				get<0>(crvChars.at(curva.first)),
				wf, yf, get<3>(crvChars.at(curva.first)));
			allCurves.insert(pair <string, shared_ptr<QCZeroCouponCurve>>(curva.first, tmpCrv));
		}

		//Metemos los fixings de los indices de tasa flotante en esta estructura
		map<string, map<QCDate, double>> mapManyFixings;
		QCDvePyBindHelperFunctions::buildManyFixings(fixings, mapManyFixings);

		//Metemos las caracteristicas de los indices en esta estructura
		map<string, pair<string, string>> indexChars; //en el pair viene el tenor y el start date rule
		QCDvePyBindHelperFunctions::buildStringPairStringMap(intRateIndexChars, indexChars);

		//Ahora hay que construir QCInterestRatePayoff para cada operacion
		//Se requiere:
		//	shared_ptr<QCInterestRate>
		//	shared_ptr<QCInterestRateLeg>
		//	QCDate (valueDate)
		//	shared_ptr<QCInterestRateCurve> (la curva de proyeccion)
		//	shared_ptr<QCInterestCurve> (la curva de descuento)
		//	shared_ptr<QCTimeSeries> (los fixings)

		//La info esta en la CellMatrix legCharacteristics:
		//0:	id_leg
		//1:	rec_pay
		//2:	start_date
		//3:	end_date
		//4:	settlement_calendar
		//5:	settlement_lag
		//6:	stub_period
		//7:	periodicity
		//8:	end_date_adjustment
		//9:	amortization
		//10:	interest_rate_index
		//11:	rate_value
		//12:	spread
		//13:	fixing_stub_period
		//14:	fixing_periodicity
		//15:	fixing_calendar
		//16:	fixing_lag
		//17:	notional_currency
		//18:	notional
		//19:	wealth_factor
		//20:	year_fraction
		//21:	projecting_curve
		//22:	discount_curve

		string strDate{ fecha };
		QCDate allValueDate{ strDate };

		map<long, shared_ptr<QCInterestRatePayoff>> payoffs;
		map<long, string> currencies;
		map<long, string> receivePay;
		for (unsigned long i = 0; i < PyList_Size(legCharacteristics); ++i)
		{
			string wf = PyString_AsString(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 19));
			QCHelperFunctions::lowerCase(wf);
			string yf = PyString_AsString(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 20));
			QCHelperFunctions::lowerCase(yf);

			shared_ptr<QCInterestRate> tmpIntRate = QCFactoryFunctions::intRateSharedPtr(
				PyFloat_AsDouble(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 11)), yf, wf);

			long numOp = PyInt_AsLong(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 0));
			QCInterestRateLeg tmpIntRateLeg = QCFactoryFunctions::buildFloatingRateLeg3(
				allValueDate,
				PyString_AsString(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 1)),					 //receive or pay
				QCDate{ string(PyString_AsString(PyList_GetItem(
				PyList_GetItem(legCharacteristics, i), 2))) }, //start date
				QCDate{ string(PyString_AsString(PyList_GetItem(
				PyList_GetItem(legCharacteristics, i), 3))) }, //end date
				mapHolidays.at(PyString_AsString(PyList_GetItem(PyList_GetItem(
				legCharacteristics, i), 4))),  //settlement calendar
				PyInt_AsLong(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 5)),	//settlement lag
				QCHelperFunctions::stringToQCStubPeriod(
				PyString_AsString(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 6))),//stub period
				PyString_AsString(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 7)),//periodicity
				QCHelperFunctions::stringToQCBusDayAdjRule(
				PyString_AsString(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 8))),//end date adjustment
				QCHelperFunctions::stringToQCAmortization(
				PyString_AsString(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 9))),//amortization
				PyInt_AsLong(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 16)),//fixing lag
				QCHelperFunctions::stringToQCStubPeriod(
				PyString_AsString(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 13))),//fixing stub period
				PyString_AsString(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 14)),//fixing periodicity
				mapHolidays.at(PyString_AsString(PyList_GetItem(
				PyList_GetItem(legCharacteristics, i), 15))), //fixing calendar
				indexChars.at(PyString_AsString(PyList_GetItem(PyList_GetItem(
				legCharacteristics, i), 10))),  //interest rate index tenor
				PyFloat_AsDouble(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 18))		 //notional
				);

			shared_ptr<QCInterestRatePayoff> tmpIntRatePayoff = shared_ptr<QCInterestRatePayoff>(
				new QCFloatingRatePayoff{ tmpIntRate,
				PyFloat_AsDouble(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 12)), 1.0,
				make_shared<QCInterestRateLeg>(tmpIntRateLeg),
				allCurves.at(PyString_AsString(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 21))),
				allCurves.at(PyString_AsString(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 22))),
				allValueDate,
				make_shared<map<QCDate, double>>(mapManyFixings.at(PyString_AsString(
				PyList_GetItem(PyList_GetItem(legCharacteristics, i), 10)))) });

			payoffs.insert(pair<long, shared_ptr<QCInterestRatePayoff>>(numOp, tmpIntRatePayoff));

			currencies.insert(pair<long, string>(numOp, PyString_AsString(PyList_GetItem(
				PyList_GetItem(legCharacteristics, i), 17))));

			receivePay.insert(pair<long, string>(numOp, PyString_AsString(PyList_GetItem(
				PyList_GetItem(legCharacteristics, i), 1))));
		}

		//Loopear sobre cada payoff y guardar los cashflows
		vector<pair<long, vector<tuple<QCDate, QCCashFlowLabel, double>>>> result;
		long numPayoffs = payoffs.size();
		result.resize(numPayoffs);
		cout << "Numero de patas flotantes de swaps: " << numPayoffs << endl;
		cout << "Inicio generacion de flujos de patas flotantes." << endl;
		long counter = 0;
		for (const auto& payoff : payoffs)
		{
			vector<tuple<QCDate, QCCashFlowLabel, double>> tempFlows;
			payoff.second->payoff();
			long numFlows = payoff.second->payoffSize();
			tempFlows.resize(numFlows);
			for (long i = 0; i < numFlows; ++i)
			{
				tempFlows.at(i) = payoff.second->getCashflowAt(i);
			}
			result.at(counter) = make_pair(payoff.first, tempFlows);
			++counter;
		}

		PyObject* allMortgages = PyList_New(numPayoffs);
		for (long i = 0; i < numPayoffs; ++i)
		{
			int numFlows = result.at(i).second.size();
			PyObject* thisMortgage = PyList_New(numFlows);
			int success;
			string currency = currencies.at(result.at(i).first);
			string rp = receivePay.at(result.at(i).first);
			for (int j = 0; j < numFlows; ++j)
			{
				PyObject* oneFlow = PyList_New(6);

				success = PyList_SetItem(oneFlow, 0, PyInt_FromLong(result.at(i).first));

				success = PyList_SetItem(oneFlow, 1, PyString_FromString(currency.c_str()));

				success = PyList_SetItem(oneFlow, 2, PyString_FromString(rp.c_str()));

				success = PyList_SetItem(oneFlow, 3, PyString_FromString(
					get<0>(result.at(i).second.at(j)).description().c_str()));

				success = PyList_SetItem(oneFlow, 4, PyInt_FromLong(
					get<1>(result.at(i).second.at(j))));

				success = PyList_SetItem(oneFlow, 5, PyFloat_FromDouble(
					get<2>(result.at(i).second.at(j))));

				success = PyList_SetItem(thisMortgage, j, oneFlow);
			}
			success = PyList_SetItem(allMortgages, i, thisMortgage);
		}
		cout << "Fin generacion de flujos de patas flotantes." << endl;
		return allMortgages;
	}
	catch (exception& e)
	{
		string msg = "Error en las patas flotantes. " + string(e.what());
		PyErr_SetString(qcDveError, msg.c_str());
		return NULL;
	}
}

PyObject* boot_zero_rates_fixed_legs(PyObject* self, PyObject* args)
{
	cout << "Enter function boot_zero_rates_fixed_legs." << endl;
	char* fecha;
	PyObject* holidays;
	PyObject* curveCharacteristics;
	PyObject* zeroRates;
	PyObject* fixedLegs;

	if (!PyArg_ParseTuple(args,
		"sO!O!O!O!",
		&fecha,
		&PyList_Type, &holidays,
		&PyList_Type, &curveCharacteristics,
		&PyList_Type, &zeroRates,
		&PyList_Type, &fixedLegs))
	{
		string msg = "boot_zero_rates_fixed_legs: error en los argumentos.";
		PyErr_SetString(qcDveError, msg.c_str());
		return NULL;
	}
	try
	{
		//Se construye la fecha de proceso
		string strDate{ fecha };
		QCDate valueDate{ strDate };

		//Se construye el vector con las fechas de los feriados
		vector<QCDate> dateVector;
		QCDvePyBindHelperFunctions::buildQCDateVector(holidays, dateVector);
		cout << "\tboot_zero_rates_fixed_legs: finished holidays." << endl;

		//Se construyen las caracteristicas de las curvas a construir
		string curveInterpol{ PyString_AsString(PyList_GetItem(curveCharacteristics, 0)) }; //Interpolacion
		string curveWf{ PyString_AsString(PyList_GetItem(curveCharacteristics, 1)) }; //wealth factor
		QCHelperFunctions::lowerCase(curveWf);
		string curveYf{ PyString_AsString(PyList_GetItem(curveCharacteristics, 2)) }; //year fraction
		QCHelperFunctions::lowerCase(curveYf);
		cout << "\tboot_zero_rates_fixed_legs: finished curve characteristics." << endl;

		//Se construye un vector con los datos de las tasas cero iniciales
		vector<QCDvePyBindHelperFunctions::ZeroRate> zeroRateVector;
		QCDvePyBindHelperFunctions::buildZeroRateVector(zeroRates, valueDate, dateVector, zeroRateVector);
		cout << "\tboot_zero_rates_fixed_legs: finished zero rates." << endl;

		//Se construye un vector con las patas fijas iniciales
		vector<QCDvePyBindHelperFunctions::SwapIndex> swapIndexVector;
		QCDvePyBindHelperFunctions::buildFixedRateIndexVector(fixedLegs, valueDate, dateVector, swapIndexVector);
		cout << "\tboot_zero_rates_fixed_legs: finished fixed rate legs." << endl;

		size_t cuantasTasas = zeroRateVector.size();
		vector<shared_ptr<QCTimeDepositPayoff>> inputRates;
		inputRates.resize(cuantasTasas);
		cout << "\tboot_zero_rates_fixed_legs: inputRates resized" << endl;

		size_t cuantosSwaps = swapIndexVector.size();
		vector<shared_ptr<QCFixedRatePayoff>> inputFixedLegs;
		inputFixedLegs.resize(cuantosSwaps);
		cout << "\tboot_zero_rates_fixed_legs: inputFixedLegs resized" << endl;

		vector<long> tenors;
		tenors.resize(cuantasTasas + cuantosSwaps);
		for (unsigned int i = 0; i < cuantasTasas; ++i)
		{
			tenors.at(i) = valueDate.dayDiff(get<1>(zeroRateVector.at(i)));
		}

		for (unsigned int i = cuantasTasas; i < cuantasTasas + cuantosSwaps; ++i)
		{
			tenors.at(i) = valueDate.dayDiff(get<2>(swapIndexVector.at(i - cuantasTasas)));
		}
		cout << "\tboot_zero_rates_fixed_legs: curve tenors ok" << endl;

		vector<double> rates;
		rates.resize(cuantasTasas + cuantosSwaps);
		for (unsigned int i = 0; i < cuantasTasas + cuantosSwaps; ++i)
		{
			rates.at(i) = 0.0;
		}
		cout << "\tboot_zero_rates_fixed_legs: curve rates ok" << endl;

		QCZrCpnCrvShrdPtr curve = QCFactoryFunctions::zrCpnCrvShrdPtr(tenors, rates, curveInterpol,
			curveYf, curveWf);
		cout << "\tboot_zero_rates_fixed_legs: curve initialized" << endl;

		for (unsigned int i = 0; i < cuantasTasas; ++i)
		{
			QCInterestRateLeg tdLeg = QCFactoryFunctions::buildTimeDepositLeg(
				"R",
				get<0>(zeroRateVector.at(i)),
				get<1>(zeroRateVector.at(i)),
				1.0);
			cout << "\tboot_zero_rates_fixed_legs: time deposit leg " << i << " initialized" << endl;
			QCIntrstRtShrdPtr intRate = QCFactoryFunctions::intRateSharedPtr(
				get<2>(zeroRateVector.at(i)),
				get<3>(zeroRateVector.at(i)),
				get<4>(zeroRateVector.at(i)));
			cout << "\tboot_zero_rates_fixed_legs: interest rate " << i << " initialized" << endl;
			QCTimeDepositPayoff tdPayoff{ intRate, make_shared<QCInterestRateLeg>(tdLeg), valueDate, curve };
			inputRates.at(i) = make_shared<QCTimeDepositPayoff>(tdPayoff);
			cout << "\tboot_zero_rates_fixed_legs: time deposit payoff " << i << " initialized" << endl;

		}

		vector<tuple<QCDate, double, double>> amortIfCustom;
		for (unsigned int i = 0; i < cuantosSwaps; ++i)
		{
			//Se construye el interest rate
			shared_ptr<QCInterestRate> tmpIntRate = QCFactoryFunctions::intRateSharedPtr(
				get<8>(swapIndexVector.at(i)),
				get<10>(swapIndexVector.at(i)),
				get<11>(swapIndexVector.at(i)));
			cout << "\tboot_zero_rates_fixed_legs: swap interest rate " << i << " initialized" << endl;

			//buildFixedRateLeg
			//Este pedazo de código debe quedar en una HelperFunction de QC_DVE_XLL
			QCInterestRateLeg tmpFixedLeg = QCFactoryFunctions::buildFixedRateLeg2(
				get<0>(swapIndexVector.at(i)),			//receive or pay
				get<1>(swapIndexVector.at(i)),			//start date
				get<2>(swapIndexVector.at(i)),			//end date
				dateVector,								//settlement calendar
				get<3>(swapIndexVector.at(i)),			//settlement lag
				get<4>(swapIndexVector.at(i)),			//stub period
				get<5>(swapIndexVector.at(i)),			//periodicity
				get<6>(swapIndexVector.at(i)),			//end date adjustment
				get<7>(swapIndexVector.at(i)),			//amortization
				amortIfCustom,							//amortization and notional by date
				get<9>(swapIndexVector.at(i)));			//notional
			cout << "\tboot_zero_rates_fixed_legs: fixed rate interest leg " << i << " initialized" << endl;

			//buildFixedRatePayoff
			shared_ptr<QCFixedRatePayoff> tmpIntRatePayoff = shared_ptr<QCFixedRatePayoff>(
				new QCFixedRatePayoff{ tmpIntRate, make_shared<QCInterestRateLeg>(tmpFixedLeg), curve, valueDate,
				nullptr });
			cout << "\tboot_zero_rates_fixed_legs: fixed rate interest payoff " << i << " initialized" << endl;

			//Se agrega el payoff al vector de fixed legs
			inputFixedLegs.at(i) = tmpIntRatePayoff;
		}

		//Se da de alta el objeto bootstrapeador
		QCZeroCurveBootstrappingFromRatesAndFixedLegs boot{ valueDate, inputRates, inputFixedLegs, curve };
		cout << "\tboot_zero_rates_fixed_legs: bootstrapping object initialized" << endl;

		boot.generateCurveAndDerivatives();
		cout << "\tboot_zero_rates_fixed_legs: curve bootstrapped" << endl;

		vector<tuple<long, double, vector<double>>> result;
		result.resize(cuantasTasas + cuantosSwaps);
		for (size_t i = 0; i < cuantasTasas + cuantosSwaps; ++i)
		{
			get<2>(result.at(i)).resize(cuantasTasas + cuantosSwaps);
			pair<long, double> temp = curve->getValuesAt(i);
			get<0>(result.at(i)) = temp.first;
			get<1>(result.at(i)) = temp.second;
			for (size_t j = 0; j < cuantasTasas + cuantosSwaps; ++j)
			{
				get<2>(result.at(i)).at(j) = boot.getDerivativeAt(i, j); // 0.0
			}
		}
		cout << "\tboot_zero_rates_fixed_legs: results obtained" << endl;

		PyObject* curveAndDeltas = PyList_New(cuantasTasas + cuantosSwaps);
		for (size_t i = 0; i < cuantasTasas + cuantosSwaps; ++i)
		{
			size_t columnas = 2 + cuantasTasas + cuantosSwaps;
			PyObject* temp = PyList_New(columnas);
			int success;
			success = PyList_SetItem(temp, 0, PyInt_FromLong(get<0>(result.at(i))));
			success = PyList_SetItem(temp, 1, PyFloat_FromDouble(get<1>(result.at(i))));
			for (unsigned int j = 0; j < cuantasTasas + cuantosSwaps; ++j)
			{
				success = PyList_SetItem(temp, j + 2, PyFloat_FromDouble(get<2>(result.at(i)).at(j)));
			}
			success = PyList_SetItem(curveAndDeltas, i, temp);
		}
		cout << "\tboot_zero_rates_fixed_legs: output prepared" << endl;

		return curveAndDeltas;

	}
	catch (exception& e)
	{
		string msg = "Error en el bootstrapping Rates&Fixed_Legs. " + string(e.what());
		PyErr_SetString(qcDveError, msg.c_str());
		return NULL;
	}

}

PyObject* boot_zero_rates_zero_coupon_spreads(PyObject* self, PyObject* args)
{
	cout << "Enter function boot_zero_rates_coupon_spreads." << endl;
	char* fecha;
	PyObject* curveCharacteristics;
	PyObject* auxCurveValues;
	PyObject* auxCurveChars;
	PyObject* zeroRates;
	PyObject* zeroRateSpreads;
	PyObject* holidays;

	if (!PyArg_ParseTuple(args,
		"sO!O!O!O!O!O!",
		&fecha,
		&PyList_Type, &curveCharacteristics,
		&PyList_Type, &auxCurveValues,
		&PyList_Type, &auxCurveChars,
		&PyList_Type, &zeroRates,
		&PyList_Type, &zeroRateSpreads,
		&PyList_Type, &holidays))
	{
		string msg = "boot_zero_rates_zero_coupon_spreads: error en los argumentos.";
		PyErr_SetString(qcDveError, msg.c_str());
		return NULL;
	}
	try
	{
		//Se construye la fecha de proceso
		string strDate{ fecha };
		QCDate valueDate{ strDate };
		cout << "\tboot_zero_rates_zero_coupon_spreads: finished process date." << endl;

		//Se construye el vector con las fechas de los feriados
		vector<QCDate> dateVector;
		QCDvePyBindHelperFunctions::buildQCDateVector(holidays, dateVector);
		cout << "\tboot_zero_rates_zero_coupon_spreads: finished holidays." << endl;

		//Se construyen las caracteristicas de las curvas a construir
		string curveInterpol{ PyString_AsString(PyList_GetItem(curveCharacteristics, 0)) }; //Interpolacion
		string curveWf{ PyString_AsString(PyList_GetItem(curveCharacteristics, 1)) }; //wealth factor
		QCHelperFunctions::lowerCase(curveWf);
		string curveYf{ PyString_AsString(PyList_GetItem(curveCharacteristics, 2)) }; //year fraction
		QCHelperFunctions::lowerCase(curveYf);
		cout << "\tboot_zero_rates_zero_coupon_spreads: finished curve characteristics." << endl;

		//Se dan de alta las caracteristicas de la curva auxiliar
		string auxCurveInterpol{ PyString_AsString(PyList_GetItem(auxCurveChars, 0)) }; //Interpolacion
		string auxCurveWf{ PyString_AsString(PyList_GetItem(auxCurveChars, 1)) }; //wealth factor
		QCHelperFunctions::lowerCase(auxCurveWf);
		string auxCurveYf{ PyString_AsString(PyList_GetItem(auxCurveChars, 2)) }; //year fraction
		QCHelperFunctions::lowerCase(auxCurveYf);
		cout << "\tboot_zero_rates_zero_coupon_spreads: finished aux curve characteristics." << endl;

		//Se construye el objeto curva auxiliar
		vector<long> auxTenors;
		vector<double> auxRates;
		QCDvePyBindHelperFunctions::buildAuxTenorsAndRates(auxCurveValues, auxTenors, auxRates);
		cout << "\tboot_zero_rates_zero_coupon_spreads: aux curve tenors and rates initialized" << endl;

		QCZrCpnCrvShrdPtr auxCrvPtr = QCFactoryFunctions::zrCpnCrvShrdPtr(auxTenors, auxRates, auxCurveInterpol,
			auxCurveYf, auxCurveWf);
		cout << "\tboot_zero_rates_zero_coupon_spreads: aux curve initialized" << endl;

		//Se construye un vector con los datos de las tasas cero iniciales
		vector<QCDvePyBindHelperFunctions::ZeroRate> zeroRateVector;
		QCDvePyBindHelperFunctions::buildZeroRateVector(zeroRates, valueDate, dateVector, zeroRateVector);
		cout << "\tboot_zero_rates_zero_coupon_spreads: finished zero rates." << endl;

		//Se construye un vector con los datos de los spreads iniciales
		vector<QCDvePyBindHelperFunctions::ZeroRate> zeroRateSpreadsVector;
		QCDvePyBindHelperFunctions::buildZeroRateVector(zeroRateSpreads, valueDate,
			dateVector, zeroRateSpreadsVector);
		cout << "\tboot_zero_rates_zero_coupon_spreads: finished zero rate spreads." << endl;

		//En este loop se le suma la base a cada spread
		for (auto& spread : zeroRateSpreadsVector)
		{
			//Se obtiene el plazo al cual interpolar la tasa
			long p = get<QCDvePyBindHelperFunctions::qcZeroRateStartDate>(spread).
				dayDiff(get<QCDvePyBindHelperFunctions::qcZeroRateEndDate>(spread));

			//Se calcula el wf interpolado
			double wf = 1 / auxCrvPtr->getDiscountFactorAt(p);
			
			//Se obtienen la yf y wf del spread
			string yf{ get<QCDvePyBindHelperFunctions::qcZeroRateYf>(spread) };
			string wfStr{ get<QCDvePyBindHelperFunctions::qcZeroRateWf>(spread) };

			//Se obtiene la tasa en la misma yf, wf que los spreads
			QCIntrstRtShrdPtr rate = QCFactoryFunctions::intRateSharedPtr(0.0, yf, wfStr);
			double baseRate = rate->getRateFromWf(wf, p);

			//Se le suma la base al spread
			get<QCDvePyBindHelperFunctions::qcZeroRateValue>(spread) += baseRate;

			zeroRateVector.push_back(spread);
		}

		size_t cuantasTasas = zeroRateVector.size();
		vector<shared_ptr<QCTimeDepositPayoff>> inputRates;
		inputRates.resize(cuantasTasas);
		cout << "\tboot_zero_rates_zero_coupon_spreads: inputRates resized" << endl;

		vector<long> tenors;
		tenors.resize(cuantasTasas);
		for (unsigned int i = 0; i < cuantasTasas; ++i)
		{
			tenors.at(i) = valueDate.dayDiff(get<1>(zeroRateVector.at(i)));
		}
		cout << "\tboot_zero_rates_zero_coupon_spreads: curve tenors ok" << endl;

		vector<double> rates;
		rates.resize(cuantasTasas);
		for (unsigned int i = 0; i < cuantasTasas; ++i)
		{
			rates.at(i) = 0.0;
		}
		cout << "\tboot_zero_rates_zero_coupon_spreads: curve rates ok" << endl;

		QCZrCpnCrvShrdPtr curve = QCFactoryFunctions::zrCpnCrvShrdPtr(tenors, rates, curveInterpol,
			curveYf, curveWf);
		cout << "\boot_zero_rates_zero_coupon_spreads: curve initialized" << endl;

		for (unsigned int i = 0; i < cuantasTasas; ++i)
		{
			QCInterestRateLeg tdLeg = QCFactoryFunctions::buildTimeDepositLeg(
				"R",
				get<0>(zeroRateVector.at(i)),
				get<1>(zeroRateVector.at(i)),
				1.0);
			cout << "\tboot_zero_rates_zero_coupon_spreads: time deposit leg " << i << " initialized" << endl;
			QCIntrstRtShrdPtr intRate = QCFactoryFunctions::intRateSharedPtr(
				get<2>(zeroRateVector.at(i)),
				get<3>(zeroRateVector.at(i)),
				get<4>(zeroRateVector.at(i)));
			cout << "\tboot_zero_rates_zero_coupon_spreads: interest rate " << i << " initialized" << endl;
			QCTimeDepositPayoff tdPayoff{ intRate, make_shared<QCInterestRateLeg>(tdLeg), valueDate, curve };
			inputRates.at(i) = make_shared<QCTimeDepositPayoff>(tdPayoff);
			cout << "\tboot_zero_rates_zero_coupon_spreads: time deposit payoff " << i << " initialized" << endl;
		}

		//Se da de alta el objeto bootstrapeador
		QCZeroCurveBootstrappingFromRates boot{ valueDate, inputRates, curve };
		cout << "\boot_zero_rates_zero_coupon_spreads: bootstrapping object initialized" << endl;

		boot.generateCurveAndDerivatives();
		cout << "\boot_zero_rates_zero_coupon_spreads: curve bootstrapped" << endl;

		vector<tuple<long, double, vector<double>>> result;
		result.resize(cuantasTasas);
		for (size_t i = 0; i < cuantasTasas; ++i)
		{
			get<2>(result.at(i)).resize(cuantasTasas);
			pair<long, double> temp = curve->getValuesAt(i);
			get<0>(result.at(i)) = temp.first;
			get<1>(result.at(i)) = temp.second;
			for (size_t j = 0; j < cuantasTasas; ++j)
			{
				get<2>(result.at(i)).at(j) = boot.getDerivativeAt(i, j);				
			}
		}
		cout << "\boot_zero_rates_zero_coupon_spreads: results obtained" << endl;

		PyObject* curveAndDeltas = PyList_New(cuantasTasas);
		for (size_t i = 0; i < cuantasTasas; ++i)
		{
			size_t columnas = 2 + cuantasTasas;
			PyObject* temp = PyList_New(columnas);
			int success;
			success = PyList_SetItem(temp, 0, PyInt_FromLong(get<0>(result.at(i))));
			success = PyList_SetItem(temp, 1, PyFloat_FromDouble(get<1>(result.at(i))));
			for (unsigned int j = 0; j < cuantasTasas; ++j)
			{
				success = PyList_SetItem(temp, j + 2, PyFloat_FromDouble(get<2>(result.at(i)).at(j)));
			}
			success = PyList_SetItem(curveAndDeltas, i, temp);
		}
		cout << "\boot_zero_rates_zero_coupon_spreads: output prepared" << endl;

		return curveAndDeltas;

	}
	catch (exception& e)
	{
		string msg = "Error en el bootstrapping Rates&Zero_Coupon_Spreads. " + string(e.what());
		PyErr_SetString(qcDveError, msg.c_str());
		return NULL;
	}

}

PyObject* boot_zero_rates_fwds_fixed_legs(PyObject* self, PyObject* args)
{
	cout << "Enter function boot_zero_rates_fwds_fixed_legs." << endl;
	char* fecha;
	double fx;
	PyObject* holidays;
	PyObject* curveCharacteristics;
	PyObject* zeroRates;
	PyObject* fxForwards;
	PyObject* fixedLegs;
	PyObject* auxCurveValues;
	PyObject* auxCurveChars;

	if (!PyArg_ParseTuple(args,
		"sdO!O!O!O!O!O!O!",
		&fecha,
		&fx,
		&PyList_Type, &holidays,
		&PyList_Type, &curveCharacteristics,
		&PyList_Type, &zeroRates,
		&PyList_Type, &fxForwards,
		&PyList_Type, &fixedLegs,
		&PyList_Type, &auxCurveValues,
		&PyList_Type, &auxCurveChars))
	{
		string msg = "boot_zero_rates_fwds_fixed_legs: error en los argumentos.";
		PyErr_SetString(qcDveError, msg.c_str());
		return NULL;
	}
	try
	{
		//Se construye la fecha de proceso
		string strDate{ fecha };
		QCDate valueDate{ strDate };

		//Se construye el vector con las fechas de los feriados
		vector<QCDate> dateVector;
		QCDvePyBindHelperFunctions::buildQCDateVector(holidays, dateVector);
		cout << "\tboot_zero_rates_fwds_fixed_legs: finished holidays." << endl;

		//Se dan de alta las caracteristicas de la curva a construir
		string curveInterpol{ PyString_AsString(PyList_GetItem(curveCharacteristics, 0)) }; //Interpolacion
		string curveWf{ PyString_AsString(PyList_GetItem(curveCharacteristics, 1)) }; //wealth factor
		QCHelperFunctions::lowerCase(curveWf);
		string curveYf{ PyString_AsString(PyList_GetItem(curveCharacteristics, 2)) }; //year fraction
		QCHelperFunctions::lowerCase(curveYf);
		cout << "\tboot_zero_rates_fwds_fixed_legs: finished curve characteristics." << endl;

		//Se dan de alta las caracteristicas de la curva auxiliar
		string auxCurveInterpol{ PyString_AsString(PyList_GetItem(auxCurveChars, 0)) }; //Interpolacion
		string auxCurveWf{ PyString_AsString(PyList_GetItem(auxCurveChars, 1)) }; //wealth factor
		QCHelperFunctions::lowerCase(auxCurveWf);
		string auxCurveYf{ PyString_AsString(PyList_GetItem(auxCurveChars, 2)) }; //year fraction
		QCHelperFunctions::lowerCase(auxCurveYf);
		cout << "\tboot_zero_rates_fwds_fixed_legs: finished aux curve characteristics." << endl;

		//Se construye el objeto curva auxiliar
		vector<long> auxTenors;
		vector<double> auxRates;
		QCDvePyBindHelperFunctions::buildAuxTenorsAndRates(auxCurveValues, auxTenors, auxRates);

		QCZrCpnCrvShrdPtr auxCrvPtr = QCFactoryFunctions::zrCpnCrvShrdPtr(auxTenors, auxRates, auxCurveInterpol,
			auxCurveYf, auxCurveWf);
		cout << "\tboot_zero_rates_fwds_fixed_legs: aux curve initialized" << endl;

		//Se construye un vector con los datos de las tasas cero iniciales
		vector<QCDvePyBindHelperFunctions::ZeroRate> zeroRateVector;
		QCDvePyBindHelperFunctions::buildZeroRateVector(zeroRates, valueDate, dateVector, zeroRateVector);
		cout << "\tboot_zero_rates_fwds_fixed_legs: finished zero rates." << endl;

		//Se construye un vector con los datos de los fwds iniciales
		vector<QCDvePyBindHelperFunctions::FwdIndex> fwdIndexVector;
		QCDvePyBindHelperFunctions::buildFwdVector(fxForwards, fwdIndexVector);
		cout << "\tboot_zero_rates_fwds_fixed_legs: finished fx fwds." << endl;

		//Se construye un vector con las patas fijas iniciales
		vector<QCDvePyBindHelperFunctions::SwapIndex> swapIndexVector;
		QCDvePyBindHelperFunctions::buildFixedRateIndexVector(fixedLegs, valueDate, dateVector, swapIndexVector);
		cout << "\tboot_zero_rates_fwds_fixed_legs: finished fixed rate legs." << endl;

		size_t cuantasTasas = zeroRateVector.size();
		vector<shared_ptr<QCTimeDepositPayoff>> inputRates;
		inputRates.resize(cuantasTasas);
		cout << "\tboot_zero_rates_fwds_fixed_legs: inputRates resized" << endl;

		size_t cuantosFwds = fwdIndexVector.size();
		vector<shared_ptr<QCFXForward>> inputForwards;
		inputForwards.resize(cuantosFwds);
		cout << "\tboot_zero_rates_fwds_fixed_legs: inputForwards resized" << endl;

		size_t cuantosSwaps = swapIndexVector.size();
		vector<shared_ptr<QCFixedRatePayoff>> inputFixedLegs;
		inputFixedLegs.resize(cuantosSwaps);
		cout << "\tboot_zero_rates_fwds_fixed_legs: inputFixedLegs resized" << endl;

		vector<long> tenors;
		tenors.resize(cuantasTasas + cuantosFwds + cuantosSwaps);
		for (size_t i = 0; i < cuantasTasas; ++i)
		{
			tenors.at(i) = valueDate.dayDiff(get<1>(zeroRateVector.at(i)));
		}

		for (size_t i = cuantasTasas; i < cuantasTasas + cuantosFwds; ++i)
		{
			tenors.at(i) = valueDate.dayDiff(get<2>(fwdIndexVector.at(i - cuantasTasas)));
		}

		for (size_t i = cuantasTasas + cuantosFwds; i < cuantasTasas + cuantosFwds + cuantosSwaps; ++i)
		{
			tenors.at(i) = valueDate.dayDiff(get<2>(swapIndexVector.at(i - cuantasTasas - cuantosFwds)));
		}
		cout << "\tboot_zero_rates_fwds_fixed_legs: curve tenors ok" << endl;

		vector<double> rates;
		rates.resize(cuantasTasas + cuantosFwds + cuantosSwaps);
		for (size_t i = 0; i < cuantasTasas + cuantosFwds + cuantosSwaps; ++i)
		{
			rates.at(i) = 0.0;
		}
		cout << "\tboot_zero_rates_fwds_fixed_legs: curve rates ok" << endl;

		QCZrCpnCrvShrdPtr curve = QCFactoryFunctions::zrCpnCrvShrdPtr(tenors, rates, curveInterpol,
			curveYf, curveWf);
		cout << "\tboot_zero_rates_fwds_fixed_legs: curve initialized" << endl;

		for (unsigned int i = 0; i < cuantasTasas; ++i)
		{
			QCInterestRateLeg tdLeg = QCFactoryFunctions::buildTimeDepositLeg(
				"R",
				get<0>(zeroRateVector.at(i)),
				get<1>(zeroRateVector.at(i)),
				1.0);
			cout << "\tboot_zero_rates_fixed_legs: time deposit leg " << i << " initialized" << endl;
			QCIntrstRtShrdPtr intRate = QCFactoryFunctions::intRateSharedPtr(
				get<2>(zeroRateVector.at(i)),
				get<3>(zeroRateVector.at(i)),
				get<4>(zeroRateVector.at(i)));
			cout << "\tboot_zero_rates_fixed_legs: interest rate " << i << " initialized" << endl;
			QCTimeDepositPayoff tdPayoff{ intRate, make_shared<QCInterestRateLeg>(tdLeg), valueDate, curve };
			inputRates.at(i) = make_shared<QCTimeDepositPayoff>(tdPayoff);
			cout << "\tboot_zero_rates_fixed_legs: time deposit payoff " << i << " initialized" << endl;

		}

		map<QCCurrencyConverter::QCFxRateEnum, double> fxRate;
		vector<shared_ptr<QCDiscountBondPayoff>> legs;
		shared_ptr<map<QCDate, double>> fixings;
		legs.resize(2);
		fxRate.insert(pair<QCCurrencyConverter::QCFxRateEnum, double>(get<6>(fwdIndexVector.at(0)), fx));
		fxRate.insert(pair<QCCurrencyConverter::QCFxRateEnum, double>(get<7>(fwdIndexVector.at(0)), 1.0));
		for (unsigned int i = 0; i < cuantosFwds; ++i)
		{
			//strong currency
			QCInterestRateLeg dbLeg0 = QCFactoryFunctions::buildDiscountBondLeg(
				"R", valueDate, get<2>(fwdIndexVector.at(i)),
				0, 0, dateVector, dateVector, get<0>(fwdIndexVector.at(i)));

			//weak currency
			QCInterestRateLeg dbLeg1 = QCFactoryFunctions::buildDiscountBondLeg(
				"P", valueDate, get<2>(fwdIndexVector.at(i)),
				0, 0, dateVector, dateVector, get<1>(fwdIndexVector.at(i)));

			//En este bloque estamos asumiendo que la curva a construir es la de la
			//moneda fuerte del par de los fwds. Hay que insertar un if para distinguir
			//si es asi o corresponde a la moneda debil.
			auto dbPayoff0 = make_shared<QCDiscountBondPayoff>(
				make_shared<QCInterestRateLeg>(dbLeg0),
				curve, valueDate, fixings,
				get<4>(fwdIndexVector.at(i)), get<6>(fwdIndexVector.at(i)));
			legs.at(0) = dbPayoff0;

			auto dbPayoff1 = make_shared<QCDiscountBondPayoff>(
				make_shared<QCInterestRateLeg>(dbLeg1),
				auxCrvPtr, valueDate, fixings,
				get<5>(fwdIndexVector.at(i)), get<7>(fwdIndexVector.at(i)));
			legs.at(1) = dbPayoff1;

			auto fxFwd = make_shared<QCFXForward>(legs, get<3>(fwdIndexVector.at(i)),
				make_shared<map<QCCurrencyConverter::QCFxRateEnum, double>>(fxRate));
			cout << "\tboot_zero_rates_fwds_fixed_legs: Fx Forward " << i << " initialized" << endl;

			inputForwards.at(i) = fxFwd;
		}


		vector<tuple<QCDate, double, double>> amortIfCustom;
		for (unsigned int i = 0; i < cuantosSwaps; ++i)
		{
			//Se construye el interest rate
			shared_ptr<QCInterestRate> tmpIntRate = QCFactoryFunctions::intRateSharedPtr(
				get<8>(swapIndexVector.at(i)),
				get<10>(swapIndexVector.at(i)),
				get<11>(swapIndexVector.at(i)));
			cout << "\tboot_zero_rates_fixed_legs: swap interest rate " << i << " initialized" << endl;

			//buildFixedRateLeg
			//Este pedazo de código debe quedar en una HelperFunction de QC_DVE_XLL
			QCInterestRateLeg tmpFixedLeg = QCFactoryFunctions::buildFixedRateLeg2(
				get<0>(swapIndexVector.at(i)),			//receive or pay
				get<1>(swapIndexVector.at(i)),			//start date
				get<2>(swapIndexVector.at(i)),			//end date
				dateVector,								//settlement calendar
				get<3>(swapIndexVector.at(i)),			//settlement lag
				get<4>(swapIndexVector.at(i)),			//stub period
				get<5>(swapIndexVector.at(i)),			//periodicity
				get<6>(swapIndexVector.at(i)),			//end date adjustment
				get<7>(swapIndexVector.at(i)),			//amortization
				amortIfCustom,							//amortization and notional by date
				get<9>(swapIndexVector.at(i)));			//notional
			cout << "\tboot_zero_rates_fixed_legs: fixed rate interest leg " << i << " initialized" << endl;

			//buildFixedRatePayoff
			shared_ptr<QCFixedRatePayoff> tmpIntRatePayoff = shared_ptr<QCFixedRatePayoff>(
				new QCFixedRatePayoff{ tmpIntRate, make_shared<QCInterestRateLeg>(tmpFixedLeg), curve, valueDate,
				nullptr });
			cout << "\tboot_zero_rates_fixed_legs: fixed rate interest payoff " << i << " initialized" << endl;

			//Se agrega el payoff al vector de fixed legs
			inputFixedLegs.at(i) = tmpIntRatePayoff;
		}

		//Se da de alta el objeto bootstrapeador
		QCZeroCurveBootstrappingFromRatesFwdsAndFixedLegs boot{ valueDate, inputRates,
			inputForwards, 0, inputFixedLegs, curve };
		cout << "\tboot_zero_rates_fwds_fixed_legs: bootstrapping object initialized" << endl;

		boot.generateCurveAndDerivatives();
		cout << "\tboot_zero_rates_fwds_fixed_legs: curve bootstrapped" << endl;

		vector<tuple<long, double, vector<double>>> result; //plazo, tasa, derivadas
		size_t tamagno = curve->getLength();
		result.resize(tamagno);
		for (size_t i = 0; i < tamagno; ++i)
		{
			get<2>(result.at(i)).resize(tamagno);
			pair<long, double> temp = curve->getValuesAt(i);
			get<0>(result.at(i)) = temp.first;
			get<1>(result.at(i)) = temp.second;
			for (size_t j = 0; j < tamagno; ++j)
			{
				get<2>(result.at(i)).at(j) = boot.getDerivativeAt(i, j);				
			}
		}
		cout << "\tboot_zero_rates_fwds_fixed_legs: results obtained" << endl;

		PyObject* curveAndDeltas = PyList_New(tamagno);
		for (size_t i = 0; i < tamagno; ++i)
		{
			size_t columnas = 2 + tamagno;
			PyObject* temp = PyList_New(columnas);
			int success;
			success = PyList_SetItem(temp, 0, PyInt_FromLong(get<0>(result.at(i))));
			success = PyList_SetItem(temp, 1, PyFloat_FromDouble(get<1>(result.at(i))));
			for (unsigned int j = 0; j < tamagno; ++j)
			{
				success = PyList_SetItem(temp, j + 2, PyFloat_FromDouble(get<2>(result.at(i)).at(j)));
			}
			success = PyList_SetItem(curveAndDeltas, i, temp);
		}
		cout << "\tboot_zero_rates_fwds_fixed_legs: output prepared" << endl;

		return curveAndDeltas;

	}
	catch (exception& e)
	{
		string msg = "Error en el bootstrapping Rates&Fwds&Fixed_Legs. " + string(e.what());
		PyErr_SetString(qcDveError, msg.c_str());
		return NULL;
	}

}

PyObject* boot_zero_rates_fwds_floating_legs(PyObject* self, PyObject* args)
{
	cout << "Enter function boot_zero_rates_fwds_floating_legs." << endl;
	char* fecha;
	double fx;
	double fixing;
	PyObject* holidays;
	PyObject* curveCharacteristics;
	PyObject* zeroRates;
	PyObject* fxForwards;
	PyObject* floatingLegs;
	PyObject* auxCurveValues;
	PyObject* auxCurveChars;

	if (!PyArg_ParseTuple(args,
		"sddO!O!O!O!O!O!O!",
		&fecha,
		&fx,
		&fixing,
		&PyList_Type, &holidays,
		&PyList_Type, &curveCharacteristics,
		&PyList_Type, &zeroRates,
		&PyList_Type, &fxForwards,
		&PyList_Type, &floatingLegs,
		&PyList_Type, &auxCurveValues,
		&PyList_Type, &auxCurveChars))
	{
		string msg = "boot_zero_rates_fwds_floating_legs: error en los argumentos.";
		PyErr_SetString(qcDveError, msg.c_str());
		return NULL;
	}
	try
	{
		//Se construye la fecha de proceso
		string strDate{ fecha };
		QCDate valueDate{ strDate };

		//Se construye el mapa con los feriados de cada calendario
		map<string, vector<QCDate>> mapHolidays;
		QCDvePyBindHelperFunctions::buildHolidays(holidays, mapHolidays);
		cout << "\tboot_zero_rates_fwds_floating_legs: finished map holidays" << endl;

		//Se dan de alta las caracteristicas de la curva a construir
		string curveInterpol{ PyString_AsString(PyList_GetItem(curveCharacteristics, 0)) }; //Interpolacion
		string curveWf{ PyString_AsString(PyList_GetItem(curveCharacteristics, 1)) }; //wealth factor
		QCHelperFunctions::lowerCase(curveWf);
		string curveYf{ PyString_AsString(PyList_GetItem(curveCharacteristics, 2)) }; //year fraction
		QCHelperFunctions::lowerCase(curveYf);
		cout << "\tboot_zero_rates_fwds_floating_legs: finished curve characteristics." << endl;

		//Se dan de alta las caracteristicas de las curvas auxiliares
		size_t numAuxCurves = PyList_Size(auxCurveChars);
		map<string, QCZrCpnCrvShrdPtr> auxCurvesMap;
		for (size_t i = 0; i < numAuxCurves; ++i)
		{
			string auxCurveInterpol{ PyString_AsString(PyTuple_GetItem(
				PyList_GetItem(auxCurveChars, i), 2)) }; //Interpolacion
			string auxCurveWf{ PyString_AsString(PyTuple_GetItem(
				PyList_GetItem(auxCurveChars, i), 3)) }; //wealth factor
			QCHelperFunctions::lowerCase(auxCurveWf);
			string auxCurveYf{ PyString_AsString(PyTuple_GetItem(
				PyList_GetItem(auxCurveChars, i), 4)) }; //year fraction
			QCHelperFunctions::lowerCase(auxCurveYf);

			//Se construye el objeto curva auxiliar
			vector<long> auxTenors;
			vector<double> auxRates;
			string queCurva = PyString_AsString(PyTuple_GetItem(PyList_GetItem(auxCurveChars, i), 0));
			QCDvePyBindHelperFunctions::buildAuxTenorsAndRates(auxCurveValues, auxTenors, auxRates, queCurva);
			QCZrCpnCrvShrdPtr auxCrvPtr = QCFactoryFunctions::zrCpnCrvShrdPtr(auxTenors, auxRates,
				auxCurveInterpol, auxCurveYf, auxCurveWf);
			string whichInput{ PyString_AsString(PyTuple_GetItem(PyList_GetItem(auxCurveChars, i), 1)) };
			auxCurvesMap.insert(make_pair(whichInput, auxCrvPtr));
		}
		cout << "\tboot_zero_rates_fwds_floating_legs: aux curves initialized" << endl;

		//Se construye un vector con los datos de las tasas cero iniciales
		vector<QCDvePyBindHelperFunctions::ZeroRate> zeroRateVector;
		string zeroRateCalendar{ PyString_AsString(PyTuple_GetItem(PyList_GetItem(zeroRates, 0), 3)) };
		QCDvePyBindHelperFunctions::buildZeroRateVector(zeroRates, valueDate,
			mapHolidays.at(zeroRateCalendar), zeroRateVector);
		cout << "\tboot_zero_rates_fwds_floating_legs: finished zero rates." << endl;

		//Se construye un vector con los datos de los fwds iniciales
		vector<QCDvePyBindHelperFunctions::FwdIndex> fwdIndexVector;
		QCDvePyBindHelperFunctions::buildFwdVector(fxForwards, fwdIndexVector, fx);
		cout << "\tboot_zero_rates_fwds_floating_legs: finished fx fwds." << endl;

		//Se construye un vector con las patas flotantes iniciales
		vector<QCDvePyBindHelperFunctions::FloatIndex> floatIndexVector;
		string floatLegSettlementCalendar{ PyString_AsString(PyTuple_GetItem(PyList_GetItem(floatingLegs, 0), 6)) };
		QCDvePyBindHelperFunctions::buildFloatingRateIndexVector(floatingLegs, valueDate,
			mapHolidays.at(floatLegSettlementCalendar), floatIndexVector);
		cout << "\tboot_zero_rates_fwds_floating_legs: finished floating rate legs." << endl;

		size_t cuantasTasas = zeroRateVector.size();
		vector<shared_ptr<QCTimeDepositPayoff>> inputRates;
		inputRates.resize(cuantasTasas);
		cout << "\tboot_zero_rates_fwds_floating_legs: inputRates resized" << endl;

		size_t cuantosFwds = fwdIndexVector.size();
		vector<shared_ptr<QCFXForward>> inputForwards;
		inputForwards.resize(cuantosFwds);
		cout << "\tboot_zero_rates_fwds_floating_legs: inputForwards resized" << endl;

		size_t cuantosSwaps = floatIndexVector.size();
		vector<shared_ptr<QCFloatingRatePayoff>> inputFloatingLegs;
		inputFloatingLegs.resize(cuantosSwaps);
		cout << "\tboot_zero_rates_fwds_floating_legs: inputFloatingLegs resized" << endl;

		vector<long> tenors;
		tenors.resize(cuantasTasas + cuantosFwds + cuantosSwaps);
		for (size_t i = 0; i < cuantasTasas; ++i)
		{
			tenors.at(i) = valueDate.dayDiff(get<1>(zeroRateVector.at(i)));
		}

		for (size_t i = cuantasTasas; i < cuantasTasas + cuantosFwds; ++i)
		{
			tenors.at(i) = valueDate.dayDiff(get<2>(fwdIndexVector.at(i - cuantasTasas)));
		}

		for (size_t i = cuantasTasas + cuantosFwds; i < cuantasTasas + cuantosFwds + cuantosSwaps; ++i)
		{
			tenors.at(i) = valueDate.dayDiff(get<2>(floatIndexVector.at(i - cuantasTasas - cuantosFwds)));
		}
		cout << "\tboot_zero_rates_fwds_floating_legs: curve tenors ok" << endl;

		vector<double> rates;
		rates.resize(cuantasTasas + cuantosFwds + cuantosSwaps);
		for (size_t i = 0; i < cuantasTasas + cuantosFwds + cuantosSwaps; ++i)
		{
			rates.at(i) = 0.0;
		}
		cout << "\tboot_zero_rates_fwds_fixed_legs: curve rates ok" << endl;

		QCZrCpnCrvShrdPtr curve = QCFactoryFunctions::zrCpnCrvShrdPtr(tenors, rates, curveInterpol,
			curveYf, curveWf);
		cout << "\tboot_zero_rates_fwds_fixed_legs: curve initialized" << endl;

		for (unsigned int i = 0; i < cuantasTasas; ++i)
		{
			QCInterestRateLeg tdLeg = QCFactoryFunctions::buildTimeDepositLeg(
				"R",
				get<0>(zeroRateVector.at(i)),
				get<1>(zeroRateVector.at(i)),
				1.0);
			//cout << "\tboot_zero_rates_floating_legs: time deposit leg " << i << " initialized" << endl;
			QCIntrstRtShrdPtr intRate = QCFactoryFunctions::intRateSharedPtr(
				get<2>(zeroRateVector.at(i)),
				get<3>(zeroRateVector.at(i)),
				get<4>(zeroRateVector.at(i)));
			//cout << "\tboot_zero_rates_floating_legs: interest rate " << i << " initialized" << endl;
			QCTimeDepositPayoff tdPayoff{ intRate, make_shared<QCInterestRateLeg>(tdLeg), valueDate, curve };
			inputRates.at(i) = make_shared<QCTimeDepositPayoff>(tdPayoff);
			//cout << "\tboot_zero_rates_floating_legs: time deposit payoff " << i << " initialized" << endl;
		}
		cout << "\tboot_zero_rates_floating_legs: time deposit payoffs initialized" << endl;

		map<QCCurrencyConverter::QCFxRateEnum, double> fxRate;
		vector<shared_ptr<QCDiscountBondPayoff>> legs;
		shared_ptr<map<QCDate, double>> fixings;
		legs.resize(2);
		fxRate.insert(pair<QCCurrencyConverter::QCFxRateEnum, double>(get<6>(fwdIndexVector.at(0)), fx));
		fxRate.insert(pair<QCCurrencyConverter::QCFxRateEnum, double>(get<7>(fwdIndexVector.at(0)), 1.0));
		string curveCurr{ PyString_AsString(PyList_GetItem(curveCharacteristics, 4)) };
		string forwardsFx{ PyString_AsString(PyTuple_GetItem(PyList_GetItem(fxForwards, 0), 3)) };
		string strongForwardFxCurr{ forwardsFx.substr(0, 3) };
		for (unsigned int i = 0; i < cuantosFwds; ++i)
		{
			//strong currency
			QCInterestRateLeg dbLeg0 = QCFactoryFunctions::buildDiscountBondLeg(
				"R", valueDate, get<2>(fwdIndexVector.at(i)),
				0, 0, mapHolidays.at(zeroRateCalendar), mapHolidays.at(zeroRateCalendar),
				get<0>(fwdIndexVector.at(i)));

			//weak currency
			QCInterestRateLeg dbLeg1 = QCFactoryFunctions::buildDiscountBondLeg(
				"P", valueDate, get<2>(fwdIndexVector.at(i)),
				0, 0, mapHolidays.at(zeroRateCalendar), mapHolidays.at(zeroRateCalendar),
				get<1>(fwdIndexVector.at(i)));

			//En este bloque estamos asumiendo que la curva a construir es la de la
			//moneda fuerte del par de los fwds. Hay que insertar un if para distinguir
			//si es asi o corresponde a la moneda debil.
			if (curveCurr == strongForwardFxCurr)
			{
				auto dbPayoff0 = make_shared<QCDiscountBondPayoff>(
					make_shared<QCInterestRateLeg>(dbLeg0),
					curve, valueDate, fixings,
					get<4>(fwdIndexVector.at(i)), get<6>(fwdIndexVector.at(i)));
				legs.at(0) = dbPayoff0;

				auto dbPayoff1 = make_shared<QCDiscountBondPayoff>(
					make_shared<QCInterestRateLeg>(dbLeg1),
					auxCurvesMap["FWDS"], valueDate, fixings,
					get<5>(fwdIndexVector.at(i)), get<7>(fwdIndexVector.at(i)));
				legs.at(1) = dbPayoff1;
			}
			else
			{
				auto dbPayoff0 = make_shared<QCDiscountBondPayoff>(
					make_shared<QCInterestRateLeg>(dbLeg0),
					auxCurvesMap["FWDS"], valueDate, fixings,
					get<4>(fwdIndexVector.at(i)), get<6>(fwdIndexVector.at(i)));
				legs.at(0) = dbPayoff0;

				auto dbPayoff1 = make_shared<QCDiscountBondPayoff>(
					make_shared<QCInterestRateLeg>(dbLeg1),
					curve, valueDate, fixings,
					get<5>(fwdIndexVector.at(i)), get<7>(fwdIndexVector.at(i)));
				legs.at(1) = dbPayoff1;

			}
			auto fxFwd = make_shared<QCFXForward>(legs, get<3>(fwdIndexVector.at(i)),
				make_shared<map<QCCurrencyConverter::QCFxRateEnum, double>>(fxRate));
			//cout << "\tboot_zero_rates_fwds_fixed_legs: Fx Forward " << i << " initialized" << endl;

			inputForwards.at(i) = fxFwd;
		}
		cout << "\tboot_zero_rates_fwds_fixed_legs: Fx Forwards initialized" << endl;

		vector<tuple<QCDate, double, double>> amortIfCustom;
		string floatLegFixingCalendar{ PyString_AsString(PyTuple_GetItem(PyList_GetItem(floatingLegs, 0), 16)) };
		map<QCDate, double> fixingMap;
		fixingMap.insert(make_pair(valueDate, fixing));
		for (unsigned int i = 0; i < cuantosSwaps; ++i)
		{
			//Se construye el interest rate
			shared_ptr<QCInterestRate> tmpIntRate = QCFactoryFunctions::intRateSharedPtr(
				get<QCDvePyBindHelperFunctions::QCFloatIndex::qcRate>(floatIndexVector.at(i)),
				get<QCDvePyBindHelperFunctions::QCFloatIndex::qcYearFraction>(floatIndexVector.at(i)),
				get<QCDvePyBindHelperFunctions::QCFloatIndex::qcWealthFactor>(floatIndexVector.at(i)));
			cout << "\tboot_zero_rates_fixed_legs: swap interest rate " << i << " initialized" << endl;

			//Este pedazo de código debe quedar en una HelperFunction de QC_DVE_XLL
			QCInterestRateLeg tmpFloatingLeg = QCFactoryFunctions::buildFloatingRateLeg2(
				get<QCDvePyBindHelperFunctions::QCFloatIndex::qcReceivePay>(floatIndexVector.at(i)),//receive or pay
				get<QCDvePyBindHelperFunctions::QCFloatIndex::qcStartDate>(floatIndexVector.at(i)),	//start date
				get<QCDvePyBindHelperFunctions::QCFloatIndex::qcEndDate>(floatIndexVector.at(i)),	//end date
				mapHolidays.at(floatLegSettlementCalendar),											//settlement calendar
				get<QCDvePyBindHelperFunctions::QCFloatIndex::qcSettlementLag>(floatIndexVector.at(i)),//settlement lag
				get<QCDvePyBindHelperFunctions::QCFloatIndex::qcStubPeriod>(floatIndexVector.at(i)),//stub period
				get<QCDvePyBindHelperFunctions::QCFloatIndex::qcPeriodicity>(floatIndexVector.at(i)),//periodicity
				get<QCDvePyBindHelperFunctions::QCFloatIndex::qcEndDateAdjustment>(floatIndexVector.at(i)),//end date adjustment
				get<QCDvePyBindHelperFunctions::QCFloatIndex::qcAmortization>(floatIndexVector.at(i)),//amortization
				amortIfCustom,							//amortization and notional by date
				get<QCDvePyBindHelperFunctions::QCFloatIndex::qcFixingLag>(floatIndexVector.at(i)),//fixing lag
				get<QCDvePyBindHelperFunctions::QCFloatIndex::qcFixingStubPeriod>(floatIndexVector.at(i)),//fixing stub period
				get<QCDvePyBindHelperFunctions::QCFloatIndex::qcFixingPeriodicity>(floatIndexVector.at(i)),//fixing periodicity
				mapHolidays.at(floatLegFixingCalendar), //fixing calendar
				make_pair(get<QCDvePyBindHelperFunctions::QCFloatIndex::qcFixingPeriodicity>(floatIndexVector.at(i)),
				to_string(get<QCDvePyBindHelperFunctions::QCFloatIndex::qcFixingLag>(floatIndexVector.at(i)))+"D"),
				get<QCDvePyBindHelperFunctions::QCFloatIndex::qcNotional>(floatIndexVector.at(i)));	//notional
			cout << "\tboot_zero_rates_fixed_legs: fixed rate interest leg " << i << " initialized" << endl;

			//buildFixedRatePayoff
			shared_ptr<QCFloatingRatePayoff> tmpIntRatePayoff = shared_ptr<QCFloatingRatePayoff>(
				new QCFloatingRatePayoff{ tmpIntRate,
				get<QCDvePyBindHelperFunctions::QCFloatIndex::qcSpread>(floatIndexVector.at(i)),
				1.0,
				make_shared<QCInterestRateLeg>(tmpFloatingLeg),
				auxCurvesMap.at("FLOATING_RATE_LEGS"),
				curve, valueDate, make_shared<map<QCDate, double>>(fixingMap) });
			cout << "\tboot_zero_rates_fixed_legs: fixed rate interest payoff " << i << " initialized" << endl;

			//Se agrega el payoff al vector de fixed legs
			inputFloatingLegs.at(i) = tmpIntRatePayoff;
		}

		//PyObject* temp = PyList_New(1);
		//return temp;

		//Se da de alta el objeto bootstrapeador
		QCZeroCurveBootstrappingFromRatesFwdsAndFloatingLegs boot{ valueDate, inputRates,
			inputForwards, 0, inputFloatingLegs, curve };
		cout << "\tboot_zero_rates_fwds_floating_legs: bootstrapping object initialized" << endl;

		boot.generateCurveAndDerivatives();
		cout << "\tboot_zero_rates_fwds_floating_legs: curve bootstrapped" << endl;

		vector<tuple<long, double, vector<double>>> result; //plazo, tasa, derivadas
		size_t tamagno = curve->getLength();
		result.resize(tamagno);
		for (size_t i = 0; i < tamagno; ++i)
		{
			get<2>(result.at(i)).resize(tamagno);
			pair<long, double> temp = curve->getValuesAt(i);
			get<0>(result.at(i)) = temp.first;
			get<1>(result.at(i)) = temp.second;
			for (size_t j = 0; j < tamagno; ++j)
			{
				get<2>(result.at(i)).at(j) = boot.getDerivativeAt(i, j);				
			}
		}
		cout << "\tboot_zero_rates_fwds_floating_legs: results obtained" << endl;

		PyObject* curveAndDeltas = PyList_New(tamagno);
		for (size_t i = 0; i < tamagno; ++i)
		{
			size_t columnas = 2 + tamagno;
			PyObject* temp = PyList_New(columnas);
			int success;
			success = PyList_SetItem(temp, 0, PyInt_FromLong(get<0>(result.at(i))));
			success = PyList_SetItem(temp, 1, PyFloat_FromDouble(get<1>(result.at(i))));
			for (unsigned int j = 0; j < tamagno; ++j)
			{
				success = PyList_SetItem(temp, j + 2, PyFloat_FromDouble(get<2>(result.at(i)).at(j)));
			}
			success = PyList_SetItem(curveAndDeltas, i, temp);
		}
		cout << "\tboot_zero_rates_fwds_floating_legs: output prepared" << endl;

		return curveAndDeltas;

	}
	catch (exception& e)
	{
		string msg = "Error en el bootstrapping Rates&Fwds&Floating_Legs. " + string(e.what());
		PyErr_SetString(qcDveError, msg.c_str());
		return NULL;
	}

}

PyObject* boot_zero_rates_floating_legs_proj_curve(PyObject* self, PyObject* args)
{
	//En la pata 0 vienen los spreads. La pata 1 es la pata Flot sin spread.
	cout << endl << "Enter function boot_zero_rates_floating_legs_proj_curve." << endl;
	char* fecha;
	double fixing0;
	double fixing1;
	int whichLeg;
	PyObject* holidays;
	PyObject* curveCharacteristics;
	PyObject* zeroRates;
	PyObject* floatingLegs0;
	PyObject* floatingLegs1;
	PyObject* auxCurveValues;
	PyObject* auxCurveChars;

	string nombreFuncion{ "\tboot_zero_rates_floating_legs_proj_curve: " };

	if (!PyArg_ParseTuple(args,
		"sddiO!O!O!O!O!O!O!",
		&fecha,
		&fixing0,
		&fixing1,
		&whichLeg,
		&PyList_Type, &holidays,
		&PyList_Type, &curveCharacteristics,
		&PyList_Type, &zeroRates,
		&PyList_Type, &floatingLegs0,
		&PyList_Type, &floatingLegs1,
		&PyList_Type, &auxCurveValues,
		&PyList_Type, &auxCurveChars))
	{
		string msg = nombreFuncion + "error en los argumentos.";
		PyErr_SetString(qcDveError, msg.c_str());
		return NULL;
	}
	try
	{
		//Se establece en true para enviar mensajes a la consola
		bool verbose = true;

		//Se construye la fecha de proceso
		string strDate{ fecha };
		QCDate valueDate{ strDate };
		if (verbose)
			cout << nombreFuncion << "fecha de proceso: " << valueDate.description() << endl;

		//Se construye el mapa con los feriados de cada calendario
		map<string, vector<QCDate>> mapHolidays;
		QCDvePyBindHelperFunctions::buildHolidays(holidays, mapHolidays);
		if (verbose)
			cout << nombreFuncion + "finished map holidays" << endl;

		//Se dan de alta las caracteristicas de la curva a construir
		string curveInterpol{ PyString_AsString(PyList_GetItem(curveCharacteristics, 0)) }; //Interpolacion
		string curveWf{ PyString_AsString(PyList_GetItem(curveCharacteristics, 1)) }; //wealth factor
		QCHelperFunctions::lowerCase(curveWf);
		string curveYf{ PyString_AsString(PyList_GetItem(curveCharacteristics, 2)) }; //year fraction
		QCHelperFunctions::lowerCase(curveYf);
		if (verbose)
			cout << nombreFuncion + "finished curve characteristics." << endl;

		//Se dan de alta las caracteristicas de las curvas auxiliares
		size_t numAuxCurves = PyList_Size(auxCurveChars);
		map<string, QCZrCpnCrvShrdPtr> auxCurvesMap;
		for (size_t i = 0; i < numAuxCurves; ++i)
		{
			string auxCurveInterpol{ PyString_AsString(PyTuple_GetItem(
				PyList_GetItem(auxCurveChars, i), 2)) }; //Interpolacion
			string auxCurveWf{ PyString_AsString(PyTuple_GetItem(
				PyList_GetItem(auxCurveChars, i), 3)) }; //wealth factor
			QCHelperFunctions::lowerCase(auxCurveWf);
			string auxCurveYf{ PyString_AsString(PyTuple_GetItem(
				PyList_GetItem(auxCurveChars, i), 4)) }; //year fraction
			QCHelperFunctions::lowerCase(auxCurveYf);

			//Se construye el objeto curva auxiliar
			vector<long> auxTenors;
			vector<double> auxRates;
			string queCurva = PyString_AsString(PyTuple_GetItem(PyList_GetItem(auxCurveChars, i), 0));
			QCDvePyBindHelperFunctions::buildAuxTenorsAndRates(auxCurveValues, auxTenors, auxRates, queCurva);
			QCZrCpnCrvShrdPtr auxCrvPtr = QCFactoryFunctions::zrCpnCrvShrdPtr(auxTenors, auxRates,
				auxCurveInterpol, auxCurveYf, auxCurveWf);
			string whichInput{ PyString_AsString(PyTuple_GetItem(PyList_GetItem(auxCurveChars, i), 1)) };
			auxCurvesMap.insert(make_pair(whichInput, auxCrvPtr));
		}
		if (verbose)
			cout << nombreFuncion + "aux curve initialized" << endl;

		//Se construye un vector con los datos de las tasas cero iniciales
		vector<QCDvePyBindHelperFunctions::ZeroRate> zeroRateVector;
		string zeroRateCalendar{ PyString_AsString(PyTuple_GetItem(PyList_GetItem(zeroRates, 0), 3)) };
		QCDvePyBindHelperFunctions::buildZeroRateVector(zeroRates, valueDate,
			mapHolidays.at(zeroRateCalendar), zeroRateVector);
		if (verbose)
			cout << nombreFuncion + "finished zero rates." << endl;

		size_t cuantasTasas = zeroRateVector.size();
		cout << "cuantas tasas: " << cuantasTasas << endl;
		vector<shared_ptr<QCTimeDepositPayoff>> inputRates;
		inputRates.resize(cuantasTasas);
		if (verbose)
			cout << nombreFuncion + "inputRates resized." << endl;

		//Se construye un vector con las patas flotantes iniciales 0
		vector<QCDvePyBindHelperFunctions::FloatIndex> floatIndexVector0;
		string floatLegSettlementCalendar0{ PyString_AsString(PyTuple_GetItem(PyList_GetItem(floatingLegs0, 0), 6)) };
		QCDvePyBindHelperFunctions::buildFloatingRateIndexVector(floatingLegs0, valueDate,
			mapHolidays.at(floatLegSettlementCalendar0), floatIndexVector0);
		if (verbose)
			cout << nombreFuncion + "finished floating rate legs 0." << endl;

		//Se construye un vector con las patas flotantes iniciales 1
		vector<QCDvePyBindHelperFunctions::FloatIndex> floatIndexVector1;
		size_t numSwaps = PyList_Size(floatingLegs1);
		//cout << "numFloatingLegs1: " << numSwaps << endl;
		string floatLegSettlementCalendar1 = { PyString_AsString(PyTuple_GetItem(PyList_GetItem(floatingLegs1, 0), 6)) };
		//cout << "floatLegSettlementCalendar1: " << floatLegSettlementCalendar1 << endl;
		QCDvePyBindHelperFunctions::buildFloatingRateIndexVector(floatingLegs1, valueDate,
			mapHolidays.at(floatLegSettlementCalendar1), floatIndexVector1, "P");
		if (verbose)
			cout << nombreFuncion + "finished floating rate legs 1." << endl;

		size_t cuantosSwaps = floatIndexVector0.size();
		vector<shared_ptr<QCFloatingRatePayoff>> inputFloatingLegs0;
		inputFloatingLegs0.resize(cuantosSwaps);
		if (verbose)
			cout << nombreFuncion + "inputFloatingLegs0 resized" << endl;

		vector<shared_ptr<QCFloatingRatePayoff>> inputFloatingLegs1;
		inputFloatingLegs1.resize(cuantosSwaps);
		if (verbose)
			cout << nombreFuncion + "inputFloatingLegs1 resized" << endl;

		vector<shared_ptr<QCInterestRateBasisSwap>> inputBasisSwaps;
		inputBasisSwaps.resize(cuantosSwaps);
		if (verbose)
			cout << nombreFuncion + "inputBasisSwaps resized" << endl;

		vector<long> tenors;
		tenors.resize(cuantasTasas + cuantosSwaps);
		for (size_t i = 0; i < cuantasTasas; ++i)
		{
			tenors.at(i) = valueDate.dayDiff(get<1>(zeroRateVector.at(i)));
		}

		for (size_t i = cuantasTasas; i < cuantasTasas + cuantosSwaps; ++i)
		{
			tenors.at(i) = valueDate.dayDiff(get<2>(floatIndexVector0.at(i - cuantasTasas)));
		}
		if (verbose)
			cout << nombreFuncion + "curve tenors ok" << endl;

		vector<double> rates;
		rates.resize(cuantasTasas + cuantosSwaps);
		//Hay que ordenar por fecha final ascendente el vector de zero rates. 
		for (size_t i = 0; i < cuantasTasas + cuantosSwaps; ++i)
		{
			if (i < cuantasTasas)
			{
				rates.at(i) = get<2>(zeroRateVector.at(i));
				//Esto hay que arreglarlo para que entre el valor en la convencion correcta
				//Hay que hacer un helper function que tome la tupla y la yf y wf deseada y devuelva
				//el valor transformado.
			}
			else
			{
				rates.at(i) = get<2>(zeroRateVector.at(0)); // 0.0;
			}
		}
		if (verbose)
			cout << nombreFuncion + "curve rates ok" << endl;

		QCZrCpnCrvShrdPtr curve = QCFactoryFunctions::zrCpnCrvShrdPtr(tenors, rates, curveInterpol,
			curveYf, curveWf);
		if (verbose)
			cout << nombreFuncion + "curve initialized" << endl;

		vector<tuple<QCDate, double, double>> amortIfCustom;
		string floatLegFixingCalendar0{ PyString_AsString(PyTuple_GetItem(PyList_GetItem(floatingLegs0, 0), 16)) };
		map<QCDate, double> fixingMap0;
		fixingMap0.insert(make_pair(valueDate, fixing0));

		string floatLegFixingCalendar1{ PyString_AsString(PyTuple_GetItem(PyList_GetItem(floatingLegs1, 0), 16)) };
		map<QCDate, double> fixingMap1;
		fixingMap1.insert(make_pair(valueDate, fixing1));
		for (unsigned int i = 0; i < cuantosSwaps; ++i)
		{
			vector<shared_ptr<QCFloatingRatePayoff>> tempFloatingLegs;
			tempFloatingLegs.resize(2);

			//************** Primero Pata 0 ***********************

			//Se construye el interest rate
			shared_ptr<QCInterestRate> tmpIntRate0 = QCFactoryFunctions::intRateSharedPtr(
				get<QCDvePyBindHelperFunctions::QCFloatIndex::qcRate>(floatIndexVector0.at(i)),
				get<QCDvePyBindHelperFunctions::QCFloatIndex::qcYearFraction>(floatIndexVector0.at(i)),
				get<QCDvePyBindHelperFunctions::QCFloatIndex::qcWealthFactor>(floatIndexVector0.at(i)));
			if (verbose)
				cout << nombreFuncion + "swap interest rate 0" << i << " initialized" << endl;

			//Este pedazo de código debe quedar en una HelperFunction de QC_DVE_XLL
			QCInterestRateLeg tmpFloatingLeg0 = QCFactoryFunctions::buildFloatingRateLeg2(
				get<QCDvePyBindHelperFunctions::QCFloatIndex::qcReceivePay>(floatIndexVector0.at(i)),//receive or pay
				get<QCDvePyBindHelperFunctions::QCFloatIndex::qcStartDate>(floatIndexVector0.at(i)),	//start date
				get<QCDvePyBindHelperFunctions::QCFloatIndex::qcEndDate>(floatIndexVector0.at(i)),	//end date
				mapHolidays.at(floatLegSettlementCalendar0),											//settlement calendar
				get<QCDvePyBindHelperFunctions::QCFloatIndex::qcSettlementLag>(floatIndexVector0.at(i)),//settlement lag
				get<QCDvePyBindHelperFunctions::QCFloatIndex::qcStubPeriod>(floatIndexVector0.at(i)),//stub period
				get<QCDvePyBindHelperFunctions::QCFloatIndex::qcPeriodicity>(floatIndexVector0.at(i)),//periodicity
				get<QCDvePyBindHelperFunctions::QCFloatIndex::qcEndDateAdjustment>(floatIndexVector0.at(i)),//end date adjustment
				get<QCDvePyBindHelperFunctions::QCFloatIndex::qcAmortization>(floatIndexVector0.at(i)),//amortization
				amortIfCustom,							//amortization and notional by date
				get<QCDvePyBindHelperFunctions::QCFloatIndex::qcFixingLag>(floatIndexVector0.at(i)),//fixing lag
				get<QCDvePyBindHelperFunctions::QCFloatIndex::qcFixingStubPeriod>(floatIndexVector0.at(i)),//fixing stub period
				get<QCDvePyBindHelperFunctions::QCFloatIndex::qcFixingPeriodicity>(floatIndexVector0.at(i)),//fixing periodicity
				mapHolidays.at(floatLegFixingCalendar0), //fixing calendar
				make_pair(get<QCDvePyBindHelperFunctions::QCFloatIndex::qcFixingPeriodicity>(floatIndexVector0.at(i)),
				to_string(get<QCDvePyBindHelperFunctions::QCFloatIndex::qcFixingLag>(floatIndexVector0.at(i))) + "D"),
				get<QCDvePyBindHelperFunctions::QCFloatIndex::qcNotional>(floatIndexVector0.at(i)));	//notional
			if (verbose)
				cout << nombreFuncion + "floating rate interest leg 0" << i << " initialized" << endl;

			//buildFloatingRatePayoff
			shared_ptr<QCFloatingRatePayoff> tmpIntRatePayoff0;
			if (whichLeg == 0)
			{
				tmpIntRatePayoff0 = shared_ptr<QCFloatingRatePayoff>(
					new QCFloatingRatePayoff{ tmpIntRate0,
					get<QCDvePyBindHelperFunctions::QCFloatIndex::qcSpread>(floatIndexVector0.at(i)),
					1.0,
					make_shared<QCInterestRateLeg>(tmpFloatingLeg0),
					curve, auxCurvesMap.at("BASIS_SWAPS"), valueDate,
					make_shared<map<QCDate, double>>(fixingMap0) });
			}
			else
			{
				tmpIntRatePayoff0 = shared_ptr<QCFloatingRatePayoff>(
					new QCFloatingRatePayoff{ tmpIntRate0,
					get<QCDvePyBindHelperFunctions::QCFloatIndex::qcSpread>(floatIndexVector0.at(i)),
					1.0,
					make_shared<QCInterestRateLeg>(tmpFloatingLeg0),
					auxCurvesMap.at("BASIS_SWAPS"), auxCurvesMap.at("BASIS_SWAPS"), valueDate,
					make_shared<map<QCDate, double>>(fixingMap0) });

			}
			if (verbose)
				cout << nombreFuncion + "floating rate interest payoff 0" << i << " initialized" << endl;
			//Se agrega el payoff al vector de floating legs 0
			tempFloatingLegs.at(0) = tmpIntRatePayoff0;

			//****************** Ahora Pata 1 *************************

			//Se construye el interest rate
			shared_ptr<QCInterestRate> tmpIntRate1 = QCFactoryFunctions::intRateSharedPtr(
				get<QCDvePyBindHelperFunctions::QCFloatIndex::qcRate>(floatIndexVector1.at(i)),
				get<QCDvePyBindHelperFunctions::QCFloatIndex::qcYearFraction>(floatIndexVector1.at(i)),
				get<QCDvePyBindHelperFunctions::QCFloatIndex::qcWealthFactor>(floatIndexVector1.at(i)));
			cout << nombreFuncion + "swap interest rate 1" << i << " initialized" << endl;

			//Este pedazo de código debe quedar en una HelperFunction de QC_DVE_XLL
			QCInterestRateLeg tmpFloatingLeg1 = QCFactoryFunctions::buildFloatingRateLeg2(
				get<QCDvePyBindHelperFunctions::QCFloatIndex::qcReceivePay>(floatIndexVector1.at(i)),//receive or pay
				get<QCDvePyBindHelperFunctions::QCFloatIndex::qcStartDate>(floatIndexVector1.at(i)),	//start date
				get<QCDvePyBindHelperFunctions::QCFloatIndex::qcEndDate>(floatIndexVector1.at(i)),	//end date
				mapHolidays.at(floatLegSettlementCalendar1),											//settlement calendar
				get<QCDvePyBindHelperFunctions::QCFloatIndex::qcSettlementLag>(floatIndexVector1.at(i)),//settlement lag
				get<QCDvePyBindHelperFunctions::QCFloatIndex::qcStubPeriod>(floatIndexVector1.at(i)),//stub period
				get<QCDvePyBindHelperFunctions::QCFloatIndex::qcPeriodicity>(floatIndexVector1.at(i)),//periodicity
				get<QCDvePyBindHelperFunctions::QCFloatIndex::qcEndDateAdjustment>(floatIndexVector1.at(i)),//end date adjustment
				get<QCDvePyBindHelperFunctions::QCFloatIndex::qcAmortization>(floatIndexVector1.at(i)),//amortization
				amortIfCustom,							//amortization and notional by date
				get<QCDvePyBindHelperFunctions::QCFloatIndex::qcFixingLag>(floatIndexVector1.at(i)),//fixing lag
				get<QCDvePyBindHelperFunctions::QCFloatIndex::qcFixingStubPeriod>(floatIndexVector1.at(i)),//fixing stub period
				get<QCDvePyBindHelperFunctions::QCFloatIndex::qcFixingPeriodicity>(floatIndexVector1.at(i)),//fixing periodicity
				mapHolidays.at(floatLegFixingCalendar1), //fixing calendar
				make_pair(get<QCDvePyBindHelperFunctions::QCFloatIndex::qcFixingPeriodicity>(floatIndexVector1.at(i)),
				to_string(get<QCDvePyBindHelperFunctions::QCFloatIndex::qcFixingLag>(floatIndexVector1.at(i))) + "D"),
				get<QCDvePyBindHelperFunctions::QCFloatIndex::qcNotional>(floatIndexVector1.at(i)));	//notional
			if (verbose)
				cout << nombreFuncion + "floating rate interest leg 0" << i << " initialized" << endl;

			//buildFloatingRatePayoff
			shared_ptr<QCFloatingRatePayoff> tmpIntRatePayoff1;
			if (whichLeg == 1)
			{
				tmpIntRatePayoff1 = shared_ptr<QCFloatingRatePayoff>(
					new QCFloatingRatePayoff{ tmpIntRate1,
					get<QCDvePyBindHelperFunctions::QCFloatIndex::qcSpread>(floatIndexVector1.at(i)),
					1.0,
					make_shared<QCInterestRateLeg>(tmpFloatingLeg1),
					curve, auxCurvesMap.at("BASIS_SWAPS"), valueDate,
					make_shared<map<QCDate, double>>(fixingMap1) });
			}
			else
			{
				tmpIntRatePayoff1 = shared_ptr<QCFloatingRatePayoff>(
					new QCFloatingRatePayoff{ tmpIntRate1,
					get<QCDvePyBindHelperFunctions::QCFloatIndex::qcSpread>(floatIndexVector1.at(i)),
					1.0,
					make_shared<QCInterestRateLeg>(tmpFloatingLeg1),
					auxCurvesMap.at("BASIS_SWAPS"), auxCurvesMap.at("BASIS_SWAPS"), valueDate,
					make_shared<map<QCDate, double>>(fixingMap1) });
			}
			if (verbose)
				cout << nombreFuncion + "floating rate interest payoff 1" << i << " initialized" << endl;

			//Se agrega el payoff al vector de floating legs 1
			tempFloatingLegs.at(1) = tmpIntRatePayoff1;

			QCInterestRateBasisSwap tempBasisSwap{ tempFloatingLegs };
			inputBasisSwaps.at(i) = make_shared<QCInterestRateBasisSwap>(tempBasisSwap);
		}

		//Se construye el vector con los Time Deposit asociados a los zero rate
		for (unsigned int i = 0; i < cuantasTasas; ++i)
		{
			QCInterestRateLeg tdLeg = QCFactoryFunctions::buildTimeDepositLeg(
				"R",
				get<0>(zeroRateVector.at(i)),
				get<1>(zeroRateVector.at(i)),
				1.0);
			cout << nombreFuncion + "time deposit leg " << i << " initialized" << endl;
			QCIntrstRtShrdPtr intRate = QCFactoryFunctions::intRateSharedPtr(
				get<2>(zeroRateVector.at(i)),
				get<3>(zeroRateVector.at(i)),
				get<4>(zeroRateVector.at(i)));
			cout << nombreFuncion + "interest rate " << i << " initialized" << endl;
			QCTimeDepositPayoff tdPayoff{ intRate, make_shared<QCInterestRateLeg>(tdLeg), valueDate, curve };
			inputRates.at(i) = make_shared<QCTimeDepositPayoff>(tdPayoff);
			cout << nombreFuncion + "time deposit payoff " << i << " initialized" << endl;

		}
		
		//Se da de alta el objeto bootstrapeador
		QCZeroCurveBootstrappingFromRatesAndFloatingLegsProjCurve boot{ valueDate, inputRates,
			inputBasisSwaps, static_cast<unsigned int>(whichLeg), curve };
		if (verbose)
			cout << nombreFuncion + "bootstrapping object initialized" << endl;

		boot.generateCurveAndDerivatives();
		if (verbose)
			cout << nombreFuncion + "curve bootstrapped" << endl;

		vector<tuple<long, double, vector<double>>> result; //plazo, tasa, derivadas
		size_t tamagno = curve->getLength();
		result.resize(tamagno);
		for (size_t i = 0; i < tamagno; ++i)
		{
			get<2>(result.at(i)).resize(tamagno);
			pair<long, double> temp = curve->getValuesAt(i);
			get<0>(result.at(i)) = temp.first;
			get<1>(result.at(i)) = temp.second;
			for (size_t j = 0; j < tamagno; ++j)
			{
				get<2>(result.at(i)).at(j) = boot.getDerivativeAt(i, j);
			}
		}
		cout << nombreFuncion + "results obtained" << endl;

		PyObject* curveAndDeltas = PyList_New(tamagno);
		for (size_t i = 0; i < tamagno; ++i)
		{
			size_t columnas = 2 + tamagno;
			PyObject* temp = PyList_New(columnas);
			int success;
			success = PyList_SetItem(temp, 0, PyInt_FromLong(get<0>(result.at(i))));
			success = PyList_SetItem(temp, 1, PyFloat_FromDouble(get<1>(result.at(i))));
			for (unsigned int j = 0; j < tamagno; ++j)
			{
				success = PyList_SetItem(temp, j + 2, PyFloat_FromDouble(get<2>(result.at(i)).at(j)));
			}
			success = PyList_SetItem(curveAndDeltas, i, temp);
		}
		cout << nombreFuncion + "output prepared" << endl;

		return curveAndDeltas;

	}
	catch (exception& e)
	{
		string msg = "Error en el bootstrapping Rates&Floating_Legs&Proj_Curve. " + string(e.what());
		PyErr_SetString(qcDveError, msg.c_str());
		return NULL;
	}

}

PyObject* pv_fixed_rate_legs(PyObject* self, PyObject*  args)
{
	cout << "Enter function pv_fixed_rate_legs." << endl;
	char* fecha;
	PyObject* holidays;
	PyObject* curveValues;
	PyObject* curveCharacteristics;
	PyObject* legCharacteristics;
	PyObject* customAmort;


	if (!PyArg_ParseTuple(args,
		"sO!O!O!O!O!",
		&fecha,
		&PyList_Type, &holidays,
		&PyList_Type, &curveValues,
		&PyList_Type, &curveCharacteristics,
		&PyList_Type, &legCharacteristics,
		&PyList_Type, &customAmort))
	{
		return NULL;
	}
	double numOp;
	try
	{
		map<string, vector<QCDate>> mapHolidays;
		QCDvePyBindHelperFunctions::buildHolidays(holidays, mapHolidays);
		cout << "	Finished map holidays" << endl;

		if (PyList_Size(curveValues) == 0)
		{
			string msg = "Error en las patas fijas. El vector de curvas esta vacio";
			PyErr_SetString(qcDveError, msg.c_str());
			return NULL;
		}
		map<string, pair<vector<long>, vector<double>>> crvValues;
		QCDvePyBindHelperFunctions::buildCurveValues(curveValues, crvValues);
		cout << "	Finished map curve values" << endl;

		map<string, QCDvePyBindHelperFunctions::string4> crvChars;
		QCDvePyBindHelperFunctions::buildCurveCharacteristics(curveCharacteristics, crvChars);
		cout << "	Finished map curve characteristics" << endl;

		map<string, shared_ptr<QCZeroCouponCurve>> allCurves;

		//Loopeo sobre los keys de crvValues
		for (const auto &curva : crvValues)
		{
			vector<long> tmpLng{ curva.second.first };
			vector<double> tmpDbl{ curva.second.second };

			string wf = get<1>(crvChars.at(curva.first));
			QCHelperFunctions::lowerCase(wf);

			string yf = get<2>(crvChars.at(curva.first));
			QCHelperFunctions::lowerCase(yf);

			shared_ptr<QCZeroCouponCurve> tmpCrv = QCFactoryFunctions::zrCpnCrvShrdPtr(
				tmpLng,
				tmpDbl,
				get<0>(crvChars.at(curva.first)),
				wf, yf, get<3>(crvChars.at(curva.first)));
			allCurves.insert(pair <string, shared_ptr<QCZeroCouponCurve>>(curva.first, tmpCrv));
		}
		cout << "	Finished constructing curve objects" << endl;

		//Guardaremos el list customAmort con los datos de amortizacion y nominal vigente
		//(en ese orden) en esta estructura.
		if (PyList_Size(customAmort) == 0)
		{
			string msg = "Error en las patas fijas. El vector de amortizaciones esta vacio";
			PyErr_SetString(qcDveError, msg.c_str());
			return NULL;
		}
		map<unsigned long, vector<tuple<QCDate, double, double>>> dateNotionalAndAmortByIdLeg;
		QCDvePyBindHelperFunctions::buildCustomAmortization(customAmort, dateNotionalAndAmortByIdLeg);
		cout << "	Finished amortization map" << endl;

		//Ahora hay que construir QCInterestRatePayoff para cada operacion
		//Se requiere:
		//	shared_ptr<QCInterestRate>
		//	shared_ptr<QCInterestRateLeg>
		//	QCDate (valueDate)
		//	shared_ptr<QCZeroCouponCurve> (la curva de descuento)
		//	shared_ptr<QCTimeSeries> (los fixings)

		//La info esta en la list legCharacteristics:
		//	0:	id_leg
		//	1:	rec_pay
		//	2:	start_date
		//	3:	end_date
		//	4:	settlement_calendar
		//	5:	settlement_lag
		//	6:	stub_period
		//	7:	periodicity
		//	8:	end_date_adjustment
		//	9:	amortization
		//	10: fixed_rate
		//	11: notional
		//	12: wealth_factor
		//	13: year_fraction
		//	14: discount_curve

		string strDate{ fecha };
		QCDate allValueDate{ strDate };

		map <long, shared_ptr<QCInterestRatePayoff>> payoffs;
		for (unsigned long i = 0; i < PyList_Size(legCharacteristics); ++i)
		{
			string wf = string(PyString_AsString(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 12)));
			QCHelperFunctions::lowerCase(wf);
			string yf = string(PyString_AsString(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 13)));
			QCHelperFunctions::lowerCase(yf);
			shared_ptr<QCInterestRate> tmpIntRate = QCFactoryFunctions::intRateSharedPtr(
				PyFloat_AsDouble(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 10)),
				yf, wf);

			numOp = PyFloat_AsDouble(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 0));
			cout << "\tnumOp: " << numOp << endl;
			vector<tuple<QCDate, double, double>> amortIfCustom;
			if (string(PyString_AsString(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 9))) != "BULLET")
			{
				amortIfCustom = dateNotionalAndAmortByIdLeg.at((unsigned long)numOp);
			}
			cout << "\tAmortizaciones Ok." << endl;
			QCInterestRateLeg tmpIntRateLeg = QCFactoryFunctions::buildFixedRateLeg2(
				string(PyString_AsString(PyList_GetItem(
				PyList_GetItem(legCharacteristics, i), 1))),//receive or pay
				QCDate{ string(PyString_AsString(PyList_GetItem(
				PyList_GetItem(legCharacteristics, i), 2))) }, //start date
				QCDate{ string(PyString_AsString(PyList_GetItem(
				PyList_GetItem(legCharacteristics, i), 3))) }, //end date
				mapHolidays.at(string(PyString_AsString(
				PyList_GetItem(PyList_GetItem(legCharacteristics, i), 4)))),  //settlement calendar
				PyInt_AsLong(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 5)), //settlement lag
				QCHelperFunctions::stringToQCStubPeriod(
				string(PyString_AsString(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 6)))), //stub period
				string(PyString_AsString(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 7))), //periodicity
				QCHelperFunctions::stringToQCBusDayAdjRule(
				string(PyString_AsString(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 8)))),//end date adjustment
				QCHelperFunctions::stringToQCAmortization(
				string(PyString_AsString(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 9)))), //amortization
				amortIfCustom,										//amortization and notional by end date
				PyFloat_AsDouble(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 11))		 //notional
				);
			cout << "\tInterest Rate Leg constructed" << endl;
			shared_ptr<QCInterestRatePayoff> tmpIntRatePayoff = shared_ptr<QCInterestRatePayoff>(
				new QCFixedRatePayoff{ tmpIntRate, make_shared<QCInterestRateLeg>(tmpIntRateLeg),
				allCurves.at(string(PyString_AsString(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 14)))),
				allValueDate, nullptr });
			payoffs.insert(pair<long, shared_ptr<QCInterestRatePayoff>>(static_cast<long>(numOp), tmpIntRatePayoff));
		}
		cout << "	Finished constructing payoffs" << endl;

		//Calcular los valores presentes
		vector<tuple<long, double, double, vector<double>>> result;
		result.resize(payoffs.size());
		double m2m;
		double valueDateCashflow;
		vector<double> der;
		unsigned long counter = 0;
		unsigned int longestCurve = 0;
		for (const auto& payoff : payoffs)
		{
			m2m = payoff.second->presentValue();
			valueDateCashflow = payoff.second->getValueDateCashflow();
			unsigned int vertices = payoff.second->discountCurveLength();
			der.resize(vertices);
			for (unsigned long i = 0; i < vertices; ++i)
			{
				der.at(i) = payoff.second->getPvRateDerivativeAt(i);
			}
			result.at(counter) = make_tuple(payoff.first, m2m, valueDateCashflow, der);
			der.clear();
			++counter;
		}
		cout << "	Finished present value and sensitivities" << endl;

		long cuantosM2M = PyList_Size(legCharacteristics);
		PyObject* legM2M = PyList_New(cuantosM2M);
		for (long i = 0; i < cuantosM2M; ++i)
		{
			unsigned int vertices = get<3>(result.at(i)).size();
			PyObject* temp = PyList_New(vertices + 3);
			int success;
			success = PyList_SetItem(temp, 0, PyInt_FromLong(get<0>(result.at(i))));
			success = PyList_SetItem(temp, 1, PyFloat_FromDouble(get<1>(result.at(i))));
			success = PyList_SetItem(temp, 2, PyFloat_FromDouble(get<2>(result.at(i))));
			for (unsigned int j = 0; j < vertices; ++j)
			{
				success = PyList_SetItem(temp, j + 3, PyFloat_FromDouble(get<3>(result.at(i)).at(j) * BASIS_POINT));
			}
			success = PyList_SetItem(legM2M, i, temp);
		}
		cout << "	Output prepared" << endl;

		return legM2M;
	}
	catch (exception& e)
	{
		string msg = "Error en las patas fijas. " + string(e.what()) + " numOp: " + to_string(numOp);
		PyErr_SetString(qcDveError, msg.c_str());
		return NULL;
	}
}

PyObject* pv_floating_rate_legs(PyObject* self, PyObject*  args)
{
	cout << "Enter function pv_floating_rate_legs" << endl;
	char* fecha;
	PyObject* holidays;
	PyObject* curveValues;
	PyObject* curveCharacteristics;
	PyObject* legCharacteristics;
	PyObject* customAmort;
	PyObject* fixings;
	PyObject* intRateIndexChars;

	if (!PyArg_ParseTuple(args,
		"sO!O!O!O!O!O!O!",
		&fecha,
		&PyList_Type, &holidays,
		&PyList_Type, &curveValues,
		&PyList_Type, &curveCharacteristics,
		&PyList_Type, &legCharacteristics,
		&PyList_Type, &customAmort,
		&PyList_Type, &fixings,
		&PyList_Type, &intRateIndexChars))
	{
		return NULL;
	}
	double numOp;
	try
	{
		map<string, vector<QCDate>> mapHolidays;
		QCDvePyBindHelperFunctions::buildHolidays(holidays, mapHolidays);
		cout << "\tFinished map holidays" << endl;

		map<string, pair<vector<long>, vector<double>>> crvValues;
		QCDvePyBindHelperFunctions::buildCurveValues(curveValues, crvValues);
		cout << "\tFinished map curve values" << endl;

		map<string, QCDvePyBindHelperFunctions::string4> crvChars;
		QCDvePyBindHelperFunctions::buildCurveCharacteristics(curveCharacteristics, crvChars);
		cout << "\tFinished map curve characteristics" << endl;

		map<string, shared_ptr<QCZeroCouponCurve>> allCurves;

		//Loopeo sobre los keys de crvValues
		for (const auto &curva : crvValues)
		{
			vector<long> tmpLng{ curva.second.first };
			vector<double> tmpDbl{ curva.second.second };

			string wf = get<1>(crvChars.at(curva.first));
			QCHelperFunctions::lowerCase(wf);

			string yf = get<2>(crvChars.at(curva.first));
			QCHelperFunctions::lowerCase(yf);

			shared_ptr<QCZeroCouponCurve> tmpCrv = QCFactoryFunctions::zrCpnCrvShrdPtr(
				tmpLng,
				tmpDbl,
				get<0>(crvChars.at(curva.first)),
				wf, yf, get<3>(crvChars.at(curva.first)));
			allCurves.insert(pair <string, shared_ptr<QCZeroCouponCurve>>(curva.first, tmpCrv));
		}
		cout << "\tFinished constructing curve objects" << endl;
		
		//Guardaremos el list customAmort con los datos de amortizacion y nominal vigente
		//(en ese orden) en esta estructura.
		if (PyList_Size(customAmort) == 0)
		{
			string msg = "Error en las patas flotantes. El vector de amortizaciones esta vacio";
			PyErr_SetString(qcDveError, msg.c_str());
			return NULL;
		}
		map<unsigned long, vector<tuple<QCDate, double, double>>> dateNotionalAndAmortByIdLeg;
		QCDvePyBindHelperFunctions::buildCustomAmortization(customAmort, dateNotionalAndAmortByIdLeg);
		cout << "\tFinished amortizations" << endl;

		//Metemos los fixings de los indices de tasa flotante en esta estructura
		map<string, map<QCDate, double>> mapManyFixings;
		QCDvePyBindHelperFunctions::buildManyFixings(fixings, mapManyFixings);
		cout << "\tFinished fixings" << endl;

		//Metemos las caracteristicas de los indices en esta estructura
		map<string, pair<string, string>> indexChars; //en el pair viene el tenor y el start date rule
		QCDvePyBindHelperFunctions::buildStringPairStringMap(intRateIndexChars, indexChars);
		cout << "\tFinished interest rate index characteristics" << endl;

		//Ahora hay que construir QCInterestRatePayoff para cada operacion
		//Se requiere:
		//	shared_ptr<QCInterestRate>
		//	shared_ptr<QCInterestRateLeg>
		//	QCDate (valueDate)
		//	shared_ptr<QCInterestRateCurve> (la curva de proyeccion)
		//	shared_ptr<QCInterestCurve> (la curva de descuento)
		//	shared_ptr<QCTimeSeries> (los fixings)

		//La info esta en la CellMatrix legCharacteristics:
		//0:	id_leg
		//1:	rec_pay
		//2:	start_date
		//3:	end_date
		//4:	settlement_calendar
		//5:	settlement_lag
		//6:	stub_period
		//7:	periodicity
		//8:	end_date_adjustment
		//9:	amortization
		//10:	interest_rate_index
		//11:	rate_value
		//12:	spread
		//13:	fixing_stub_period
		//14:	fixing_periodicity
		//15:	fixing_calendar
		//16:	fixing_lag
		//17:	notional_currency
		//18:	notional
		//19:	wealth_factor
		//20:	year_fraction
		//21:	projecting_curve
		//22:	discount_curve

		string strDate{ fecha };
		QCDate allValueDate{ strDate };

		map <long, shared_ptr<QCInterestRatePayoff>> payoffs;
		for (unsigned long i = 0; i < PyList_Size(legCharacteristics); ++i)
		{
			string wf = PyString_AsString(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 19));
			QCHelperFunctions::lowerCase(wf);
			string yf = PyString_AsString(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 20));
			QCHelperFunctions::lowerCase(yf);

			shared_ptr<QCInterestRate> tmpIntRate = QCFactoryFunctions::intRateSharedPtr(
				PyFloat_AsDouble(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 11)), yf, wf);

			numOp = (long)PyFloat_AsDouble(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 0));
			//cout << "numOp: " << numOp << endl;
			vector<tuple<QCDate, double, double>> amortIfCustom;
			if (PyString_AsString(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 9)) != "BULLET")
			{
				amortIfCustom = dateNotionalAndAmortByIdLeg.at(numOp);
			}
			QCInterestRateLeg tmpIntRateLeg = QCFactoryFunctions::buildFloatingRateLeg2(
				PyString_AsString(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 1)),					 //receive or pay
				QCDate{ string(PyString_AsString(PyList_GetItem(
				PyList_GetItem(legCharacteristics, i), 2))) }, //start date
				QCDate{ string(PyString_AsString(PyList_GetItem(
				PyList_GetItem(legCharacteristics, i), 3))) }, //end date
				mapHolidays.at(PyString_AsString(PyList_GetItem(PyList_GetItem(
				legCharacteristics, i), 4))),  //settlement calendar
				PyInt_AsLong(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 5)),	//settlement lag
				QCHelperFunctions::stringToQCStubPeriod(
				PyString_AsString(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 6))),//stub period
				PyString_AsString(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 7)),//periodicity
				QCHelperFunctions::stringToQCBusDayAdjRule(
				PyString_AsString(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 8))),//end date adjustment
				QCHelperFunctions::stringToQCAmortization(
				PyString_AsString(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 9))),//amortization
				amortIfCustom,											 //amortization and notional by end date
				PyInt_AsLong(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 16)),//fixing lag
				QCHelperFunctions::stringToQCStubPeriod(
				PyString_AsString(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 13))),//fixing stub period
				PyString_AsString(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 14)),//fixing periodicity
				mapHolidays.at(PyString_AsString(PyList_GetItem(
				PyList_GetItem(legCharacteristics, i), 15))), //fixing calendar
				indexChars.at(PyString_AsString(PyList_GetItem(PyList_GetItem(
				legCharacteristics, i), 10))),  //interest rate index tenor
				PyFloat_AsDouble(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 18))		 //notional
				);

			//auto x = allCurves.at(PyString_AsString(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 21)));
			//auto xx = allCurves.at(PyString_AsString(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 22)));
			//auto y = mapManyFixings.at(PyString_AsString(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 10)));
			cout << "idLeg: " << numOp << endl;
			shared_ptr<QCInterestRatePayoff> tmpIntRatePayoff = shared_ptr<QCInterestRatePayoff>(
				new QCFloatingRatePayoff{ tmpIntRate,
				PyFloat_AsDouble(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 12)), 1.0,
				make_shared<QCInterestRateLeg>(tmpIntRateLeg),
				allCurves.at(PyString_AsString(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 21))),
				allCurves.at(PyString_AsString(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 22))),
				allValueDate,
				make_shared<map<QCDate, double>>(mapManyFixings.at(PyString_AsString(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 10)))) });

			payoffs.insert(pair<long, shared_ptr<QCInterestRatePayoff>>(static_cast<long>(numOp), tmpIntRatePayoff));
		}
		cout << "\tFinished constructing payoffs" << endl;

		//Calcular los valores presentes
		vector<tuple<long, double, double, vector<double>, vector<double>>> result;
		result.resize(payoffs.size());
		double m2m;
		double valueDateCashflow;
		vector<double> discDer;
		vector<double> projDer;
		unsigned long counter = 0;
		unsigned int longestDiscCurve = 0;
		unsigned int longestProjCurve = 0;
		for (const auto& payoff : payoffs)
		{
			m2m = payoff.second->presentValue();
			valueDateCashflow = payoff.second->getValueDateCashflow();
			unsigned int discVertices = payoff.second->discountCurveLength();
			if (discVertices > longestDiscCurve)
				longestDiscCurve = discVertices;
			discDer.resize(discVertices);
			for (unsigned long i = 0; i < discVertices; ++i)
			{
				discDer.at(i) = payoff.second->getPvRateDerivativeAt(i);
			}

			unsigned int projVertices = payoff.second->projectingCurveLength();
			if (projVertices > longestProjCurve)
				longestProjCurve = projVertices;
			projDer.resize(projVertices);
			for (unsigned long i = 0; i < projVertices; ++i)
			{
				projDer.at(i) = payoff.second->getPvProjRateDerivativeAt(i);
			}
			result.at(counter) = make_tuple(payoff.first, m2m, valueDateCashflow, discDer, projDer);
			++counter;
		}
		cout << "\tFinished present value and sensitivities" << endl;

		long cuantosResultados = PyList_Size(legCharacteristics);
		PyObject* legM2MAndDelta = PyTuple_New(cuantosResultados);
		for (long i = 0; i < cuantosResultados; ++i)
		{
			unsigned int discVertices = get<3>(result.at(i)).size();
			unsigned int projVertices = get<4>(result.at(i)).size();
			PyObject* temp = PyTuple_New(discVertices + projVertices + 4);
			int success;
			success = PyTuple_SetItem(temp, 0, PyInt_FromLong(get<0>(result.at(i))));
			success = PyTuple_SetItem(temp, 1, PyFloat_FromDouble(get<1>(result.at(i))));
			success = PyTuple_SetItem(temp, 2, PyFloat_FromDouble(get<2>(result.at(i))));
			success = PyTuple_SetItem(temp, 3, PyInt_FromLong(discVertices));
			for (unsigned int j = 0; j < discVertices; ++j)
			{
				success = PyTuple_SetItem(temp, j + 4, PyFloat_FromDouble(
					get<3>(result.at(i)).at(j) * BASIS_POINT));
			}
			for (unsigned int j = 0; j < projVertices; ++j)
			{
				success = PyTuple_SetItem(temp, j + 4 + discVertices, PyFloat_FromDouble(
					get<4>(result.at(i)).at(j) * BASIS_POINT));
			}
			success = PyTuple_SetItem(legM2MAndDelta, i, temp);
		}
		cout << "\tOutput prepared" << endl;

		return legM2MAndDelta;
	}
	catch (exception& e)
	{
		string msg = "Error en las patas flotantes. " + string(e.what()) + " numOp: " + to_string(numOp);
		PyErr_SetString(qcDveError, msg.c_str());
		return NULL;
	}
}

PyObject* pv_icp_clf_rate_legs(PyObject* self, PyObject*  args)
{
	char* fecha;
	PyObject* holidays;
	PyObject* curveValues;
	PyObject* curveCharacteristics;
	PyObject* legCharacteristics;
	PyObject* customAmort;
	PyObject* icpFixings;
	PyObject* ufFixings;

	if (!PyArg_ParseTuple(args,
		"sO!O!O!O!O!O!O!",
		&fecha,
		&PyList_Type, &holidays,
		&PyList_Type, &curveValues,
		&PyList_Type, &curveCharacteristics,
		&PyList_Type, &legCharacteristics,
		&PyList_Type, &customAmort,
		&PyList_Type, &icpFixings,
		&PyList_Type, &ufFixings))
	{
		return NULL;
	}
	double numOp;
	try
	{
		map<string, vector<QCDate>> mapHolidays;
		QCDvePyBindHelperFunctions::buildHolidays(holidays, mapHolidays);

		map<string, pair<vector<long>, vector<double>>> crvValues;
		QCDvePyBindHelperFunctions::buildCurveValues(curveValues, crvValues);

		map<string, QCDvePyBindHelperFunctions::string4> crvChars;
		QCDvePyBindHelperFunctions::buildCurveCharacteristics(curveCharacteristics, crvChars);

		map<string, shared_ptr<QCZeroCouponCurve>> allCurves;

		//Loopeo sobre los keys de crvValues
		for (const auto &curva : crvValues)
		{
			vector<long> tmpLng{ curva.second.first };
			vector<double> tmpDbl{ curva.second.second };

			string wf = get<1>(crvChars.at(curva.first));
			QCHelperFunctions::lowerCase(wf);

			string yf = get<2>(crvChars.at(curva.first));
			QCHelperFunctions::lowerCase(yf);

			shared_ptr<QCZeroCouponCurve> tmpCrv = QCFactoryFunctions::zrCpnCrvShrdPtr(
				tmpLng,
				tmpDbl,
				get<0>(crvChars.at(curva.first)),
				wf, yf, get<3>(crvChars.at(curva.first)));
			allCurves.insert(pair <string, shared_ptr<QCZeroCouponCurve>>(curva.first, tmpCrv));
		}

		//Guardaremos el list customAmort con los datos de amortizacion y nominal vigente
		//(en ese orden) en esta estructura.
		if (PyList_Size(customAmort) == 0)
		{
			string msg = "Error en las patas icp_clf. El vector de amortizaciones esta vacio";
			PyErr_SetString(qcDveError, msg.c_str());
			return NULL;
		}

		map<unsigned long, vector<tuple<QCDate, double, double>>> dateNotionalAndAmortByIdLeg;
		QCDvePyBindHelperFunctions::buildCustomAmortization(customAmort, dateNotionalAndAmortByIdLeg);

		//Metemos los fixings de ICP en esta estructura
		map<QCDate, double> mapIcpFixings;
		QCDvePyBindHelperFunctions::buildFixings(icpFixings, mapIcpFixings);

		//Metemos los fixings de UF en esta estructura
		map<QCDate, double> mapUfFixings;
		QCDvePyBindHelperFunctions::buildFixings(ufFixings, mapUfFixings);

		//Ahora hay que construir QCInterestRatePayoff para cada operacion
		//Se requiere:
		//	shared_ptr<QCInterestRate>
		//	shared_ptr<QCInterestRateLeg>
		//	QCDate (valueDate)
		//	shared_ptr<QCZeroCouponCurve> (la curva de proyeccion)
		//	shared_ptr<QCZeroCouponCurve> (la curva de descuento)
		//	shared_ptr<QCTimeSeries> (los fixings)

		//La info esta en la CellMatrix legCharacteristics:
		//0:	id_leg					long
		//1:	rec_pay					string
		//2:	start_date				QCDate
		//3:	end_date				QCDate
		//4:	settlement_calendar		string
		//5:	settlement_lag			int
		//6:	stub_period				string
		//7:	periodicity				string
		//8:	end_date_adjustment		string
		//9:	amortization			string
		//10:	interest_rate_index		string
		//11:	rate_value				double
		//12:	spread					double
		//13:	notional_currency		string
		//14:	notional				double
		//15:	wealth_factor			string
		//16:	year_fraction			string
		//17:	projecting_curve		string
		//18:	discount_curve			string
		//19:	fixing_calendar			string

		string strDate{ fecha };
		QCDate allValueDate{ strDate };

		map <long, shared_ptr<QCInterestRatePayoff>> payoffs;
		for (unsigned long i = 0; i < PyList_Size(legCharacteristics); ++i)
		{
			string wf = PyString_AsString(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 15));
			QCHelperFunctions::lowerCase(wf);
			string yf = PyString_AsString(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 16));
			QCHelperFunctions::lowerCase(yf);
			shared_ptr<QCInterestRate> tmpIntRate = QCFactoryFunctions::intRateSharedPtr(
				PyFloat_AsDouble(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 11)), yf, wf);

			numOp = PyFloat_AsDouble(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 0));
			cout << "icpclf numOp: " << numOp << endl;
			vector<tuple<QCDate, double, double>> amortIfCustom;
			if (PyString_AsString(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 9)) != "BULLET")
			{
				amortIfCustom = dateNotionalAndAmortByIdLeg.at(numOp);
			}
			QCInterestRateLeg tmpIntRateLeg = QCFactoryFunctions::buildIcpLeg2(
				PyString_AsString(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 1)),	//receive or pay
				QCDate{ string(PyString_AsString(PyList_GetItem(
				PyList_GetItem(legCharacteristics, i), 2))) },	//start date
				QCDate{ string(PyString_AsString(PyList_GetItem(
				PyList_GetItem(legCharacteristics, i), 3))) },	//end date
				mapHolidays.at(PyString_AsString(PyList_GetItem(
				PyList_GetItem(legCharacteristics, i), 4))),		//settlement calendar
				PyInt_AsLong(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 5)),		//settlement lag
				QCHelperFunctions::stringToQCStubPeriod(
				PyString_AsString(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 6))),//stub period
				PyString_AsString(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 7)),//periodicity
				QCHelperFunctions::stringToQCBusDayAdjRule(
				PyString_AsString(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 8))),//end date adjustment
				QCHelperFunctions::stringToQCAmortization(
				PyString_AsString(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 9))),//amortization
				amortIfCustom,										//amortization and notional by end date
				PyFloat_AsDouble(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 14))	//notional
				);
			shared_ptr<QCInterestRatePayoff> tmpIntRatePayoff = shared_ptr<QCInterestRatePayoff>(
				new QCIcpClfPayoff{ tmpIntRate, PyFloat_AsDouble(PyList_GetItem(
				PyList_GetItem(legCharacteristics, i), 12)), 1.0,
				make_shared<QCInterestRateLeg>(tmpIntRateLeg), allCurves.at(PyString_AsString(
				PyList_GetItem(PyList_GetItem(legCharacteristics, i), 17))),
				allCurves.at(PyString_AsString(PyList_GetItem(PyList_GetItem(
				legCharacteristics, i), 18))), allValueDate,
				make_shared<map<QCDate, double>>(mapIcpFixings),
				make_shared<map<QCDate, double>>(mapUfFixings) });
			payoffs.insert(pair<long, shared_ptr<QCInterestRatePayoff>>(static_cast<long>(numOp), tmpIntRatePayoff));
		}

		//Calcular los valores presentes
		vector<tuple<long, double, double, vector<double>, vector<double>>> result;
		result.resize(payoffs.size());
		double m2m;
		double valueDateCashflow;
		vector<double> discDer;
		vector<double> projDer;
		unsigned long counter = 0;
		for (const auto& payoff : payoffs)
		{
			m2m = payoff.second->presentValue();
			valueDateCashflow = payoff.second->getValueDateCashflow();
			unsigned int discVertices = payoff.second->discountCurveLength();
			discDer.resize(discVertices);
			for (unsigned long i = 0; i < discVertices; ++i)
			{
				discDer.at(i) = payoff.second->getPvRateDerivativeAt(i);
			}

			unsigned int projVertices = payoff.second->projectingCurveLength();
			cout << "proj. vertices 1: " << projVertices << endl;
			projDer.resize(projVertices);
			for (unsigned long i = 0; i < projVertices; ++i)
			{
				projDer.at(i) = payoff.second->getPvProjRateDerivativeAt(i);
			}
			result.at(counter) = make_tuple(payoff.first, m2m, valueDateCashflow, discDer, projDer);
			++counter;
		}

		long cuantosResultados = PyList_Size(legCharacteristics);
		PyObject* legM2MAndDelta = PyTuple_New(cuantosResultados);
		for (long i = 0; i < cuantosResultados; ++i)
		{
			unsigned int discVertices = get<3>(result.at(i)).size();
			unsigned int projVertices = get<4>(result.at(i)).size();
			cout << "proj. vertices 2: " << projVertices << endl;
			PyObject* temp = PyTuple_New(discVertices + projVertices + 4);
			int success;
			success = PyTuple_SetItem(temp, 0, PyInt_FromLong(get<0>(result.at(i))));
			success = PyTuple_SetItem(temp, 1, PyFloat_FromDouble(get<1>(result.at(i))));
			success = PyTuple_SetItem(temp, 2, PyFloat_FromDouble(get<2>(result.at(i))));
			success = PyTuple_SetItem(temp, 3, PyInt_FromLong(discVertices));
			for (unsigned int j = 0; j < discVertices; ++j)
			{
				success = PyTuple_SetItem(temp, j + 4, PyFloat_FromDouble(
					get<3>(result.at(i)).at(j) * BASIS_POINT));
			}
			for (unsigned int j = 0; j < projVertices; ++j)
			{
				success = PyTuple_SetItem(temp, j + 4 + discVertices, PyFloat_FromDouble(
					get<4>(result.at(i)).at(j) * BASIS_POINT));
			}
			success = PyTuple_SetItem(legM2MAndDelta, i, temp);
		}
		return legM2MAndDelta;
	}
	catch (exception& e)
	{
		string msg = "Error en las patas icp_clf. " + string(e.what()) + " numOp: " + to_string(numOp);
		PyErr_SetString(qcDveError, msg.c_str());
		return NULL;
	}
}

PyObject* pv_icp_clp_rate_legs(PyObject* self, PyObject*  args)
{
	char* fecha;
	PyObject* holidays;
	PyObject* curveValues;
	PyObject* curveCharacteristics;
	PyObject* legCharacteristics;
	PyObject* customAmort;
	PyObject* icpFixings;

	if (!PyArg_ParseTuple(args,
		"sO!O!O!O!O!O!",
		&fecha,
		&PyList_Type, &holidays,
		&PyList_Type, &curveValues,
		&PyList_Type, &curveCharacteristics,
		&PyList_Type, &legCharacteristics,
		&PyList_Type, &customAmort,
		&PyList_Type, &icpFixings))
	{
		return NULL;
	}
	long long numOp;
	try
	{
		map<string, vector<QCDate>> mapHolidays;
		QCDvePyBindHelperFunctions::buildHolidays(holidays, mapHolidays);

		map<string, pair<vector<long>, vector<double>>> crvValues;
		QCDvePyBindHelperFunctions::buildCurveValues(curveValues, crvValues);

		map<string, QCDvePyBindHelperFunctions::string4> crvChars;
		QCDvePyBindHelperFunctions::buildCurveCharacteristics(curveCharacteristics, crvChars);

		map<string, shared_ptr<QCZeroCouponCurve>> allCurves;

		//Loopeo sobre los keys de crvValues
		for (const auto &curva : crvValues)
		{
			vector<long> tmpLng{ curva.second.first };
			vector<double> tmpDbl{ curva.second.second };

			string wf = get<1>(crvChars.at(curva.first));
			QCHelperFunctions::lowerCase(wf);

			string yf = get<2>(crvChars.at(curva.first));
			QCHelperFunctions::lowerCase(yf);

			shared_ptr<QCZeroCouponCurve> tmpCrv = QCFactoryFunctions::zrCpnCrvShrdPtr(
				tmpLng,
				tmpDbl,
				get<0>(crvChars.at(curva.first)),
				wf, yf, get<3>(crvChars.at(curva.first)));
			allCurves.insert(pair <string, shared_ptr<QCZeroCouponCurve>>(curva.first, tmpCrv));
			cout << "Entro la curva: " << curva.first << endl;
		}

		//Guardaremos el list customAmort con los datos de amortizacion y nominal vigente
		//(en ese orden) en esta estructura.
		if (PyList_Size(customAmort) == 0)
		{
			string msg = "Error en las patas icp_clp. El vector de amortizaciones esta vacio";
			PyErr_SetString(qcDveError, msg.c_str());
			return NULL;
		}

		map<unsigned long, vector<tuple<QCDate, double, double>>> dateNotionalAndAmortByIdLeg;
		QCDvePyBindHelperFunctions::buildCustomAmortization(customAmort, dateNotionalAndAmortByIdLeg);

		//Metemos los fixings de ICP en esta estructura
		map<QCDate, double> mapIcpFixings;
		QCDvePyBindHelperFunctions::buildFixings(icpFixings, mapIcpFixings);

		//Ahora hay que construir QCInterestRatePayoff para cada operacion
		//Se requiere:
		//	shared_ptr<QCInterestRate>
		//	shared_ptr<QCInterestRateLeg>
		//	QCDate (valueDate)
		//	shared_ptr<QCZeroCouponCurve> (la curva de proyeccion)
		//	shared_ptr<QCZeroCouponCurve> (la curva de descuento)
		//	shared_ptr<QCTimeSeries> (los fixings)

		//La info esta en la CellMatrix legCharacteristics:
		//0:	id_leg					long
		//1:	rec_pay					string
		//2:	start_date				QCDate
		//3:	end_date				QCDate
		//4:	settlement_calendar		string
		//5:	settlement_lag			int
		//6:	stub_period				string
		//7:	periodicity				string
		//8:	end_date_adjustment		string
		//9:	amortization			string
		//10:	interest_rate_index		string
		//11:	rate_value				double
		//12:	spread					double
		//13:	notional_currency		string
		//14:	notional				double
		//15:	wealth_factor			string
		//16:	year_fraction			string
		//17:	projecting_curve		string
		//18:	discount_curve			string
		//19:	fixing_calendar			string

		string strDate{ fecha };
		QCDate allValueDate{ strDate };

		map <long, shared_ptr<QCInterestRatePayoff>> payoffs;
		for (unsigned long i = 0; i < PyList_Size(legCharacteristics); ++i)
		{
			string wf = PyString_AsString(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 15));
			QCHelperFunctions::lowerCase(wf);
			string yf = PyString_AsString(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 16));
			QCHelperFunctions::lowerCase(yf);
			shared_ptr<QCInterestRate> tmpIntRate = QCFactoryFunctions::intRateSharedPtr(
				PyFloat_AsDouble(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 11)), yf, wf);

			numOp = (long long)PyFloat_AsDouble(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 0));
			cout << "numOp: " << numOp << endl;
			vector<tuple<QCDate, double, double>> amortIfCustom;
			if (string(PyString_AsString(PyList_GetItem(
				PyList_GetItem(legCharacteristics, i), 9))) != "BULLET")
			{
				amortIfCustom = dateNotionalAndAmortByIdLeg.at(numOp);
			}
			QCInterestRateLeg tmpIntRateLeg = QCFactoryFunctions::buildIcpLeg2(
				PyString_AsString(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 1)),	//receive or pay
				QCDate{ string(PyString_AsString(PyList_GetItem(
				PyList_GetItem(legCharacteristics, i), 2))) },	//start date
				QCDate{ string(PyString_AsString(PyList_GetItem(
				PyList_GetItem(legCharacteristics, i), 3))) },	//end date
				mapHolidays.at(PyString_AsString(PyList_GetItem(
				PyList_GetItem(legCharacteristics, i), 4))),		//settlement calendar
				PyInt_AsLong(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 5)),		//settlement lag
				QCHelperFunctions::stringToQCStubPeriod(
				PyString_AsString(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 6))),//stub period
				PyString_AsString(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 7)),//periodicity
				QCHelperFunctions::stringToQCBusDayAdjRule(
				PyString_AsString(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 8))),//end date adjustment
				QCHelperFunctions::stringToQCAmortization(
				PyString_AsString(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 9))),//amortization
				amortIfCustom,										//amortization and notional by end date
				PyFloat_AsDouble(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 14))	//notional
				);
			cout << "notional: " << PyFloat_AsDouble(PyList_GetItem(PyList_GetItem(
				legCharacteristics, i), 14));
			cout << "buildIcpLegOk" << endl;
			cout << "curva1: " << PyString_AsString(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 17))
				<< endl;
			cout << "curva2: " << PyString_AsString(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 18))
				<< endl;
			cout << "Alta payoff: " << numOp << endl;
			shared_ptr<QCInterestRatePayoff> tmpIntRatePayoff = shared_ptr<QCInterestRatePayoff>(
					new QCIcpClpPayoff{ tmpIntRate, PyFloat_AsDouble(PyList_GetItem(
					PyList_GetItem(legCharacteristics, i), 12)), 1.0,
					make_shared<QCInterestRateLeg>(tmpIntRateLeg),
					allCurves.at(PyString_AsString(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 17))),
					allCurves.at(PyString_AsString(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 18))),
					allValueDate,
					make_shared<map<QCDate, double>>(mapIcpFixings) });
			cout << "payoff ok" << endl;
			payoffs.insert(pair<long, shared_ptr<QCInterestRatePayoff>>(static_cast<long>(numOp), tmpIntRatePayoff));
		}

		//Calcular los valores presentes
		vector<tuple<long, double, double, vector<double>, vector<double>>> result;
		result.resize(payoffs.size());
		double m2m;
		double valueDateCashflow;
		vector<double> discDer;
		vector<double> projDer;
		unsigned long counter = 0;
		for (const auto& payoff : payoffs)
		{
			m2m = payoff.second->presentValue();
			valueDateCashflow = payoff.second->getValueDateCashflow();
			unsigned int discVertices = payoff.second->discountCurveLength();
			discDer.resize(discVertices);
			for (unsigned long i = 0; i < discVertices; ++i)
			{
				discDer.at(i) = payoff.second->getPvRateDerivativeAt(i);
			}

			unsigned int projVertices = payoff.second->projectingCurveLength();
			projDer.resize(projVertices);
			for (unsigned long i = 0; i < projVertices; ++i)
			{
				projDer.at(i) = payoff.second->getPvProjRateDerivativeAt(i);
			}
			result.at(counter) = make_tuple(payoff.first, m2m, valueDateCashflow, discDer, projDer);
			++counter;
		}

		long cuantosResultados = PyList_Size(legCharacteristics);
		PyObject* legM2MAndDelta = PyTuple_New(cuantosResultados);
		for (long i = 0; i < cuantosResultados; ++i)
		{
			unsigned int discVertices = get<3>(result.at(i)).size();
			unsigned int projVertices = get<4>(result.at(i)).size();
			PyObject* temp = PyTuple_New(discVertices + projVertices + 4);
			int success;
			success = PyTuple_SetItem(temp, 0, PyInt_FromLong(get<0>(result.at(i))));
			success = PyTuple_SetItem(temp, 1, PyFloat_FromDouble(get<1>(result.at(i))));
			success = PyTuple_SetItem(temp, 2, PyFloat_FromDouble(get<2>(result.at(i))));
			success = PyTuple_SetItem(temp, 3, PyInt_FromLong(discVertices));
			for (unsigned int j = 0; j < discVertices; ++j)
			{
				success = PyTuple_SetItem(temp, j + 4, PyFloat_FromDouble(
					get<3>(result.at(i)).at(j) * BASIS_POINT));
			}
			for (unsigned int j = 0; j < projVertices; ++j)
			{
				success = PyTuple_SetItem(temp, j + 4 + discVertices, PyFloat_FromDouble(
					get<4>(result.at(i)).at(j) * BASIS_POINT));
			}
			success = PyTuple_SetItem(legM2MAndDelta, i, temp);
		}
		return legM2MAndDelta;
	}
	catch (exception& e)
	{
		string msg = "Error en las patas icp_clp. " + string(e.what()) + " numOp: " + to_string(numOp);
		PyErr_SetString(qcDveError, msg.c_str());
		return NULL;
	}

}

PyObject* pv_discount_bond_legs(PyObject* self, PyObject*  args)
{
	char* fecha;
	PyObject* holidays;
	PyObject* curveValues;
	PyObject* curveCharacteristics;
	PyObject* legCharacteristics;
	PyObject* fxFixings;
	PyObject* typeFxRate;

	if (!PyArg_ParseTuple(args,
		"sO!O!O!O!O!O!",
		&fecha,
		&PyList_Type, &holidays,
		&PyList_Type, &curveValues,
		&PyList_Type, &curveCharacteristics,
		&PyList_Type, &legCharacteristics,
		&PyList_Type, &fxFixings,
		&PyList_Type, &typeFxRate))
	{
		cout << "ERROR" << endl;
		return NULL;
	}
	long long numOp;
	cout << "Enter discount_bond_legs" << endl;
	try
	{
		map<string, vector<QCDate>> mapHolidays;
		QCDvePyBindHelperFunctions::buildHolidays(holidays, mapHolidays);

		map<string, pair<vector<long>, vector<double>>> crvValues;
		QCDvePyBindHelperFunctions::buildCurveValues(curveValues, crvValues);

		map<string, QCDvePyBindHelperFunctions::string4> crvChars;
		QCDvePyBindHelperFunctions::buildCurveCharacteristics(curveCharacteristics, crvChars);

		map<string, string> fxRateChars;
		QCDvePyBindHelperFunctions::buildFxRateIndexFxRate(typeFxRate, fxRateChars);
		for (auto& x : fxRateChars)
		{
			cout << "\tÍndice: " << x.first << ", " << x.second << endl;
		}
		map<string, shared_ptr<QCZeroCouponCurve>> allCurves;

		//Loopeo sobre los keys de crvValues
		for (const auto &curva : crvValues)
		{
			vector<long> tmpLng{ curva.second.first };
			vector<double> tmpDbl{ curva.second.second };

			string wf = get<1>(crvChars.at(curva.first));
			QCHelperFunctions::lowerCase(wf);

			string yf = get<2>(crvChars.at(curva.first));
			QCHelperFunctions::lowerCase(yf);

			shared_ptr<QCZeroCouponCurve> tmpCrv = QCFactoryFunctions::zrCpnCrvShrdPtr(
				tmpLng,
				tmpDbl,
				get<0>(crvChars.at(curva.first)),
				wf, yf, get<3>(crvChars.at(curva.first)));
			allCurves.insert(pair <string, shared_ptr<QCZeroCouponCurve>>(curva.first, tmpCrv));
			cout << "\tIngresó la curva: " << curva.first << endl;
		}

		//Metemos los fixings de los índices fx en esta estructura
		map<string, map<QCDate, double>> mapManyFixings;
		QCDvePyBindHelperFunctions::buildManyFixings(fxFixings, mapManyFixings);
		cout << "\tFinished Fx Fixings" << endl;

		//Ahora hay que construir QCInterestRatePayoff para cada operacion
		//Se requiere:
		//	shared_ptr<QCInterestRate>
		//	shared_ptr<QCInterestRateLeg>
		//	QCDate (valueDate)
		//	shared_ptr<QCZeroCouponCurve> (la curva de descuento)
		//	shared_ptr<QCTimeSeries> (los fixings)

		//La info esta en la CellMatrix legCharacteristics:
		//0:	id_leg						long
		//1:	rec_pay						string
		//2:	start_date					QCDate
		//3:	end_date					QCDate
		//4:	fx_rate						string
		//5:	settlement_lag				int
		//6:	settlement_mechanism		string
		//7:	settlement_currency			string
		//8:	settlement_fx_rate_index	string
		//9:	fixing_calendar				string
		//10:	fixing_lag					integer
		//11:	settlement_calendar			string
		//12:	notional_currency			string
		//13:	notional					double
		//14:	discount_curve				string

		string strDate{ fecha };
		QCDate allValueDate{ strDate };

		map<long, shared_ptr<QCInterestRatePayoff>> payoffs;
		QCCurrencyConverter crrncyCnvrtr;

		for (unsigned long i = 0; i < PyList_Size(legCharacteristics); ++i)
		{
			numOp = (long long)PyFloat_AsDouble(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 0));
			cout << "\tnumOp: " << numOp << endl;
			QCInterestRateLeg tmpIntRateLeg = QCFactoryFunctions::buildDiscountBondLeg(
				PyString_AsString(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 1)),	//receive or pay
				QCDate{ string(PyString_AsString(PyList_GetItem(
				PyList_GetItem(legCharacteristics, i), 2))) },	//start date
				QCDate{ string(PyString_AsString(PyList_GetItem(
				PyList_GetItem(legCharacteristics, i), 3))) },	//end date
				PyInt_AsLong(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 5)),		//settlement lag
				PyInt_AsLong(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 10)),	//fixing lag
				mapHolidays.at(PyString_AsString(PyList_GetItem(
				PyList_GetItem(legCharacteristics, i), 9))),		//fixing calendar
				mapHolidays.at(PyString_AsString(PyList_GetItem(
				PyList_GetItem(legCharacteristics, i), 11))),		//settlement calendar
				PyFloat_AsDouble(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 13))	//notional
				);
			cout << "\tbuildDiscountBondLegOk" << endl;
			cout << "\tfx rate code: " << PyString_AsString(PyList_GetItem(
				PyList_GetItem(legCharacteristics, i), 8)) << endl;;
			cout << "\tcurve: " << PyString_AsString(PyList_GetItem(PyList_GetItem(
				legCharacteristics, i), 14)) << endl;
			cout << crrncyCnvrtr.getFxRateEnum(fxRateChars.at(PyString_AsString(PyList_GetItem(
				PyList_GetItem(legCharacteristics, i), 8)))) << endl;

			shared_ptr<QCInterestRatePayoff> tmpIntRatePayoff = shared_ptr<QCInterestRatePayoff>(
				new QCDiscountBondPayoff{ make_shared<QCInterestRateLeg>(tmpIntRateLeg),
				allCurves.at(PyString_AsString(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 14))),
				allValueDate,
				make_shared<map<QCDate, double>>(mapManyFixings.at(
				PyString_AsString(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 8)))),
				crrncyCnvrtr.getCurrencyEnum(PyString_AsString(PyList_GetItem(
				PyList_GetItem(legCharacteristics, i), 12))),
				crrncyCnvrtr.getFxRateEnum(fxRateChars.at(PyString_AsString(PyList_GetItem(
				PyList_GetItem(legCharacteristics, i), 8)))) }
			);
			cout << "\tpayoff ok" << endl;
			payoffs.insert(pair<long, shared_ptr<QCInterestRatePayoff>>(static_cast<long>(numOp), tmpIntRatePayoff));
		}

		cout << "\tFinished constructing payoffs" << endl;

		//Calcular los valores presentes
		vector<tuple<long, double, double, vector<double>>> result;
		result.resize(payoffs.size());
		double m2m;
		double valueDateCashflow;
		vector<double> der;
		unsigned long counter = 0;
		unsigned int longestCurve = 0;
		for (const auto& payoff : payoffs)
		{
			const double zero = 0;
			m2m = payoff.second->presentValue();
			valueDateCashflow = payoff.second->getValueDateCashflow();
			if (m2m == zero)
			{
				m2m = valueDateCashflow;
			}
			unsigned int vertices = payoff.second->discountCurveLength();
			der.resize(vertices);
			for (unsigned long i = 0; i < vertices; ++i)
			{
				der.at(i) = payoff.second->getPvRateDerivativeAt(i);
			}
			result.at(counter) = make_tuple(payoff.first, m2m, valueDateCashflow, der);
			der.clear();
			++counter;
		}
		cout << "\tFinished present value and sensitivities" << endl;
		cout << "result.size() " << result.size() << endl;

		long cuantosM2M = PyList_Size(legCharacteristics);
		PyObject* legM2M = PyList_New(cuantosM2M);
		for (long i = 0; i < cuantosM2M; ++i)
		{
			unsigned int vertices = get<3>(result.at(i)).size();
			PyObject* temp = PyList_New(vertices + 3);
			int success;
			success = PyList_SetItem(temp, 0, PyInt_FromLong(get<0>(result.at(i))));
			success = PyList_SetItem(temp, 1, PyFloat_FromDouble(get<1>(result.at(i))));
			success = PyList_SetItem(temp, 2, PyFloat_FromDouble(get<2>(result.at(i))));
			for (unsigned int j = 0; j < vertices; ++j)
			{
				success = PyList_SetItem(temp, j + 3, PyFloat_FromDouble(get<3>(result.at(i)).at(j) * BASIS_POINT));
			}
			success = PyList_SetItem(legM2M, i, temp);
		}
		cout << "\tOutput prepared" << endl;

		return legM2M;
	}
	catch (exception& e)
	{
		string msg = "Error en las patas a descuento. " + string(e.what()) + " numOp: " + to_string(numOp);
		PyErr_SetString(qcDveError, msg.c_str());
		return NULL;
	}
}

PyObject* next_business_date(PyObject* self, PyObject*  args)
{
	cout << "Enter next_business_date" << endl;

	char* fecha;
	PyObject* holidays;

	if (!PyArg_ParseTuple(args, "sO!", &fecha, &PyList_Type, &holidays))
	{
		cout << "\tnext_business_date: argument error";
		return NULL;
	}

	//Se construye la fecha de proceso
	string strDate{ fecha };
	QCDate valueDate{ strDate };
	cout << "\tnext_business_date: finished constructing date";

	//Se construye el vector con las fechas de los feriados
	vector<QCDate> dateVector;
	QCDvePyBindHelperFunctions::buildQCDateVector(holidays, dateVector);
	cout << "\tnext_business_date: finished holidays" << endl;

	QCDate result = valueDate.addDays(1).businessDay(dateVector, QCDate::QCBusDayAdjRules::qcFollow);
	cout << "\tnext_business_date: finished calculation" << endl;

	return Py_BuildValue("s", result.description().c_str());
}

//Tres funciones de prueba
PyObject* qc_date_2_string(PyObject* self, PyObject*  args)
{
	long fecha;
	if (!PyArg_ParseTuple(args, "i", &fecha))
	{
		return NULL;
	}
	
	QCDate qcFecha{ fecha };
	std::string result = qcFecha.description();

	return Py_BuildValue("s", result.c_str());
}

PyObject * hello_world_nombre(PyObject * self, PyObject * args)
{
	char* nombre;
	if (!PyArg_ParseTuple(args, "s", &nombre))
	{
		return NULL;
	}

	//no se porque tengo que definir nombre como puntero...

	return Py_BuildValue("s", nombre);
}

PyObject * hello_world(PyObject * self, PyObject * args)
{
	return Py_BuildValue("s", "Hello World!");
}

//TEST LIST
PyObject* test_uso_list_r_int(PyObject* self, PyObject* args)
{
	PyObject * list;
	int aux = 0;

	if (!PyArg_ParseTuple(args, "O!", &PyList_Type, &list)) return NULL;

	for (int i = 0; i < PyList_Size(list); i++)
	{
		aux += PyInt_AsLong(PyList_GetItem(list, i));

	}

	return Py_BuildValue("i", aux);
}

PyObject* test_uso_dos_list_int(PyObject* self, PyObject* args)
{
	PyObject* holidays;
	int aux = 0;

	if (!PyArg_ParseTuple(args, "O!", &PyList_Type, &holidays)) return NULL;

	map<string, vector<QCDate>> mapHolidays;
	QCDvePyBindHelperFunctions::buildHolidays(holidays, mapHolidays);

	/*for (int i = 0; i < PyList_Size(list1); ++i)
	{
		aux += PyInt_AsLong(PyList_GetItem(list1, i)) + PyInt_AsLong(PyList_GetItem(list2, i));

	}*/

	return Py_BuildValue("i", aux);
}

//Toma lista de enteros y retorna la lista de enteros mas 1
PyObject* test_uso_list_r_list(PyObject* self, PyObject* args)
{
	PyObject* list;
	PyObject* result = PyList_New(0);
	PyObject* objeto_append;
	long aux;

	if (!PyArg_ParseTuple(args, "O!", &PyList_Type, &list)) return NULL;

	for (int i = 0; i < PyList_Size(list); i++)
	{
		aux = PyInt_AsLong(PyList_GetItem(list, i)) + 1;
		objeto_append = PyInt_FromLong(aux);
		PyList_Append(result, objeto_append);
	}

	return result;
}

//Toma Lisla de listas de int (una matriz) y retorna la suma de sus elementos
PyObject * test_uso_tuples_r_int(PyObject * self, PyObject * args)
{
	PyObject * list;
	PyObject * list_in_list;
	int aux = 0;

	if (!PyArg_ParseTuple(args, "O!", &PyList_Type, &list)) return NULL;

	for (int i = 0; i < PyList_Size(list); i++)
	{
		//aux += PyInt_AsLong(PyList_GetItem(list, i));
		list_in_list = PyList_GetItem(list, i);
		for (int j = 0; j < PyList_Size(list_in_list); j++)
		{
			aux += PyInt_AsLong(PyList_GetItem(list_in_list, j));
		}

	}

	return Py_BuildValue("i", aux);
}

// toma una matriz de enteros y retorna la misma matriz mas uno
PyObject* test_uso_tuples_r_list_in_list(PyObject* self, PyObject* args)
{
	PyObject* list;
	PyObject* list_in_list;
	PyObject* result = PyList_New(0);
	PyObject* preresult;
	long aux = 0;

	if (!PyArg_ParseTuple(args, "O!", &PyList_Type, &list)) return NULL;

	for (int i = 0; i < PyList_Size(list); i++)
	{
		preresult = PyList_New(0);

		list_in_list = PyList_GetItem(list, i);
		for (int j = 0; j < PyList_Size(list_in_list); j++)
		{
			aux = PyInt_AsLong(PyList_GetItem(list_in_list, j)) + 1;
			PyList_Append(preresult, PyInt_FromLong(aux));
		}

		PyList_Append(result, preresult);

	}

	return result;
}

//tomo lista de listas de int y string y retorna la matriz en 
//donde los valores int estan mas 1 y el string se concatena a CONCATENADO 
PyObject * test_uso_tuples_r_lil_string_int(PyObject * self, PyObject * args)
{
	PyObject * list;
	PyObject * list_in_list;
	PyObject * result = PyList_New(0);
	PyObject * preresult;
	long aux = 0;
	char label[100];

	if (!PyArg_ParseTuple(args, "O!", &PyList_Type, &list)) return NULL;

	for (int i = 0; i < PyList_Size(list); i++)
	{
		preresult = PyList_New(0);
		list_in_list = PyList_GetItem(list, i);

		aux = PyInt_AsLong(PyList_GetItem(list_in_list, 0)) + 1;

		std::string auxlabel{ PyString_AsString(PyList_GetItem(list_in_list, 1)) };
		auxlabel += " CONCATENADO ";
		PyList_Append(preresult, PyInt_FromLong(aux));
		PyList_Append(preresult, PyString_FromString(auxlabel.c_str()));

		PyList_Append(result, preresult);

	}

	return result;
}

//PyMethodDef es un arreglo de las funciones 
// que que serán empaquetadas en el modulo de Python
PyMethodDef QC_DVE_Methods[] =
{
	//Primer argumento:		nombre con que se llamara a la función en python.
	//Segundo argumento:	funcion que se llamará (se castea a funcion de python)
	//tercer argumento:		flag que le dice al interprete la convencion utilizada por la funcion C
	//						 - METH_VARARGS: la funcion espera parámetros Python
	//						 - METH_KEYWORDS: The function expects three parameters: self, args, and a 
	//						   dictionary of all the keyword arguments. 
	//						   The flag is typically combined with METH_VARARGS,
	//						   and the parameters are typically processed using 
	//						   PyArg_ParseTupleAndKeywords()  
	//						 - 0: means that an obsolete variant of PyArg_ParseTuple() is used.
	{ "add_two_arguments", (PyCFunction)some_function, METH_VARARGS, 0 },
	{ "hello_world", (PyCFunction)hello_world, 0, 0 },
	{ "hello_world_nombre", (PyCFunction)hello_world_nombre, METH_VARARGS, 0 },
	{ "qc_date_to_string", (PyCFunction)qc_date_2_string, METH_VARARGS, 0 },
	{ "test_uso_dos_list_int", (PyCFunction)test_uso_dos_list_int, METH_VARARGS, 0 },
	{ "test_uso_list_r_int", (PyCFunction)test_uso_list_r_int, METH_VARARGS, 0 },
	{ "test_uso_list_r_list", (PyCFunction)test_uso_list_r_list, METH_VARARGS, 0 },
	{ "test_uso_tuples_r_int", (PyCFunction)test_uso_tuples_r_int, METH_VARARGS, 0 },
	{ "test_uso_tuples_r_list_in_list", (PyCFunction)test_uso_tuples_r_list_in_list, METH_VARARGS, 0 },
	{ "test_uso_tuples_r_lil_string_int", (PyCFunction)test_uso_tuples_r_lil_string_int, METH_VARARGS, 0 },
	{ "pv_fixed_rate_legs", (PyCFunction)pv_fixed_rate_legs, METH_VARARGS, 0 },
	{ "pv_floating_rate_legs", (PyCFunction)pv_floating_rate_legs, METH_VARARGS, 0 },
	{ "pv_icp_clf_rate_legs", (PyCFunction)pv_icp_clf_rate_legs, METH_VARARGS, 0 },
	{ "pv_icp_clp_rate_legs", (PyCFunction)pv_icp_clp_rate_legs, METH_VARARGS, 0 },
	{ "pv_discount_bond_legs", (PyCFunction)pv_discount_bond_legs, METH_VARARGS, 0 },
	{ "cashflow_fixed_mortgages", (PyCFunction)cashflow_fixed_mortgages, METH_VARARGS, 0 },
	{ "cashflow_fixed_swaps", (PyCFunction)cashflow_fixed_swaps, METH_VARARGS, 0 },
	{ "cashflow_icp_clp_rate_legs", (PyCFunction)cashflow_icp_clp_rate_legs, METH_VARARGS, 0 },
	{ "cashflow_floating_rate_legs", (PyCFunction)cashflow_floating_rate_legs, METH_VARARGS, 0 },
	{ "boot_zero_rates_fixed_legs", (PyCFunction)boot_zero_rates_fixed_legs, METH_VARARGS, 0 },
	{ "next_business_date", (PyCFunction)next_business_date, METH_VARARGS, 0 },
	{ "boot_zero_rates_fwds_fixed_legs", (PyCFunction)boot_zero_rates_fwds_fixed_legs, METH_VARARGS, 0 },
	{ "boot_zero_rates_fwds_floating_legs", (PyCFunction)boot_zero_rates_fwds_floating_legs, METH_VARARGS, 0 },
	{ "boot_zero_rates_zero_coupon_spreads", (PyCFunction)boot_zero_rates_zero_coupon_spreads, METH_VARARGS, 0 },
	{ "boot_zero_rates_floating_legs_proj_curve", (PyCFunction)boot_zero_rates_floating_legs_proj_curve, METH_VARARGS, 0 },
	{ 0, 0, 0, 0 }
};

//funcion __init__ python, tecnicamente acá estamos construyendo el modulo pyd 
PyMODINIT_FUNC
initQC_DVE_PYBIND(void)
{
	PyObject *m;

	m = Py_InitModule("QC_DVE_PYBIND", QC_DVE_Methods);
	if (m == NULL)
		return;

	qcDveError = PyErr_NewException("qcDve.error", NULL, NULL);
	Py_INCREF(qcDveError);
	PyModule_AddObject(m, "error", qcDveError);
}