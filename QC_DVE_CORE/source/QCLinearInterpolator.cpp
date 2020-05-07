#include "curves/QCLinearInterpolator.h"

QCLinearInterpolator::QCLinearInterpolator(shared_ptr<QCCurve<long>> curve) :
	QCInterpolator(curve)
{
	_derivatives.resize(_curve->getLength());
}

double QCLinearInterpolator::interpolateAt(long value)
{
	for (size_t j = 0; j < _derivatives.size(); ++j)
	{
		_derivatives.at(j) = 0;
	}
	
	long i = _index(value);
	
	if (i == _curve->getLength() - 1)
	{
		_derivatives.at(i) = 1.0;
		return _curve->getValuesAt(i).second;
	}
	else
	{
		double x1 = _curve->getValuesAt(i).first;
		double x2 = _curve->getValuesAt(i + 1).first;
		double y1 = _curve->getValuesAt(i).second;
		double y2 = _curve->getValuesAt(i + 1).second;

		if (value < x1)
		{
			_derivatives.at(i) = 1.0;
			_derivatives.at(i + 1) = 0.0;
			return y1;
		}
		if (value > x2)
		{
			_derivatives.at(i) = 0.0;
			_derivatives.at(i + 1) = 1.0;
			return y2;
		}
		_derivatives.at(i) = - ((value - x1) / (x2 - x1) - 1);
		_derivatives.at(i + 1) = (value - x1) / (x2 - x1);

		return (y2 - y1) / (x2 - x1) * (value - x1) + y1;
	}
}

double QCLinearInterpolator::derivativeAt(long value)
{
	long i = _index(value);

	//Si es el primer o ultimo x de la curva

	if (i == _curve->getLength() - 1 || i == 0)
	{
		//Retorna la primera o última y de la curva
		return _curve->getValuesAt(i).second;
	}

	double x1 = _curve->getValuesAt(i).first;
	double x2;
	if (i == _curve->getLength() - 1)
	{
		x2 = _curve->getValuesAt(i - 1).first;
	}
	else
	{
		x2 = _curve->getValuesAt(i + 1).first;
	}
	double y1 = _curve->getValuesAt(i).second;
	double y2;
	if (i == _curve->getLength() - 1)
	{
		y2 = _curve->getValuesAt(i - 1).second;
	}
	else
	{
		y2 = _curve->getValuesAt(i + 1).second;
	}

	return (y2 - y1) / (x2 - x1); // Aquí estamos entregando la derivada de izquierda hacia derecha.
}

double QCLinearInterpolator::secondDerivativeAt(long value)
{
	return 0.0;
}

QCLinearInterpolator::~QCLinearInterpolator()
{}
