#include "asset_classes/QCLinearWf.h"

QCLinearWf::QCLinearWf()
{
}

double QCLinearWf::wf(double rate, double yf)
{
	_dwf = yf;
	_d2wf = 0.0;
	return 1 + rate * yf;
}

double QCLinearWf::rate(double wf, double yf)
{
	return (wf - 1) * 1 / yf;
}

std::string QCLinearWf::description()
{
	return "Lin";
}

QCLinearWf::~QCLinearWf()
{
}
