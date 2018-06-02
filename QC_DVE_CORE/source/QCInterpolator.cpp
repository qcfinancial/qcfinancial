#include "QCInterpolator.h"
#include "QCDate.h"

#include <exception>

QCInterpolator::QCInterpolator()
{}

QCInterpolator::QCInterpolator(shared_ptr<QCCurve<long>> curve) : _curve(curve)
{}

void QCInterpolator::reset(unsigned long newSize)
{
	_curve->reset(newSize);
	_derivatives.resize(newSize);
}

void QCInterpolator::setPair(long x, double y)
{
	_curve->setPair(x, y);
}

void QCInterpolator::setOrdinateAtWithValue(unsigned long position, double value)
{
	_curve->setOrdinateAtWithValue(position, value);
}

void QCInterpolator::setAbscissaAtWithValue(unsigned long position, long value)
{
	_curve->setAbscissaAtWithValue(position, value);
}

size_t QCInterpolator::getLength()
{
	return _curve->getLength();
}

double QCInterpolator::rateDerivativeAt(unsigned long rateIndex)
{
	return _derivatives.at(rateIndex);
}

pair<long, double> QCInterpolator::getValuesAt(size_t position)
{
	return _curve->getValuesAt(position);
}

long QCInterpolator::_index(long arg)
{
	vector<double> data;
	for (long i = 0; i < _curve->getLength(); ++i)
	{
		data.push_back(_curve->getValuesAt(i).first);
	}

	long inf = 0;
	long sup = static_cast<long>(data.size()) - 1;
	long centro = (inf + sup) / 2;
	long indice = centro;
    if (data.size() < 2)
	{
		throw invalid_argument("At least two data points are needed.");
		return 1;
	}
	while (inf <= sup)
	{
		if (arg == data[centro])
		{
			indice = centro;
			inf = static_cast<long>(data.size());
		}
		else
		{
			if (arg < data[centro])
				sup = centro-1;
			else
				inf = centro+1;
		indice = sup;
		}
		centro = (inf + sup) / 2;

	}	
	if (indice < 0)
		indice = 0;

    return indice;
}

QCInterpolator::~QCInterpolator(void)
{}

