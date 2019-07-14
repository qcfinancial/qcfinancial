#include <algorithm>
#include <locale>

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
	else if (symbol == "d")
	{
		return stoi(periodicity.substr(0, periodicity.size() - 1));
	}
	else if (symbol == "w")
	{
		return stoi(periodicity.substr(0, periodicity.size() - 1));
	}
	else
	{
		return 0;
	}
}

int QCHelperFunctions::lagToInt(string lag)
{
	QCHelperFunctions::lowerCase(lag);
	if (string{ lag.back() } != "d")
		return 0;
	lag.pop_back();
	return stoi(lag);
}

void QCHelperFunctions::lowerCase(string& word)
{
	std::transform(word.begin(), word.end(), word.begin(), ::tolower);
}

QCHelperFunctions::QCZeroOne QCHelperFunctions::boolToZeroOne(bool value)
{
	if (value) return QCHelperFunctions::qcOne;
	return QCHelperFunctions::qcZero;
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
	if (stubPeriod == "LARGO INICIO 2")
		return QCInterestRateLeg::qcLongFront2;
	if (stubPeriod == "LARGO INICIO 3")
		return QCInterestRateLeg::qcLongFront3;
	if (stubPeriod == "LARGO INICIO 4")
		return QCInterestRateLeg::qcLongFront4;
	if (stubPeriod == "LARGO INICIO 5")
		return QCInterestRateLeg::qcLongFront5;
	if (stubPeriod == "LARGO INICIO 6")
		return QCInterestRateLeg::qcLongFront6;
	if (stubPeriod == "LARGO INICIO 7")
		return QCInterestRateLeg::qcLongFront7;
	if (stubPeriod == "LARGO INICIO 8")
		return QCInterestRateLeg::qcLongFront8;
	if (stubPeriod == "LARGO INICIO 9")
		return QCInterestRateLeg::qcLongFront9;
	if (stubPeriod == "LARGO INICIO 10")
		return QCInterestRateLeg::qcLongFront10;
	if (stubPeriod == "LARGO INICIO 11")
		return QCInterestRateLeg::qcLongFront11;
	if (stubPeriod == "LARGO INICIO 12")
		return QCInterestRateLeg::qcLongFront12;
	if (stubPeriod == "LARGO INICIO 120")
		return QCInterestRateLeg::qcLongFront120;
	if (stubPeriod == "LARGO INICIO 121")
		return QCInterestRateLeg::qcLongFront121;
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
	if (amortization == "FRENCH")
		return QCInterestRateLeg::qcFrenchAmort;
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