#include <exception>
#include <memory>
#include <string>
#include <map>

#include "include.h"
#include "QCDate.h"
#include "QCDvePyBindHelperFunctions.h"
#include "QCZeroCouponCurve.h"
#include "QCHelperFunctions.h"
#include "QCFactoryFunctions.h"
#include "QCFixedRatePayoff.h"
#include "QCFloatingRatePayoff.h"
#include "QCIcpClfPayoff.h"
#include "QCIcpClpPayoff.h"

using namespace std;
#define BASIS_POINT .0001

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
PyObject* pv_fixed_rate_legs(PyObject* self, PyObject*  args)
{
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
	map<unsigned long, vector<tuple<QCDate, double, double>>> dateNotionalAndAmortByIdLeg;
	QCDvePyBindHelperFunctions::buildCustomAmortization(customAmort, dateNotionalAndAmortByIdLeg);

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

		long numOp = PyInt_AsLong(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 0));
		vector<tuple<QCDate, double, double>> amortIfCustom;
		if (string(PyString_AsString(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 9))) != "BULLET")
		{
			amortIfCustom = dateNotionalAndAmortByIdLeg.at(numOp);
		}
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
		shared_ptr<QCInterestRatePayoff> tmpIntRatePayoff = shared_ptr<QCInterestRatePayoff>(
			new QCFixedRatePayoff{ tmpIntRate, make_shared<QCInterestRateLeg>(tmpIntRateLeg),
			allCurves.at(string(PyString_AsString(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 14)))),
			allValueDate, nullptr });
		payoffs.insert(pair<long, shared_ptr<QCInterestRatePayoff>>(numOp,tmpIntRatePayoff));
	}

	//Calcular los valores presentes
	vector<tuple<long, double, vector<double>>> result;
	result.resize(payoffs.size());
	double m2m;
	vector<double> der;
	unsigned long counter = 0;
	unsigned int longestCurve = 0;
	for (const auto& payoff : payoffs)
	{
		m2m = payoff.second->presentValue();
		unsigned int vertices = payoff.second->discountCurveLength();
		der.resize(vertices);
		for (unsigned long i = 0; i < vertices; ++i)
		{
			der.at(i) = payoff.second->getPvRateDerivativeAt(i);
		}
		result.at(counter) = make_tuple(payoff.first, m2m, der);
		der.clear();
		++counter;
	}
	
	long cuantosM2M = PyList_Size(legCharacteristics);
	PyObject* legM2M = PyList_New(cuantosM2M);
	for (long i = 0; i < cuantosM2M; ++i)
	{
		unsigned int vertices = get<2>(result.at(i)).size();
		PyObject* temp = PyList_New(vertices + 2);
		int success;
		success = PyList_SetItem(temp, 0, PyInt_FromLong(get<0>(result.at(i))));
		success = PyList_SetItem(temp, 1, PyFloat_FromDouble(get<1>(result.at(i))));
		for (unsigned int j = 0; j < vertices; ++j)
		{
			success = PyList_SetItem(temp, j + 2, PyFloat_FromDouble(get<2>(result.at(i)).at(j) * BASIS_POINT));
		}
		success = PyList_SetItem(legM2M, i, temp);
	}

	return legM2M;
	//return Py_BuildValue("f", get<2>(dateNotionalAndAmortByIdLeg.at(35819).at(1)));
}

