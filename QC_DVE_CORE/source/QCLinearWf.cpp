#include "asset_classes/QCLinearWf.h"


double QCLinearWf::wf(double rate, double yf)
{
	_dwf = yf;
	_d2wf = 0.0;
	return 1 + rate * yf;
}

double QCLinearWf::rate(double wf, double yf)
{
    auto result = (wf - 1) * 1 / yf;
    _drate = wf / yf;
	return result;
}

std::string QCLinearWf::description()
{
	return "Lin";
}

QCLinearWf::~QCLinearWf()
{
}
