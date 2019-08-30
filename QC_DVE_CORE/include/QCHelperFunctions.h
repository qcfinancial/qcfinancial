#ifndef QCHELPERFUNCTIONS_H
#define QCHELPERFUNCTIONS_H

#include <memory>
#include <string>

#include "QCDefinitions.h"
#include "time/QCDate.h"
#include "QCInterestRateLeg.h"
#include "curves/QCInterestRateCurve.h"

using namespace std;

namespace QCHelperFunctions
{
	enum QCZeroOne
	{
		qcZero = 0,
		qcOne = 1
	};

	long tenor(string periodicity);
	int lagToInt(string lag);
	void lowerCase(string& word);
	QCZeroOne boolToZeroOne(bool value);
	QCDate::QCBusDayAdjRules stringToQCBusDayAdjRule(string rule);
	QCInterestRateLeg::QCStubPeriod stringToQCStubPeriod(string stubPeriod);
	QCInterestRateLeg::QCAmortization stringToQCAmortization(string amortization);
	QCInterestRateCurve::QCTypeInterestRateCurve stringToQCIntRateCurve(string typeCurve);
}


#endif //QCHELPERFUNCTIONS_H
