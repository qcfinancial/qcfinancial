#ifndef QCPASTFIXING_H
#define QCPASTFIXING_H

#include <vector>

#include "QCDate.h"

using namespace std;

class QCPastFixing
{
public:
	QCPastFixing();
	virtual void pastFixing(QCDate& startDate, QCDate& endDate, vector<double>& rates) = 0;
	~QCPastFixing();
};

#endif //QCPASTFIXING_H

 