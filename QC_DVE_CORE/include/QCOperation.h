#ifndef QCOPERATION_H
#define QCOPERATION_H

#include <vector>
#include <memory>

#include "QCInterestRatePayoff.h"

using namespace std;

/*!
* @brief Por ahora el único propósito de esta clase es proveer un mínimo de estructura para
* modelar operaciones a partir de vectores de QCInterestRatePayoff.
*/
class QCOperation
{
public:
	QCOperation()
	{
	}

	virtual double marketValue() = 0;

	virtual ~QCOperation();
};

#endif //QCOPERATION_H

