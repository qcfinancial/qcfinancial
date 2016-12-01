#include "QCCurve.h"
#include <exception>
#include <algorithm>

QCCurve::QCCurve(vector<double>& abscissa, vector<double>& ordinate)
{		
	//Se verifica si el numero de abscissa y ordinate coinciden
	if (abscissa.size() != ordinate.size())
	{
		string msg = "Error contsructing curve: ";
		msg += "number of abscissa is different from number of ordinate.";
		throw invalid_argument(msg);
	}

	//Se verifica que no se repitan valores de abcissa
	for (unsigned long i = 0; i < abscissa.size(); ++i)
	{
		for (unsigned long j = i + 1; j < abscissa.size(); ++j)
		{
			if (abscissa[i] == abscissa[j])
			{
				string msg = "Error constructing curve: ";
				msg += "Some values of abscissa coincide.";
				throw invalid_argument(msg);
			}
		}
	}

	//Se inicializa el vector _values
	if (abscissa.size() > 0)
	{
		for (unsigned long i = 0; i < abscissa.size(); ++i)
		{
			_values.push_back(make_pair(abscissa[i], ordinate[i]));
		}
	}
	else
	{
			_values.push_back(make_pair(0.0, 0.0));
	}

	//Ordenar las abscissa respetando el mapeo con la ordenada
	sort(_values.begin(), _values.end());
}

QCCurve::~QCCurve(void)
{
}

pair<double, double> QCCurve::getValuesAt(long position)
{
	return _values[position];
}

long QCCurve::getLength()
{
	return _values.size();
}
