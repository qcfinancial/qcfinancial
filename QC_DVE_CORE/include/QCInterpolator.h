#ifndef QCINTERPOLATOR_H
#define QCINTERPOLATOR_H

#include "QCCurve.h"
#include <memory>

//Esta es una clase base abstracta.

class QCInterpolator
{
public:
	//Constructors
	QCInterpolator(shared_ptr<QCCurve<long>> curve);
	
	//Other
	virtual double interpolateAt(long value) { return 0.0; }
	virtual double derivativeAt(long value) { return 0.0; }
	virtual double secondDerivativeAt(long value) { return 0.0; }
	double rateDerivativeAt(unsigned int rateIndex);
	long getLength();

	//Destructors
	virtual ~QCInterpolator(void);

protected:
	shared_ptr<QCCurve<long>> _curve;
	long index(long arg);	//Se va usar en cualquier implementacion especifica
	vector<double> _derivatives;
};

#endif //QCINTERPOLATOR_H


