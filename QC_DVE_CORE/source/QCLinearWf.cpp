#include "QCLinearWf.h"

QCLinearWf::QCLinearWf()
{
}

double QCLinearWf::wf(double rate, double yf)
{
	_dwf = yf;
	return 1 + rate * yf;
}

double QCLinearWf::rate(double wf, double yf)
{
	return (wf - 1) * 1 / yf;
}

QCLinearWf::~QCLinearWf()
{
}
