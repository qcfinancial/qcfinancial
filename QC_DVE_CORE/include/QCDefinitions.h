#ifndef QCDEFINITIONS_H
#define QCDEFINITIONS_H

#include <memory>
#include <vector>
#include <utility>
#include <tuple>
#include <map>

#include "QCDate.h"
class QCWealthFactor;
class QCInterestRateLeg;
class QCInterestRatePayoff;
class QCZeroCouponCurve;
class QCInterestRate;
class QCYearFraction;

using namespace std;

/*!
* Enumera las componentes de un QCInterestRatePeriod
*/
enum InterestRatePeriodElement
{
	intRtPrdElmntInitialAccrtn, /*!< Disposicion inicial del periodo */
	intRtPrdElmntAcctrnIsCshflw,/*!< Indica si la disposicion inicial es flujo */
	intRtPrdElmntFinalAmrtztn,	/*!< Amortizacion final del periodo */
	inRtPrdElmntAmrtztnIsCshflw,/*!< Indica si la amortizacion final es flujo */
	intRtPrdElmntNotional,		/*!< Nocional vigente del periodo */
	intRtPrdElmntStartDate,		/*!< Fecha de inicio del periodo */
	intRtPrdElmntEndDate,		/*!< Fecha final del periodo */
	intRtPrdElmntSettlmntDate	/*!< Fecha de pago del flujo */
};

/*!
* Representa las componentes esenciales de un periodo de un
* instrumento de tasa de interes. El orden es: disposicion, esDispFlujo,
* amortizacion, esAmortFlujo, nocional, fechaInicio, fechaFinal, fechaPago.
*/
typedef tuple<double, bool, double, bool, double,
	QCDate, QCDate, QCDate> QCInterestRatePeriod;

/*!
* Vector de QCInterestRatePeriod. Objeto base de un QCInterestRateLeg.
*/
typedef vector<QCInterestRatePeriod> QCInterestRatePeriods;

/*!
* Shared pointer de QCInterestRatePeriods
*/
typedef shared_ptr<QCInterestRatePeriods> QCIntrstRtPrdsShrdPntr;

/*!
* Shared pointer de QCZeroCouponCurve
*/
typedef shared_ptr<QCZeroCouponCurve> QCZrCpnCrvShrdPtr;

/*!
* Shared pointer de QCInterestRateLeg
*/
typedef  shared_ptr<QCInterestRateLeg> QCIntrstRtLgShrdPtr;

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

#endif //QCDEFINITIONS_H