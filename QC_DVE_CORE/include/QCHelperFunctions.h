#ifndef QCHELPERFUNCTIONS_H
#define QCHELPERFUNCTIONS_H

#include <memory>

#include "QCDefinitions.h"
#include "QCDate.h"
#include "QCInterestRateCurve.h"

using namespace std;

namespace QCHelperFunctions
{
	long tenor(string periodicity);
	void lowerCase(string& word);
	QCDate::QCBusDayAdjRules stringToQCBusDayAdjRule(string rule);
	QCInterestRateCurve::QCTypeInterestRateCurve stringToQCIntRateCurve(string typeCurve);
}


#endif //QCHELPERFUNCTIONS_H
