#ifndef QCLINEARINTERPOLATOR_H
#define QCLINEARINTERPOLATOR_H

#include "QCInterpolator.h"

class QCLinearInterpolator :
	public QCInterpolator
{
public:
	QCLinearInterpolator(shared_ptr<QCCurve> curve);

	virtual double interpolateAt(double value);
	virtual double derivativeAt(double value);
	virtual double secondDerivativeAt(double value);

	virtual ~QCLinearInterpolator();

private:
	vector<double> _derivatives;
};

#endif //QCLINEARINTERPOLATOR_H