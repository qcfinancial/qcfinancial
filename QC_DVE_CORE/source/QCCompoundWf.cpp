#include "QCCompoundWf.h"
#include <cmath>

QCCompoundWf::QCCompoundWf()
{
}

double QCCompoundWf::wf(double rate, double yf)
{
	return pow(1 + rate, yf);
}


QCCompoundWf::~QCCompoundWf()
{
}
