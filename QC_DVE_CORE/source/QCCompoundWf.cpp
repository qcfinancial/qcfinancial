#include "QCCompoundWf.h"
#include <cmath>

QCCompoundWf::QCCompoundWf()
{
}

double QCCompoundWf::wf(double rate, double yf)
{
	_dwf = yf * pow(1 + rate, yf - 1);
	_d2wf = yf * (yf - 1) * pow(1 + rate, yf - 2);
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
