#ifndef HELPERFUNCTIONS_H
#define HELPERFUNCTIONS_H

#include "xlw/MyContainers.h"
#include <xlw/CellMatrix.h>
#include <xlw/DoubleOrNothing.h>
#include <xlw/ArgList.h>

#include <string>
#include <vector>
#include <utility>

#include "QCDate.h"

using namespace xlw;
using namespace std;

namespace HelperFunctions
{
	typedef tuple<string, string, string, string> string4;

	void buildHolidays(CellMatrix& holidays, map<string, vector<QCDate>>& mapHolidays)
	{
		string lastName, nextName;
		lastName = holidays(0, 0).StringValue();
		vector<QCDate> temp;
		for (long i = 0; i < holidays.RowsInStructure(); ++i)
		{
			nextName = holidays(i, 0).StringValue();
			if (nextName == lastName)
			{
				temp.push_back(QCDate{ (long)holidays(i, 1).NumericValue() });
				lastName = nextName;
			}
			else
			{
				mapHolidays.insert(pair<string, vector<QCDate>>{lastName, temp});
				temp.clear();
				temp.push_back(QCDate{ (long)holidays(i, 1).NumericValue() });
				lastName = nextName;
			}
		}
		mapHolidays.insert(pair<string, vector<QCDate>>{lastName, temp});
		return;
	}

	void buildCurveValues(CellMatrix& curveValues, map<string, pair<vector<long>, vector<double>>>& crvValues)
	{
		string lastName, nextName;
		lastName = curveValues(0, 0).StringValue();
		vector<long> tenors;
		vector<double> values;
		for (long i = 0; i < curveValues.RowsInStructure(); ++i)
		{
			nextName = curveValues(i, 0).StringValue();
			if (nextName == lastName)
			{
				double aux = curveValues(i, 1).NumericValue();
				tenors.push_back((long)curveValues(i, 1).NumericValue());
				values.push_back((double)curveValues(i, 2).NumericValue());
				lastName = nextName;
			}
			else
			{
				crvValues.insert(pair<string, pair<vector<long>, vector<double>>>{lastName, { tenors, values }});
				tenors.clear();
				values.clear();
				tenors.push_back((long)curveValues(i, 1).NumericValue());
				values.push_back((double)curveValues(i, 2).NumericValue());
				lastName = nextName;
			}
		}
		crvValues.insert(pair<string, pair<vector<long>, vector<double>>>{lastName, { tenors, values }});

	}

	void buildCurveCharacteristics(CellMatrix& curveChars, map<string, string4>& crvChars)
	{
		for (int i = 0; i < curveChars.RowsInStructure(); ++i)
		{
			string4 temp = make_tuple(curveChars(i, 1).StringValue(),
				curveChars(i, 2).StringValue(),
				curveChars(i, 3).StringValue(),
				curveChars(i, 4).StringValue());
			crvChars.insert(pair<string, string4>(curveChars(i, 0).StringValue(),
				temp));
		}
	}

	void buildCustomAmortization(CellMatrix& customAmort,
		map<unsigned long, vector<tuple<QCDate, double, double>>>& dateNotionalAndAmortByIdLeg)
	{
		unsigned long lastIdLeg = (long)customAmort(0, 0).NumericValue();
		QCDate tempDate;
		double tempAmort;
		double tempNotional;
		tuple<QCDate, double, double> tempValue;
		vector<tuple<QCDate, double, double>> tempVector;
		for (unsigned long i = 0; i < customAmort.RowsInStructure(); ++i)
		{
			unsigned long idLeg = customAmort(i, 0).NumericValue();
			tempDate = QCDate{ (long)customAmort(i, 1).NumericValue() };
			tempAmort = (double)customAmort(i, 2).NumericValue();
			tempNotional = (double)customAmort(i, 3).NumericValue();
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

	void buildFixings(CellMatrix& fixings, map<QCDate, double>& mapFixings)
	{
		for (unsigned long i = 0; i < fixings.RowsInStructure(); ++i)
		{
			mapFixings.insert(pair<QCDate, double>(QCDate{ (long)fixings(i, 0).NumericValue() },
				fixings(i, 1).NumericValue()));
		}
	}
}

#endif