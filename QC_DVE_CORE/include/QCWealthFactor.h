#ifndef QWEALTHFACTOR_H
#define QWEALTHFACTOR_H

class QCWealthFactor
{
public:
	QCWealthFactor();
	virtual double wf(double rate, double yf) = 0;
	virtual double rate(double wf, double yf) = 0;
	double dwf();
	virtual ~QCWealthFactor();

protected:
	double _dwf;
};

#endif //QWEALTHFACTOR_H

