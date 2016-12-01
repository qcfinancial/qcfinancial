#include "QCInterestRate.h"

QCInterestRate::QCInterestRate(double value, QCYearFraction yearFraction,
	QCWealthFactor wealthFactor) : _value(value),
	_yf(yearFraction), _wf(wealthFactor) {}

double wf(QCDate& startDate, QCDate& endDate)
{
	return 0.0;
}

QCInterestRate::~QCInterestRate() {}