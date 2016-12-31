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

QCInterestRateLeg::QCStubPeriod QCHelperFunctions::stringToQCStubPeriod(string stubPeriod)
{
	if (stubPeriod == "NO")
		return QCInterestRateLeg::qcNoStubPeriod;
	if (stubPeriod == "CORTO FINAL")
		return QCInterestRateLeg::qcShortBack;
	if (stubPeriod == "LARGO FINAL")
		return QCInterestRateLeg::qcLongBack;
	if (stubPeriod == "CORTO INICIO")
		return QCInterestRateLeg::qcShortFront;
	if (stubPeriod == "LARGO INICIO")
		return QCInterestRateLeg::qcLongFront;
	return QCInterestRateLeg::qcNoStubPeriod;
}

QCInterestRateLeg::QCAmortization QCHelperFunctions::stringToQCAmortization(string amortization)
{
	if (amortization == "BULLET")
		return QCInterestRateLeg::qcBulletAmort;
	if (amortization == "CONSTANT")
		return QCInterestRateLeg::qcConstantAmort;
	if (amortization == "CUSTOM")
		return QCInterestRateLeg::qcCustomAmort;
	return QCInterestRateLeg::qcBulletAmort;
}

QCInterestRateCurve::QCTypeInterestRateCurve QCHelperFunctions::stringToQCIntRateCurve(string typeCurve)
{
	if (typeCurve == "ZEROCPN")
		return QCInterestRateCurve::qcZeroCouponCurve;
	if (typeCurve == "DISCFCTR")
		return QCInterestRateCurve::qcDiscountFactorCurve;
	if (typeCurve == "PROJCRV")
		return QCInterestRateCurve::qcProjectingCurve;
	return QCInterestRateCurve::qcZeroCouponCurve;
}