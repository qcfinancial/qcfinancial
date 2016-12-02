#include "QCContinousWf.h"
#include <cmath>

QCContinousWf::QCContinousWf()
{
}

double QCContinousWf::wf(double rate, double yf)
{
	return exp(rate * yf);
}

QCContinousWf::~QCContinousWf()
{
}
