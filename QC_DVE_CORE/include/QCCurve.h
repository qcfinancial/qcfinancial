#ifndef QCCURVE_H
#define QCCURVE_H

#include <vector>
#include <utility>

using namespace std;

class QCCurve
{
public:
	//Constructors
	QCCurve(void);
	QCCurve(vector<double>& abscissa, vector<double>& ordinate);

	//Getter
	pair<double, double> getValuesAt(long position);
	long getLength();

	//Destructor
	virtual ~QCCurve(void);

protected:
	vector<pair<double, double>> _values;	//En esta variable se guardan los datos de la curva
};

#endif //QCCURVE_H

