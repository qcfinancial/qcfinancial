#ifndef QCINTERESTRATE_H
#define QCINTERESTRATE_H

#include "QCYearFraction.h"
#include "QCWealthFactor.h"

class QCInterestRate
{
public:
	QCInterestRate(double value, QCYearFraction yearFraction,
		QCWealthFactor wealthFactor);
	double wf(QCDate& startDate, QCDate& endDate);
	~QCInterestRate();

private:
	double _value;
	QCYearFraction _yf;
	QCWealthFactor _wf;

};

#endif //QCINTERESTRATE_H
