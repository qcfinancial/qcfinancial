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
	double wf(long days);
	double dwf(QCDate& startDate, QCDate& endDate);
	double dwf(long days);
	double getRateFromWf(double wf, QCDate& startDate, QCDate& endDate);
	double getRateFromWf(double wf, long days);
	~QCInterestRate();

private:
	double _value;
	double _dwf;
	QCYrFrctnShrdPtr _yf;
	QCWlthFctrShrdPtr _wf;

};

#endif //QCINTERESTRATE_H
