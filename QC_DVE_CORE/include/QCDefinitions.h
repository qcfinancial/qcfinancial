#ifndef QCDEFINITIONS_H
#define QCDEFINITIONS_H

#include <memory>
#include <vector>
#include <utility>
#include <tuple>
#include <map>

class QCDate;
class QCInterestRateLeg;
//class QCInterestRatePayoff
class QCZeroCouponCurve;

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
* instrumento de tasa de interes
*/
typedef tuple<double, QCDate, QCDate, QCDate> QCInterestRatePeriod;

/*!
* Vector de QCInterestRatePeriod. Objeto base de un QCInteterestRateLeg.
*/
typedef tuple<double, bool, double, bool, double, 
	          QCDate, QCDate, QCDate> QCInterestRatePeriods;

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
typedef  QCInterestRateLeg QCIntrstRtLgShrdPtr;

/*!
* Shared pointer de map<QCDate, double>
*/
typedef shared_ptr<map<QCDate, double>> QCTimeSeriesShrdPtr;

#endif //QCDEFINITIONS_H