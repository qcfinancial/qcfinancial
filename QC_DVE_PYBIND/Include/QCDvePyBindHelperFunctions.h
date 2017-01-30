#ifndef QCDVEPYBINDHELPERFUNCTIONS_H
#define QCDVEPYBINDHELPERFUNCTIONS_H

#include <map>

#include "include.h"
#include "QCDate.h"

using namespace std;

namespace QCDvePyBindHelperFunctions
{
	typedef tuple<string, string, string, string> string4;

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

		cout << "icp_fixings" << endl;
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