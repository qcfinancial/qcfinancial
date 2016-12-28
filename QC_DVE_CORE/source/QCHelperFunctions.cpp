#include <algorithm>

#include "QCHelperFunctions.h"

long QCHelperFunctions::tenor(string periodicity)
{
	QCHelperFunctions::lowerCase(periodicity);
	string symbol = periodicity.substr(periodicity.size() - 1, 1);
	if (symbol == "y")
	{
		return stoi(periodicity.substr(0, periodicity.size() - 1)) * 12;
	}
	else if (symbol == "m")
	{
		return stoi(periodicity.substr(0, periodicity.size() - 1));
	}
	else
	{
		return 0;
	}
}

void QCHelperFunctions::lowerCase(string& word)
{
	transform(word.begin(), word.end(), word.begin(), tolower);
}

QCDate::QCBusDayAdjRules QCHelperFunctions::stringToQCBusDayAdjRule(string rule)
{
	if (rule == "NO")
		return QCDate::qcNo;
	if (rule == "FOLLOW")
		return QCDate::qcFollow;
	if (rule == "MOD_FOLLOW")
		return QCDate::qcModFollow;
	if (rule == "PREV")
		return QCDate::qcPrev;
	if (rule == "MOD_PREV")
		return QCDate::qcModPrev;
	return QCDate::qcNo;
}

QCInterestRateCurve::QCTypeInterestRateCurve QCHelperFunctions::stringToQCIntRateCurve(string typeCurve)
{
	if (typeCurve == "ZEROCPN")
		return QCInterestRateCurve::qcZeroCouponCurve;
	if (typeCurve == "DISCFCTR")
		return QCInterestRateCurve::qcDiscountFactorCurve;
	if (typeCurve == "PROJCRV")
		return QCInterestRateCurve::qcProjectingCurve;
}