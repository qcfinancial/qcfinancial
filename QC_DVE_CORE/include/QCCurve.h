#ifndef QCCURVE_H
#define QCCURVE_H

#include <vector>
#include <utility>

#include "QCDate.h"

using namespace std;

/*!
* @brief QCCurve representa una curva de Set ---> R. Set debe ser ordenable y en la mayoría de las aplicaciones
* será Z (enteros relativos) o R (campo real).
*/
template<class T>
class QCCurve
{
public:
	/*!
	* Constructor por defecto
	*/
	QCCurve(void){}

	/*!
	* Constructor
	* @param abscissa<T> vector con las abscisas de la curva
	* @param vector<double> con las ordenadas de la curva
	*/
	QCCurve(vector<T>& abscissa, vector<double>& ordinate);

	/*!
	* Este método borra abscisas y ordenadas de la curva y vuelve a definir el tamaño de los
	* vectores de abscisa y ordenadas.
	* @param newSize nuevo tamaño de la curva
	*/
	void reset(unsigned long newSize);

	/*!
	* Este método actualiza el valor de un par ordenado de la curva. Si la abscisa corresponde
	* a un par ya existente, entonces sobre escribe los valores existentes. Si la abscisa no existe,
	* se inserta un nuevo par en la posición que corresponda (ordenado por abscisa ascendente).
	* @param x abscisa
	* @param y ordenada
	*/
	void setPair(T x, double y);

	/*!
	Método que actualiza el valor de la ordenada en una posición dada.
	* @param pos
	* @param value
	*/
	void setOrdinateAtWithValue(unsigned long position, double value);

	/*!
	Método que actualiza el valor de la abscisa en una posición dada.
	* @param pos
	* @param value
	*/
	void setAbscissaAtWithValue(unsigned long position, T value);

	/*!
	* Método que devuelve el valor de un par ordenado en una posición dada.
	* @param position posición del par buscado.
	* @return valor del par buscado.
	*/
	pair<T, double> getValuesAt(unsigned long position);

	/*!
	* Método que devuelve el largo de la curva modelada.
	@return largo de la curva.
	*/
	long getLength();

	/*!
	* Destructor
	*/
	~QCCurve(void);

protected:
	vector<pair<T, double>> _values;	//En esta variable se guardan los datos de la curva

};

#include <exception>
#include <algorithm>

template<class T> QCCurve<T>::QCCurve(vector<T> &abscissa, vector<double>& ordinate)
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
				msg += "Some values of abscissa coincide. ";
				msg += "Values " + to_string(abscissa[i]) + " and " + to_string(abscissa[j]);
				msg += " at positions " + to_string(i) + " and " + to_string(j);
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

template<class T> void QCCurve<T>::reset(unsigned long newSize)
{
	_values.clear();
	_values.resize(newSize);
}

template<class T> void QCCurve<T>::setPair(T x, double y)
{
	for (auto& elmnt : _values)
	{
		if (elmnt.first == x)
		{
			elmnt.second = y;
			return;
		}
	}
	_values.push_back(make_pair(x, y));
	sort(_values.begin(), _values.end());
}

template<class T> void QCCurve<T>::setOrdinateAtWithValue(unsigned long position, double value)
{
	_values.at(position).second = value;
	return;
}

template<class T> void QCCurve<T>::setAbscissaAtWithValue(unsigned long position, T value)
{
	_values.at(position).first = value;
	return;
}

template<class T> pair<T, double> QCCurve<T>::getValuesAt(unsigned long position)
{
	return _values[position];
}

template<class T> long QCCurve<T>::getLength()
{
	return _values.size();
}

#endif //QCCURVE_H

