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
	return _wf->wf(_value, _yf->yf(startDate, endDate));
}

QCInterestRate::~QCInterestRate()
{}