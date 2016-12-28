#ifndef QCLOGLINEARINTERPOLATOR_H
#define QCLOGLINEARINTERPOLATOR_H

#include "QCInterpolator.h"

class QCLogLinearInterpolator :
	public QCInterpolator
{
public:
	QCLogLinearInterpolator(shared_ptr<QCCurve<long>> curve);

	virtual double interpolateAt(long value) override;
	virtual double derivativeAt(long value) override;
	virtual double secondDerivativeAt(long value) override;

	virtual ~QCLogLinearInterpolator() override;

private:
	vector<double> _derivatives;
};

#endif //QCLOGLINEARINTERPOLATOR_H