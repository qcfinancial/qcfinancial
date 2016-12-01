#ifndef QCCLAMPEDSPLINE_H
#define QCCLAMPEDSPLINE_H

#include "QCCurve.h"
#include "QCInterpolator.h"

class QCClampedSpline : public QCInterpolator
{
public:
	//Constructors
	QCClampedSpline(shared_ptr<QCCurve> curva) : QCInterpolator(curva)
	{
		setCoefficients();
	};

	//Methods
	double interpolateAt(double value);
	double derivativeAt(double value);
	double secondDerivativeAt(double value);

	//Destructor
	~QCClampedSpline(void);

private:
	vector<double> _b, _c, _d;		//Coeficientes para el spline
	void setCoefficients(void);		//Calcula los coeficientes del spline
};

#endif //QCCLAMPEDSPLINE_H

