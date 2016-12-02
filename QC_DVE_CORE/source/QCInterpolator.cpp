#include "QCInterpolator.h"
#include <exception>

QCInterpolator::QCInterpolator(shared_ptr<QCCurve> curve) : _curve(curve)
{}

QCInterpolator::~QCInterpolator(void)
{}

long QCInterpolator::index(double arg)
{
	vector<double> data;
	for (long i = 0; i < _curve->getLength(); ++i)
	{
		data.push_back(_curve->getValuesAt(i).first);
	}

	long inf = 0;
	long sup = data.size() - 1;
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
			inf = data.size();
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

