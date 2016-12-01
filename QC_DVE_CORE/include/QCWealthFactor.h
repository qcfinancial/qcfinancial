#ifndef QWEALTHFACTOR_H
#define QWEALTHFACTOR_H

class QCWealthFactor
{
public:
	QCWealthFactor();
	virtual double wf(double rate, double yf) { return 0.0; };
	virtual ~QCWealthFactor();
};

#endif //QWEALTHFACTOR_H

