#include "QCLinearInterpolator.h"


QCLinearInterpolator::QCLinearInterpolator(shared_ptr<QCCurve> curve) : QCInterpolator(curve)
{
	_derivatives.resize(_curve->getLength());
}

double QCLinearInterpolator::interpolateAt(double value)
{
	long i = index(value);
	for (unsigned int j = 0; j < _derivatives.size(); ++j)
	{
		_derivatives.at(j) = 0;
	}
	//Si es el primer o ultimo x de la curva
	if (i == _curve->getLength() - 1 || i == 0)
	{
		//Retorna la primera o ultima y de la curva
		return _curve->getValuesAt(i).second;
		if (i == 0)
		{
			_derivatives.at(0) = 1;
		}
		else
		{
			_derivatives.at(_derivatives.size() - 1) = 1;
		}
	}	

	double x1 = _curve->getValuesAt(i).first;
	double x2 = _curve->getValuesAt(i + 1).first;
	double y1 = _curve->getValuesAt(i).second;
	double y2 = _curve->getValuesAt(i + 1).second;

	if (value == x1)
	{
		_derivatives.at(i) = 1;
	}
	else
	{
		_derivatives.at(i) = -1 / (x2 - x1) * (value - x1) + 1;
		_derivatives.at(i + 1) = 1 / (x2 - x1) * (value - x1);
	}
	return (y2 - y1) / (x2 - x1) * (value - x1) + y1;
}

double QCLinearInterpolator::derivativeAt(double value)
{
	long i = index(value);

	//Si es el primer o ultimo x de la curva

	if (i == _curve->getLength() - 1 || i == 0)
	{
		//Retorna la primera o ultima y de la curva
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

	return (y2 - y1) / (x2 - x1); //Aqui estamos entregando la derivada de izquierda hacia derecha
}

double QCLinearInterpolator::secondDerivativeAt(double value)
{
	return 0.0;
}

QCLinearInterpolator::~QCLinearInterpolator()
{}
