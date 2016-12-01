#include "QCLinearWf.h"


QCLinearWf::QCLinearWf()
{
}

double QCLinearWf::wf(double rate, double yf)
{
	return 1 + rate * yf;
}

QCLinearWf::~QCLinearWf()
{
}
