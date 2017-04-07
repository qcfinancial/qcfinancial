#ifndef QCDVEPYBINDHELPERFUNCTIONS_H
#define QCDVEPYBINDHELPERFUNCTIONS_H

#include <map>

#include "include.h"
#include "QCDate.h"
#include "QCInterestRateLeg.h"
#include "QCHelperFunctions.h"

using namespace std;

namespace QCDvePyBindHelperFunctions
{
	typedef tuple<string, string, string, string> string4;
	
	//start_date, end_date, value, yf, wf
	typedef tuple<QCDate, QCDate, double, string, string> ZeroRate;
	
	typedef tuple<
		string,								//0		receive or pay
		QCDate,								//1		start_date
		QCDate,								//2		end_date
		unsigned int,						//3		settlement_lag
		QCInterestRateLeg::QCStubPeriod,	//4		stub period
		string,								//5		periodicity
		QCDate::QCBusDayAdjRules,			//6		end_date_adjustment
		QCInterestRateLeg::QCAmortization,	//7		amortization
		double,								//8		rate
		double,								//9		notional
		string,								//10	yf
		string								//11	wf
	> SwapIndex;

	typedef tuple<
		double,								//0 notional1 (strong currency)
		double,								//1 notional2 (weak currency)
		QCDate,								//2 end_date
		QCCurrencyConverter::QCCurrency,	//3 present value currency
		QCCurrencyConverter::QCCurrency,	//4 notional1 currency (strong)
		QCCurrencyConverter::QCCurrency,	//5 notional2 currency (weak)
		QCCurrencyConverter::QCFxRate,		//6 fxRate1 (strong a present value)
		QCCurrencyConverter::QCFxRate		//7 fxRate2 (weak a presente value)
	> FwdIndex;
	
	void buildAuxTenorsAndRates(PyObject* auxCurveValues, vector<long>& auxTenors, vector<double>& auxRates)
	{
		//cout << "Enter buildAuxTenorsAndRates" << endl;
		size_t numValues = PyList_Size(auxCurveValues);
		//cout << "\tnumValues: " << numValues << endl;
		auxTenors.resize(numValues);
		auxRates.resize(numValues);
		for (size_t i = 0; i < numValues; ++i)
		{
			auxTenors.at(i) = PyInt_AsLong(PyTuple_GetItem(PyList_GetItem(auxCurveValues, i), 2));
			auxRates.at(i) = PyFloat_AsDouble(PyTuple_GetItem(PyList_GetItem(auxCurveValues, i), 3));
		}
	}

	void buildZeroRateVector(PyObject* input, QCDate& processDate, vector<QCDate>& dateVector,
		vector<ZeroRate>& zeroRateVector)
	{
		//(nombre, start_date_rule, tenor, calendar, yf, wf, valor)
		size_t numRates = PyList_Size(input);
		zeroRateVector.resize(numRates);
		cout << "Enter buildZeroRateVector" << endl;
		for (size_t i = 0; i < numRates; ++i)
		{
			ZeroRate temp;

			cout << "\t" << PyString_AsString(PyTuple_GetItem((PyList_GetItem(input, i)), 0)) << endl;
			//Build start date
			string rule = PyString_AsString(PyTuple_GetItem((PyList_GetItem(input, i)), 1));
			unsigned int lag = QCHelperFunctions::tenor(rule);
			get<0>(temp) = processDate.shift(dateVector, lag, QCDate::QCBusDayAdjRules::qcFollow);
			cout << "\tbuildZeroRateVector: start_date " + get<0>(temp).description() << endl;

			//Build end date
			rule = PyString_AsString(PyTuple_GetItem(PyList_GetItem(input, i), 2));
			QCHelperFunctions::lowerCase(rule);
			if (rule.substr(rule.size() - 1, 1) == "d")
			{
				lag = QCHelperFunctions::tenor(rule);
				get<1>(temp) = get<0>(temp).addDays(lag).businessDay(dateVector,
					QCDate::QCBusDayAdjRules::qcFollow);
			}
			else
			{
				lag = QCHelperFunctions::tenor(rule);
				get<1>(temp) = get<0>(temp).addMonths(lag).businessDay(dateVector,
					QCDate::QCBusDayAdjRules::qcFollow);
			}
			cout << "\tbuildZeroRateVector: end_date " + get<1>(temp).description() << endl;

			//Build value
			get<2>(temp) = PyFloat_AsDouble(PyTuple_GetItem(PyList_GetItem(input, i), 6));
			cout << "\tbuildZeroRateVector: rate " << get<2>(temp) << endl;
			
			//Build yf
			string tempStr = PyString_AsString(PyTuple_GetItem(PyList_GetItem(input, i), 4));
			QCHelperFunctions::lowerCase(tempStr);
			get<3>(temp) = tempStr;
			cout << "\tbuildZeroRateVector: yf " << get<3>(temp) << endl;

			//Build wf
			tempStr = PyString_AsString(PyTuple_GetItem(PyList_GetItem(input, i), 5));
			QCHelperFunctions::lowerCase(tempStr);
			get<4>(temp) = tempStr;
			cout << "\tbuildZeroRateVector: wf " << get<4>(temp) << endl;

			//Agrega temp al vector resultado
			zeroRateVector.at(i) = temp;
		}
	}

