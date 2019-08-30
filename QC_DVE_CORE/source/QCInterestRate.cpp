#include <memory>
#include "asset_classes/QCInterestRate.h"

QCInterestRate::QCInterestRate(double value,
							   QCYrFrctnShrdPtr yearFraction,
							   QCWlthFctrShrdPtr wealthFactor) :
							   _value(value),
							   _yf(yearFraction),
							   _wf(wealthFactor),
							   _dwf(0.0),
							   _d2wf(0.0)
{
}

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
	_d2wf = _wf->d2wf();
	return result;
}

double QCInterestRate::wf(long days)
{
	double result = _wf->wf(_value, _yf->yf(days));
	_dwf = _wf->dwf();
	_d2wf = _wf->d2wf();
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

double QCInterestRate::d2wf(QCDate& startDate, QCDate& endDate)
{
	return _d2wf;
}

double QCInterestRate::dwf(long days)
{
	return _dwf;
}

double QCInterestRate::d2wf(long days)
{
	return _d2wf;
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

shared_ptr<QCWealthFactor> QCInterestRate::getWealthFactor()
{
	return _wf;
}

shared_ptr<QCYearFraction> QCInterestRate::getYearFraction()
{
	return _yf;
}

std::string QCInterestRate::description()
{
	return std::to_string(_value) + " " + _yf->description() + " " + _wf->description();
}

QCInterestRate::~QCInterestRate()
{}