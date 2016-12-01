#ifndef QCCOMPOUNDWF_H
#define QCCOMPOUNDWF_H

#include "QCWealthFactor.h"
class QCCompoundWf :
	public QCWealthFactor
{
public:
	QCCompoundWf();
	virtual double wf(double rate, double yf) override;
	virtual ~QCCompoundWf();
};

#endif //QCCOMPOUNDWF_H

