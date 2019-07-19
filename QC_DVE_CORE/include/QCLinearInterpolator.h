#ifndef QCLINEARINTERPOLATOR_H
#define QCLINEARINTERPOLATOR_H

#include "QCInterpolator.h"

class QCLinearInterpolator :
	public QCInterpolator
{
public:
	QCLinearInterpolator(shared_ptr<QCCurve<long>> curve);

	virtual double interpolateAt(long value) override;
	virtual double derivativeAt(long value) override;
	virtual double secondDerivativeAt(long value) override;

	virtual ~QCLinearInterpolator() override;

};

#endif //QCLINEARINTERPOLATOR_H