PyObject* pv_floating_rate_legs(PyObject* self, PyObject*  args)
{
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
	map<unsigned long, vector<tuple<QCDate, double, double>>> dateNotionalAndAmortByIdLeg;
	QCDvePyBindHelperFunctions::buildCustomAmortization(customAmort, dateNotionalAndAmortByIdLeg);

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
	
	map <long, shared_ptr<QCInterestRatePayoff>> payoffs;
	for (unsigned long i = 0; i < PyList_Size(legCharacteristics); ++i)
	{
		string wf = PyString_AsString(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 19));
		QCHelperFunctions::lowerCase(wf);
		string yf = PyString_AsString(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 20));
		QCHelperFunctions::lowerCase(yf);
		
		shared_ptr<QCInterestRate> tmpIntRate = QCFactoryFunctions::intRateSharedPtr(
			PyFloat_AsDouble(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 11)), yf, wf);
		
		long numOp = PyInt_AsLong(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 0));
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
		
		shared_ptr<QCInterestRatePayoff> tmpIntRatePayoff = shared_ptr<QCInterestRatePayoff>(
			new QCFloatingRatePayoff{ tmpIntRate,
			PyFloat_AsDouble(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 12)), 1.0,
			make_shared<QCInterestRateLeg>(tmpIntRateLeg),
			allCurves.at(PyString_AsString(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 21))),
			allCurves.at(PyString_AsString(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 22))),
			allValueDate,
			make_shared<map<QCDate, double>>(mapManyFixings.at(PyString_AsString(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 10)))) });

		payoffs.insert(pair<long, shared_ptr<QCInterestRatePayoff>>(numOp, tmpIntRatePayoff));
	}

	//Calcular los valores presentes
	vector<tuple<long, double, vector<double>, vector<double>>> result;
	result.resize(payoffs.size());
	double m2m;
	vector<double> discDer;
	vector<double> projDer;
	unsigned long counter = 0;
	unsigned int longestDiscCurve = 0;
	unsigned int longestProjCurve = 0;
	for (const auto& payoff : payoffs)
	{
		m2m = payoff.second->presentValue();
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
		result.at(counter) = make_tuple(payoff.first, m2m, discDer, projDer);
		++counter;
	}

	long cuantosResultados = PyList_Size(legCharacteristics);
	PyObject* legM2MAndDelta = PyTuple_New(cuantosResultados);
	for (long i = 0; i < cuantosResultados; ++i)
	{
		unsigned int discVertices = get<2>(result.at(i)).size();
		unsigned int projVertices = get<3>(result.at(i)).size();
		PyObject* temp = PyTuple_New(discVertices + projVertices + 3);
		int success;
		success = PyTuple_SetItem(temp, 0, PyInt_FromLong(get<0>(result.at(i))));
		success = PyTuple_SetItem(temp, 1, PyFloat_FromDouble(get<1>(result.at(i))));
		success = PyTuple_SetItem(temp, 2, PyInt_FromLong(discVertices));
		for (unsigned int j = 0; j < discVertices; ++j)
		{
			success = PyTuple_SetItem(temp, j + 3, PyFloat_FromDouble(
				get<2>(result.at(i)).at(j) * BASIS_POINT));
		}
		for (unsigned int j = 0; j < projVertices; ++j)
		{
			success = PyTuple_SetItem(temp, j + 3 + discVertices, PyFloat_FromDouble(
				get<3>(result.at(i)).at(j) * BASIS_POINT));
		}
		success = PyTuple_SetItem(legM2MAndDelta, i, temp);
	}
	return legM2MAndDelta;
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

		long numOp = PyInt_AsLong(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 0));
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
		payoffs.insert(pair<long, shared_ptr<QCInterestRatePayoff>>(numOp, tmpIntRatePayoff));
	}

	//Calcular los valores presentes
	vector<tuple<long, double, vector<double>, vector<double>>> result;
	result.resize(payoffs.size());
	double m2m;
	vector<double> discDer;
	vector<double> projDer;
	unsigned long counter = 0;
	unsigned int longestDiscCurve = 0;
	unsigned int longestProjCurve = 0;
	for (const auto& payoff : payoffs)
	{
		m2m = payoff.second->presentValue();
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
		result.at(counter) = make_tuple(payoff.first, m2m, discDer, projDer);
		++counter;
	}

	long cuantosResultados = PyList_Size(legCharacteristics);
	PyObject* legM2MAndDelta = PyTuple_New(cuantosResultados);
	for (long i = 0; i < cuantosResultados; ++i)
	{
		unsigned int discVertices = get<2>(result.at(i)).size();
		unsigned int projVertices = get<3>(result.at(i)).size();
		PyObject* temp = PyTuple_New(discVertices + projVertices + 3);
		int success;
		success = PyTuple_SetItem(temp, 0, PyInt_FromLong(get<0>(result.at(i))));
		success = PyTuple_SetItem(temp, 1, PyFloat_FromDouble(get<1>(result.at(i))));
		success = PyTuple_SetItem(temp, 2, PyInt_FromLong(discVertices));
		for (unsigned int j = 0; j < discVertices; ++j)
		{
			success = PyTuple_SetItem(temp, j + 3, PyFloat_FromDouble(
				get<2>(result.at(i)).at(j) * BASIS_POINT));
		}
		for (unsigned int j = 0; j < projVertices; ++j)
		{
			success = PyTuple_SetItem(temp, j + 3 + discVertices, PyFloat_FromDouble(
				get<3>(result.at(i)).at(j) * BASIS_POINT));
		}
		success = PyTuple_SetItem(legM2MAndDelta, i, temp);
	}
	return legM2MAndDelta;
	
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

		long numOp = PyInt_AsLong(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 0));
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
			new QCIcpClpPayoff{ tmpIntRate, PyFloat_AsDouble(PyList_GetItem(
			PyList_GetItem(legCharacteristics, i), 12)), 1.0,
			make_shared<QCInterestRateLeg>(tmpIntRateLeg),
			allCurves.at(PyString_AsString(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 17))),
			allCurves.at(PyString_AsString(PyList_GetItem(PyList_GetItem(legCharacteristics, i), 18))),
			allValueDate,
			make_shared<map<QCDate, double>>(mapIcpFixings) });
		payoffs.insert(pair<long, shared_ptr<QCInterestRatePayoff>>(numOp, tmpIntRatePayoff));
	}

	//Calcular los valores presentes
	vector<tuple<long, double, vector<double>, vector<double>>> result;
	result.resize(payoffs.size());
	double m2m;
	vector<double> discDer;
	vector<double> projDer;
	unsigned long counter = 0;
	unsigned int longestDiscCurve = 0;
	unsigned int longestProjCurve = 0;
	for (const auto& payoff : payoffs)
	{
		m2m = payoff.second->presentValue();
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
		result.at(counter) = make_tuple(payoff.first, m2m, discDer, projDer);
		++counter;
	}

	long cuantosResultados = PyList_Size(legCharacteristics);
	PyObject* legM2MAndDelta = PyTuple_New(cuantosResultados);
	for (long i = 0; i < cuantosResultados; ++i)
	{
		unsigned int discVertices = get<2>(result.at(i)).size();
		unsigned int projVertices = get<3>(result.at(i)).size();
		PyObject* temp = PyTuple_New(discVertices + projVertices + 3);
		int success;
		success = PyTuple_SetItem(temp, 0, PyInt_FromLong(get<0>(result.at(i))));
		success = PyTuple_SetItem(temp, 1, PyFloat_FromDouble(get<1>(result.at(i))));
		success = PyTuple_SetItem(temp, 2, PyInt_FromLong(discVertices));
		for (unsigned int j = 0; j < discVertices; ++j)
		{
			success = PyTuple_SetItem(temp, j + 3, PyFloat_FromDouble(
				get<2>(result.at(i)).at(j) * BASIS_POINT));
		}
		for (unsigned int j = 0; j < projVertices; ++j)
		{
			success = PyTuple_SetItem(temp, j + 3 + discVertices, PyFloat_FromDouble(
				get<3>(result.at(i)).at(j) * BASIS_POINT));
		}
		success = PyTuple_SetItem(legM2MAndDelta, i, temp);
	}
	return legM2MAndDelta;

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
}