	void buildFwdVector(PyObject* input, vector<FwdIndex>& fwdIndexVector)
	{
		size_t numFwds = PyList_Size(input);
		fwdIndexVector.resize(numFwds);
		QCCurrencyConverter conv;
		for (size_t i = 0; i < numFwds; ++i)
		{
			FwdIndex temp;
			get<0>(temp) = 1.0;
			get<1>(temp) = PyFloat_AsDouble(PyTuple_GetItem(PyList_GetItem(input, i), 1));
			get<2>(temp) = QCDate{ string(PyString_AsString(PyTuple_GetItem(PyList_GetItem(input, i), 2))) };
			get<3>(temp) = conv.getWeakCurrencyEnum(
				string(PyString_AsString(PyTuple_GetItem(PyList_GetItem(input, i), 3))));
			get<4>(temp) = conv.getStrongCurrencyEnum(
				string(PyString_AsString(PyTuple_GetItem(PyList_GetItem(input, i), 3))));
			get<5>(temp) = get<3>(temp);
			get<6>(temp) = conv.getStandardFxRate(get<4>(temp), get<3>(temp));
			get<7>(temp) = conv.getStandardFxRate(get<3>(temp), get<3>(temp));
			fwdIndexVector.at(i) = temp;
		}
	}

	void buildFixedRateIndexVector(PyObject* input, QCDate& processDate, vector<QCDate>& dateVector,
		vector<SwapIndex>& swapIndexVector)
	{
		//Esto es un ejemplo de lo que entra:
		//('SwapCamCLP9M' 0, False 1, 2 (start_date_lag) 2, '9M' 3, None (start_date) 4, None (end_date) 5,
		//'SCL' 6, 0L 7, 'CORTO INICIO' 8, '2Y' 9, 'FOLLOW' 10, 'BULLET' 11, 1.0 12, 'LIN' 13, 
		//'ACT/360' 14, 'CAMARACLP' 15, 0.0291 16)

		//Se quiere obtener
		//string (R) 0, QCDate (start) 1, QCDate (end) 2, unsigned int (sett lag) 3, enum stub 4,
		//string (periodicity) 5, enum follow 6, enum amort 7, double (rate) 8, double (notional) 9,
		//enum wf 10, enum yf 11
		size_t numSwaps = PyList_Size(input);
		swapIndexVector.resize(numSwaps);
		cout << "buildFixedRateIndexVector" << endl;
		for (size_t i = 0; i < numSwaps; ++i)
		{
			SwapIndex temp;
			cout << endl;
			
			//Receive or pay
			get<0>(temp) = "R";
			cout << "\tbuildFixedRateIndexVector: receive or pay " << get<0>(temp) << endl;

			//Build start date (start_date_lag 2)
			unsigned int lag = PyInt_AsLong(PyTuple_GetItem(PyList_GetItem(input, i), 2));
			get<1>(temp) = processDate.shift(dateVector, lag, QCDate::QCBusDayAdjRules::qcFollow);
			cout << "\tbuildFixedRateIndexVector: start date " << get<1>(temp).description() << endl;

			//Build end date (5)
			long condition =  PyInt_AsLong(PyTuple_GetItem(PyList_GetItem(input, i), 1));
			if (condition)
			{
				string tempDate = PyString_AsString(PyTuple_GetItem(PyList_GetItem(input, i), 5));
				get<2>(temp) = QCDate{ tempDate };
				cout << "\tbuildFixedRateIndexVector: end date " << get<2>(temp).description() << endl;
			}
			else
			{
				unsigned int m = QCHelperFunctions::tenor(
					PyString_AsString(
					PyTuple_GetItem(
					PyList_GetItem(input, i), 3)));
				get<2>(temp) = get<1>(temp).addMonths(m).businessDay(dateVector, QCDate::QCBusDayAdjRules::qcFollow);
				cout << "\tbuildFixedRateIndexVector: end date " << get<2>(temp).description() << endl;
			}

			//Build settlement lag (7)
			get<3>(temp) = PyInt_AsLong(PyTuple_GetItem(PyList_GetItem(input, i), 7));
			cout << "\tbuildFixedRateIndexVector: settlement lag " << get<3>(temp) << endl;

			//Build enum stub (8)
			string stub = PyString_AsString(PyTuple_GetItem(PyList_GetItem(input, i), 8));
			get<4>(temp) = QCHelperFunctions::stringToQCStubPeriod(stub);
			cout << "\tbuildFixedRateIndexVector: enum stub " << get<4>(temp) << endl;

			//Build periodicity (9)
			get<5>(temp) = string(PyString_AsString(PyTuple_GetItem(PyList_GetItem(input, i), 9)));
			cout << "\tbuildFixedRateIndexVector: periodicity " << get<5>(temp) << endl;

			//Build enum end date adjustment (10)
			string endDatAdj = PyString_AsString(PyTuple_GetItem(PyList_GetItem(input, i), 10));
			get<6>(temp) = QCHelperFunctions::stringToQCBusDayAdjRule(endDatAdj);
			cout << "\tbuildFixedRateIndexVector: enum end date adj " << get<6>(temp) << endl;

			//Build enum amort (11)
			string amort = PyString_AsString(PyTuple_GetItem(PyList_GetItem(input, i), 11));
			get<7>(temp) = QCHelperFunctions::stringToQCAmortization(amort);
			cout << "\tbuildFixedRateIndexVector: enum amort " << get<7>(temp) << endl;
			 
			//Build rate (16)
			get<8>(temp) = PyFloat_AsDouble(PyTuple_GetItem(PyList_GetItem(input, i), 16));
			cout << "\tbuildFixedRateIndexVector: rate " << get<8>(temp) << endl;

			//Build notional (12)
			get<9>(temp) = PyFloat_AsDouble(PyTuple_GetItem(PyList_GetItem(input, i), 12));
			cout << "\tbuildFixedRateIndexVector: notional " << get<9>(temp) << endl;

			//Build enum wf (14)
			string yf = PyString_AsString(PyTuple_GetItem(PyList_GetItem(input, i), 14));
			QCHelperFunctions::lowerCase(yf);
			get<10>(temp) = yf;
			cout << "\tbuildFixedRateIndexVector: enum yf " << get<10>(temp) << endl;

			//Build enum yf (13)
			string wf = PyString_AsString(PyTuple_GetItem(PyList_GetItem(input, i), 13));
			QCHelperFunctions::lowerCase(wf);
			get<11>(temp) = wf;
			cout << "\tbuildFixedRateIndexVector: enum wf " << get<11>(temp) << endl;

			swapIndexVector.at(i) = temp;
		}
	}

