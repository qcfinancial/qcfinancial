#ifndef QCLINEARWF_H
#define QCLINEARWF_H

#include "QCWealthFactor.h"

class QCLinearWf :
	public QCWealthFactor
{
public:
	QCLinearWf();
	virtual double wf(double rate, double yf) override;
	~QCLinearWf();
};

#endif //QCLINEARWF_H

