#ifndef QCCONTINOUSWF_H
#define QCCONTINOUSWF_H

#include "QCWealthFactor.h"
class QCContinousWf :
	public QCWealthFactor
{
public:
	QCContinousWf();
	virtual double wf(double rate, double yf) override;
	virtual double rate(double wf, double yf) override;
	~QCContinousWf();
};

#endif

