#ifndef QCINTERPOLATOR_H
#define QCINTERPOLATOR_H

#include "QCCurve.h"
#include <memory>

class QCInterpolator
{
public:
	//Constructors
	QCInterpolator(shared_ptr<QCCurve> curva) : _curva(curva) {};
	
	//Other
	virtual double interpolateAt(double value) = 0;
	virtual double derivativeAt(double value) = 0;
	virtual double secondDerivativeAt(double value) = 0;

	//Destructors
	virtual ~QCInterpolator(void);

protected:
	shared_ptr<QCCurve> _curva;
	long index(double arg);	//Se va usar en cualquier implementacion especifica

private:

};

#endif //QCINTERPOLATOR_H