	void buildQCDateVector(PyObject* input, vector<QCDate>& dateVector)
	{
		size_t sizeInput = PyList_Size(input);
		dateVector.resize(sizeInput);
		for (size_t i = 0; i < sizeInput; ++i)
		{
			QCDate tempDate{ string(PyString_AsString(PyList_GetItem(input, 0))) };
			dateVector.at(i) = tempDate;
		}
	}

	void buildHolidays(PyObject* holidays, map<string, vector<QCDate>>& mapHolidays)
	{
		string lastName, nextName;
		lastName = PyString_AsString(PyList_GetItem(PyList_GetItem(holidays, 0), 0));
		vector<QCDate> temp;
		for (long i = 0; i < PyList_Size(holidays); ++i)
		{
			nextName = PyString_AsString(PyList_GetItem(PyList_GetItem(holidays, i), 0));
			if (nextName == lastName)
			{
				temp.push_back(QCDate{ string(PyString_AsString(
					PyList_GetItem(PyList_GetItem(holidays, i), 1))) });
				lastName = nextName;
			}
			else
			{
				mapHolidays.insert(pair<string, vector<QCDate>>{lastName, temp});
				temp.clear();
				temp.push_back(QCDate{ string(PyString_AsString(
					PyList_GetItem(PyList_GetItem(holidays, i), 1))) });
				lastName = nextName;
			}
		}
		mapHolidays.insert(pair<string, vector<QCDate>>{lastName, temp});
		return;
	}

