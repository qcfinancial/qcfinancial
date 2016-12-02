#ifndef QCINTERPOLATOR_H
#define QCINTERPOLATOR_H

#include "QCCurve.h"
#include <memory>

//Esta es una clase base abstracta.

class QCInterpolator
{
public:
	//Constructors
	QCInterpolator(shared_ptr<QCCurve> curve);
	
	//Other
	virtual double interpolateAt(double value) = 0;
	virtual double derivativeAt(double value) = 0;
	virtual double secondDerivativeAt(double value) = 0;

	//Destructors
	virtual ~QCInterpolator(void);

protected:
	shared_ptr<QCCurve> _curve;
	long index(double arg);	//Se va usar en cualquier implementacion especifica

private:

};

#endif //QCINTERPOLATOR_H


