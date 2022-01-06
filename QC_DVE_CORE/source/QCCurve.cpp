#include "curves/QCCurve.h"
#include <exception>
#include <algorithm>

template<class T>
QCCurve<T>::QCCurve(vector<T> &abscissa, vector<double>& ordinate)
{		
	//Se verifica si el numero de abscissa y ordinate coinciden
	if (abscissa.size() != ordinate.size())
	{
		string msg = "Error constructing curve: ";
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
			throw runtime_error("El vector de abscissa no contiene datos.");
	}

	//Ordenar las abscissa respetando el mapeo con la ordenada
	sort(_values.begin(), _values.end());
}

template<class T> QCCurve<T>::~QCCurve(void)
{
}

template<class T> pair<T, double> QCCurve<T>::getValuesAt(long position)
{
	return _values[position];
}

template<class T> long QCCurve<T>::getLength()
{
	return _values.size();
}