#include "QCInterestRate.h"

QCInterestRate::QCInterestRate(double value, QCYrFrctnShrdPtr yearFraction,
	QCWlthFctrShrdPtr wealthFactor) : _value(value),
	_yf(yearFraction), _wf(wealthFactor) {}

double QCInterestRate::getValue()
{
	return _value;
}

void QCInterestRate::setValue(double value)
{
	_value = value;
}

double QCInterestRate::wf(QCDate& startDate, QCDate& endDate)
{
	double result = _wf->wf(_value, _yf->yf(startDate, endDate));
	_dwf = _wf->dwf();
	return result;
}

double QCInterestRate::wf(long days)
{
	double result = _wf->wf(_value, _yf->yf(days));
	_dwf = _wf->dwf();
	return result;
}

double QCInterestRate::yf(QCDate& startDate, QCDate& endDate)
{
	return _yf->yf(startDate, endDate);
}


double QCInterestRate::dwf(QCDate& startDate, QCDate& endDate)
{
	return _dwf;
}

double QCInterestRate::dwf(long days)
{
	return _dwf;
}

double QCInterestRate::getRateFromWf(double wf, QCDate& startDate, QCDate& endDate)
{
	double yf = _yf->yf(startDate, endDate);
	return _wf->rate(wf, yf);
}

double QCInterestRate::getRateFromWf(double wf, long days)
{
	double yf = _yf->yf(days);
	return _wf->rate(wf, yf);
}

QCInterestRate::~QCInterestRate()
{}