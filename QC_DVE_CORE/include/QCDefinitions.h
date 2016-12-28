#ifndef QCDEFINITIONS_H
#define QCDEFINITIONS_H

#include <memory>
#include <vector>
#include <utility>
#include <tuple>
#include <map>
#include "QCDate.h"

class QCWealthFactor;
class QCInterestRatePayoff;
class QCZeroCouponCurve;
class QCDiscountFactorCurve;
class QCInterestRate;
class QCInterestRateCurve;
class QCYearFraction;

using namespace std;

/*!
* Enumera las alternativas al dar de alta una curva cero cupon
*/
enum QCZCCRepresentation
{
	qcZCCInterestRate,			/*!< ZCC se representa con tasas */
	qcZCCDiscountFactor,		/*!< ZCC se representa con df */
	qcZCCWealthFactor			/*!< ZCC se representa con wf */
};

/*!
* Shared pointer de QCInterestRateCurve
*/
typedef shared_ptr<QCInterestRateCurve> QCIntRtCrvShrdPtr;

/*!
* Shared pointer de QCZeroCouponCurve
*/
typedef shared_ptr<QCZeroCouponCurve> QCZrCpnCrvShrdPtr;

/*!
* Shared pointer de QCDiscountFactorCurve
*/
typedef shared_ptr<QCDiscountFactorCurve> QCDiscFctrCrvShrdPtr;

/*!
* Shared pointer de QCInterestRatePayoff
*/
typedef  shared_ptr<QCInterestRatePayoff> QCIntrstRtPffShrdPtr;

/*!
* Shared pointer de map<QCDate, double>
*/
typedef shared_ptr<map<QCDate, double>> QCTimeSeriesShrdPtr;

/*!
* Shared pointer de QCWealthFactor
*/
typedef shared_ptr<QCWealthFactor> QCWlthFctrShrdPtr;

/*!
* Shared pointer de QCYearFraction
*/
typedef shared_ptr<QCYearFraction> QCYrFrctnShrdPtr;

/*!
* Shared pointer de QCInterestRate
*/
typedef shared_ptr<QCInterestRate> QCIntrstRtShrdPtr;

/*!
* Representa en una tupla los elementos de una pata fija que
* permiten construir el QCInterestRateLeg asociado
*/
typedef tuple<QCInterestRate> QCFixedLegTuple;
#endif //QCDEFINITIONS_H