	void buildCurveValues(PyObject* curveValues, map<string, pair<vector<long>, vector<double>>>& crvValues)
	{
		string lastName, nextName;
		lastName = PyString_AsString(PyList_GetItem(PyList_GetItem(curveValues, 0), 0));
		vector<long> tenors;
		vector<double> values;
		for (long i = 0; i < PyList_Size(curveValues); ++i)
		{
			nextName = PyString_AsString(PyList_GetItem(PyList_GetItem(curveValues, i), 0));
			if (nextName == lastName)
			{
				tenors.push_back(PyInt_AsLong(PyList_GetItem(PyList_GetItem(curveValues, i), 1)));
				values.push_back(PyFloat_AsDouble(PyList_GetItem(PyList_GetItem(curveValues, i), 2)));
				lastName = nextName;
			}
			else
			{
				crvValues.insert(pair<string, pair<vector<long>, vector<double>>>{lastName, { tenors, values }});
				tenors.clear();
				values.clear();
				tenors.push_back(PyInt_AsLong(PyList_GetItem(PyList_GetItem(curveValues, i), 1)));
				values.push_back(PyFloat_AsDouble(PyList_GetItem(PyList_GetItem(curveValues, i), 2)));
				lastName = nextName;
			}
		}
		crvValues.insert(pair<string, pair<vector<long>, vector<double>>>{lastName, { tenors, values }});
	}

	void buildCurveCharacteristics(PyObject* curveChars,
		map<string, QCDvePyBindHelperFunctions::string4>& crvChars)
	{
		for (int i = 0; i < PyList_Size(curveChars); ++i)
		{
			string4 temp = make_tuple(PyString_AsString(PyList_GetItem(PyList_GetItem(curveChars, i), 1)),
				PyString_AsString(PyList_GetItem(PyList_GetItem(curveChars, i), 2)),
				PyString_AsString(PyList_GetItem(PyList_GetItem(curveChars, i), 3)),
				PyString_AsString(PyList_GetItem(PyList_GetItem(curveChars, i), 4)));
			crvChars.insert(pair<string, string4>(
				PyString_AsString(PyList_GetItem(PyList_GetItem(curveChars, i), 0)), temp));
		}
	}

	void buildFxRateIndexFxRate(PyObject* fxRateChars,
		map<string, string>& fxRateIndexFxRate)
	{
		for (int i = 0; i < PyList_Size(fxRateChars); ++i)
		{
			fxRateIndexFxRate.insert(pair<string, string>(
				PyString_AsString(PyList_GetItem(PyList_GetItem(fxRateChars, i), 0)),
				PyString_AsString(PyList_GetItem(PyList_GetItem(fxRateChars, i), 1))));
		}
	}

