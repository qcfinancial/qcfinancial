#include "QCContinousWf.h"
#include <cmath>

QCContinousWf::QCContinousWf()
{
}

double QCContinousWf::wf(double rate, double yf)
{
	_dwf = yf * exp(rate * yf);
	_d2wf = pow(yf, 2) * exp(rate * yf);
	return exp(rate * yf);
}

double QCContinousWf::rate(double wf, double yf)
{
	return log(wf) / yf;
}

std::string QCContinousWf::description()
{
	return "Exp";
}

QCContinousWf::~QCContinousWf()
{
}
