#include "QCClampedSpline.h"
#include <math.h>

double QCClampedSpline::interpolateAt(long value)
{
	long i = index(value);
	long temp = _curve->getLength();
	double t = value - _curve->getValuesAt(i).first;
	double a = _curve->getValuesAt(i).second;
	return a + _b[i] * t + _c[i] * pow(t, 2.0) + _d[i] * pow(t, 3.0);
}

double QCClampedSpline::derivativeAt(long value)
{
	long i = index(value);
	double t = value - _curve->getValuesAt(i).first;
	return _b[i] + 2.0 * _c[i] * t + 3.0 * _d[i] * pow(t, 2.0);
}

double QCClampedSpline::secondDerivativeAt(long value)
{
	long i = index(value);
	double t = value - _curve->getValuesAt(i).first;
	return 2.0 * _c[i] + 6.0 * _d[i] * t;
}

void QCClampedSpline::setCoefficients(void)
{	
	//Se define x e y
	vector<double> x, y;
	for (long i = 0; i < _curve->getLength(); ++i)
	{
		x.push_back(_curve->getValuesAt(i).first);
		y.push_back(_curve->getValuesAt(i).second);
	}
	const signed long n = x.size() - 1;
	double fp1,fpn ;
	
	vector <double> h(n), alpha(n+1);
	vector <double> l(n+1), u(n+1), z(n+1);
	_c.resize(n+1);
	_b.resize(n+1);
	_d.resize(n+1);
    
	fp1 = (y[1] - y[0]) / (x[1] - x[0]);
	fpn = (y[n] - y[n - 1]) / (x[n] - x[n - 1]);
  
	for (int i = 0; i < n; i++)
		h.at(i) = x[i + 1] - x[i];
  
    alpha.at(0) = 3 * (y[1] - y[0]) / h[0] - 3 * fp1;
	alpha.at(n) = 3 * fpn - 3 * (y[n] - y[n - 1]) / h[n - 1];

	for (int i=1; i < n ; i++)
		alpha.at(i) = 3 * (y[i + 1] - y[i])/ h[i] - 3  * (y[i] - y[i - 1])/ h[i - 1];    
	
	l.at(0) = 2 * h[0]; 
	u.at(0) = 0.5; 
	z.at(0) = alpha[0] / l[0]; 	
	for (long i = 1; i < n ; ++i)
    {
		l.at(i) = 2 * (x[i + 1] - x[i - 1]) - h[i - 1] * u[i - 1];
		u.at(i)= h[i] / l[i];
		z.at(i) = (alpha[i] - h[i - 1] * z[i - 1]) / l[i];
	} 

	l.at(n) = h[n - 1] * (2 - u[n - 1]);
	z.at(n) = (alpha[n] - h[n - 1] * z[n - 1]) / l[n];
	_c.at(n) = z[n];
	int i, j;
	for (j = 1; j < (n + 1) ; ++j)
    {	         
		i = n - j;
		_c.at(i) = z[i] - u[i] * _c[i + 1];
		_b.at(i) = (y[i + 1] - y[i]) / h[i] - h[i] * (_c[i + 1] + 2 * _c[i]) / 3;
		_d.at(i) = (_c[i + 1] - _c[i]) /(3 * h[i]);
	}
	_c.at(n) = _b.at(n) = _d.at(n) = 0;

	x.clear(),y.clear(),h.clear(), alpha.clear(),l.clear(),u.clear(),z.clear();
}	

QCClampedSpline::~QCClampedSpline(void)
{
}