	void buildCustomAmortization(PyObject* customAmort,
		map<unsigned long, vector<tuple<QCDate, double, double>>>& dateNotionalAndAmortByIdLeg)
	{
		unsigned long lastIdLeg = PyInt_AsLong(PyList_GetItem(PyList_GetItem(customAmort, 0), 0));
		QCDate tempDate;
		double tempAmort;
		double tempNotional;
		tuple<QCDate, double, double> tempValue;
		vector<tuple<QCDate, double, double>> tempVector;
		for (unsigned long i = 0; i < PyList_Size(customAmort); ++i)
		{
			unsigned long idLeg = PyInt_AsLong(PyList_GetItem(PyList_GetItem(customAmort, i), 0));;
			tempDate = QCDate{ string(PyString_AsString(PyList_GetItem(PyList_GetItem(customAmort, i), 1))) };
			tempAmort = PyFloat_AsDouble(PyList_GetItem(PyList_GetItem(customAmort, i), 2));
			tempNotional = PyFloat_AsDouble(PyList_GetItem(PyList_GetItem(customAmort, i), 3));
			tempValue = make_tuple(tempDate, tempAmort, tempNotional);
			if (idLeg == lastIdLeg)
			{
				tempVector.push_back(tempValue);
				lastIdLeg = idLeg;
			}
			else
			{
				dateNotionalAndAmortByIdLeg.insert(pair<unsigned long, vector<tuple<QCDate, double, double>>>
					(lastIdLeg, tempVector));
				tempVector.clear();
				tempVector.push_back(tempValue);
				lastIdLeg = idLeg;
			}
		}

		//Insertamos en el ultimo vector en el mapa 
		dateNotionalAndAmortByIdLeg.insert(pair<unsigned long, vector<tuple<QCDate, double, double>>>
			(lastIdLeg, tempVector));
		tempVector.clear();

	}

	void buildFixings(PyObject* fixings, map<QCDate, double>& mapFixings)
	{
		for (unsigned long i = 0; i < PyList_Size(fixings); ++i)
		{
			mapFixings.insert(pair<QCDate, double>(QCDate{ string(
				PyString_AsString(PyList_GetItem(PyList_GetItem(fixings, i), 0))) },
				PyFloat_AsDouble(PyList_GetItem(PyList_GetItem(fixings, i), 1))));
		}
	}

	void buildManyFixings(PyObject* fixings, map<string, map<QCDate, double>>& mapManyFixings)
	{
		//La list de fixings debe venir ordenada por nombre
		string lastName, nextName;
		lastName = PyString_AsString(PyList_GetItem(PyList_GetItem(fixings, 0), 0));
		map<QCDate, double> tempMap;
		unsigned long numRows = PyList_Size(fixings);
		for (unsigned long i = 0; i < numRows; ++i)
		{
			nextName = PyString_AsString(PyList_GetItem(PyList_GetItem(fixings, i), 0));
			if (nextName == lastName)
			{
				tempMap.insert(pair<QCDate, double>(QCDate{
					string(PyString_AsString(PyList_GetItem(PyList_GetItem(fixings, i), 1))) },
					PyFloat_AsDouble(PyList_GetItem(PyList_GetItem(fixings, i), 2))));
				lastName = nextName;
			}
			else
			{
				mapManyFixings.insert(pair<string, map<QCDate, double>>(lastName, tempMap));
				tempMap.clear();
				tempMap.insert(pair<QCDate, double>(QCDate{ string(PyString_AsString(PyList_GetItem(PyList_GetItem(fixings, i), 1))) },
					PyFloat_AsDouble(PyList_GetItem(PyList_GetItem(fixings, i), 2))));
				lastName = nextName;
			}
		}
		//Un insert final
		mapManyFixings.insert(pair<string, map<QCDate, double>>(lastName, tempMap));
	}

	void buildStringPairStringMap(PyObject* stringMatrix, map<string, pair<string, string>>& mapStringPairString)
	{
		for (unsigned long i = 0; i < PyList_Size(stringMatrix); ++i)
		{
			mapStringPairString.insert(pair<string, pair<string, string>>{
				string(PyString_AsString(PyList_GetItem(PyList_GetItem(stringMatrix, i), 0))),
					make_pair(string(PyString_AsString(PyList_GetItem(PyList_GetItem(stringMatrix, i), 1))),
					string(PyString_AsString(PyList_GetItem(PyList_GetItem(stringMatrix, i), 2))))});
		}
	}
}

#endif //QCDVEPYBINDHELPERFUNCTIONS_H