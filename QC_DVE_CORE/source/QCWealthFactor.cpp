#include "QCWealthFactor.h"

QCWealthFactor::QCWealthFactor()
{
}

double QCWealthFactor::wf(double rate, double yf)
{
	return 0.0;
}


double QCWealthFactor::rate(double wf, double yf)
{
	return 0.0;
}

double QCWealthFactor::dwf()
{
	return _dwf;
}

double QCWealthFactor::d2wf()
{
	return _d2wf;
}

std::string QCWealthFactor::description()
{
	return "";
}

QCWealthFactor::~QCWealthFactor()
{
}