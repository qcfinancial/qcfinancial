#include "QCCompoundWf.h"
#include <cmath>

QCCompoundWf::QCCompoundWf()
{
}

double QCCompoundWf::wf(double rate, double yf)
{
	_dwf = yf * pow(1 + rate, yf - 1);
	return pow(1 + rate, yf);
}

double QCCompoundWf::rate(double wf, double yf)
{
	return pow(wf, 1 / yf) - 1;
}

std::string QCCompoundWf::description()
{
	return "Com";
}

QCCompoundWf::~QCCompoundWf()
{
}
