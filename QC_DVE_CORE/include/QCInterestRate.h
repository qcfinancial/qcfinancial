#ifndef QCINTERESTRATE_H
#define QCINTERESTRATE_H

#include "QCYearFraction.h"
#include "QCWealthFactor.h"
#include "QCDefinitions.h"

class QCInterestRate
{
public:
	QCInterestRate(double value, QCYrFrctnShrdPtr yearFraction,
		QCWlthFctrShrdPtr wealthFactor);
	double getValue();
	void setValue(double value);
	double wf(QCDate& startDate, QCDate& endDate);
	~QCInterestRate();

private:
	double _value;
	QCYrFrctnShrdPtr _yf;
	QCWlthFctrShrdPtr _wf;

};

#endif //QCINTERESTRATE_H
