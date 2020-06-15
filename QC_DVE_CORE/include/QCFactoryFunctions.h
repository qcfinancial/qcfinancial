#ifndef QCFACTORYFUNCTIONS_H
#define QCFACTORYFUNCTIONS_H

#include <memory>
#include <string>

#include "asset_classes/QCYearFraction.h"
#include "asset_classes/QCWealthFactor.h"
#include "QCInterestRateLeg.h"
#include "QCDefinitions.h"
#include "time/QCDate.h"
#include "curves/QCInterestRateCurve.h"
#include "asset_classes/QCCurrency.h"

using namespace std;

namespace QCFactoryFunctions
{
	shared_ptr<QCYearFraction> yfSharedPtr(const string& whichYf);
	
	shared_ptr<QCWealthFactor> wfSharedPtr(const string& whichWf);

	shared_ptr<QCInterestRate> intRateSharedPtr(double value, const string& whichYf, const string& whichWf);
	
	QCZrCpnCrvShrdPtr zrCpnCrvShrdPtr(vector<long>& tenors, vector<double>& rates,
		const string& interpolator, const string& yf, const string& wf);
	
	QCZrCpnCrvShrdPtr zrCpnCrvShrdPtr(vector<long>& tenors, vector<double>& rates,
		const string& interpolator, const string& yf, const string& wf, const string& typeCurve);

	/*!
	* Construye una tasa de interés Lin Act/360 c0n valor 0.
	*/
	shared_ptr<QCInterestRate> zeroIntRateSharedPtr();

	shared_ptr<QCInterestRateCurve> intRtCrvShrdPtr(
		vector<long>& tenors, vector<double>& rates,
		const string& interpolator, const string& wf, const string& yf,
		QCInterestRateCurve::QCTypeInterestRateCurve typeCurve);
	
	QCIntRtCrvShrdPtr discFctrCrvShrdPtr(vector<long>& tenors, vector<double>& dfs,
		const string& interpolator, const string& yf, const string& wf);

	QCInterestRateLeg buildDiscountBondLeg(
		string receivePay,				/*!< receive or pay */
		QCDate startDate,				/*!< start date */
		QCDate endDate,					/*!< end date */
		unsigned int fixingLag,			/*!< fixing lag refers to fx rate */
		unsigned int settlementLag,				/*!< settlement lag */
		vector<QCDate> fixingCalendar,	/*!< fixing calendar refers to fx rate */
		vector<QCDate> settlementCalendar,	/*!< settlement calendar */
		double notional					/*!< notional */
		);

	QCInterestRateLeg buildTimeDepositLeg(
		string receivePay,				/*!< receive or pay */
		QCDate startDate,				/*!< start date */
		QCDate endDate,					/*!< end date */
		double notional					/*!< notional */
		);

	QCInterestRateLeg buildFixedRateLeg(
		string receivePay,				//receive or pay
		QCDate startDate,				//start date
		QCDate endDate,					//end date
		vector<QCDate> calendar,		//settlement calendar
		int settlementLag,				//settlement lag
		QCInterestRateLeg::QCStubPeriod stubPeriod,		//stub period
		string periodicity,				//periodicity
		QCDate::QCBusDayAdjRules endDateAdjustment,		//end date adjustment
		QCInterestRateLeg::QCAmortization amortization,	//amortization
		vector<tuple<QCDate, double, double>> amortNotionalByDate, //amortization and notional by date
		double notional					//notional
		);

	QCInterestRateLeg buildFixedRateLeg2(
		string receivePay,				//receive or pay
		QCDate startDate,				//start date
		QCDate endDate,					//end date
		vector<QCDate> calendar,		//settlement calendar
		int settlementLag,				//settlement lag
		QCInterestRateLeg::QCStubPeriod stubPeriod,		//stub period
		string periodicity,				//periodicity
		QCDate::QCBusDayAdjRules endDateAdjustment,		//end date adjustment
		QCInterestRateLeg::QCAmortization amortization,	//amortization
		vector<tuple<QCDate, double, double>> amortNotionalByDate, //amortization and notional by date
		double notional					//notional
		);

	QCInterestRateLeg buildFixedRateLeg3(
		QCDate valueDate,			//value date
		string receivePay,			//receive or pay
		QCDate startDate,			//start date
		QCDate endDate,				//end date
		vector<QCDate> calendar,	//settlement calendar
		int settlementLag,			//settlement lag
		QCInterestRateLeg::QCStubPeriod stubPeriod,	//stub period
		string periodicity,			//periodicity
		QCDate::QCBusDayAdjRules endDateAdjustment,		//end date adjustment
		QCInterestRateLeg::QCAmortization amortization,	//amortization
		vector<tuple<QCDate, double, double>> amortNotionalByDate, //amortization and notional by date
		double notional,			//notional
		shared_ptr<QCInterestRate> intRate //interest rate
		);

	QCInterestRateLeg buildFrenchFixedRateLeg(
		QCDate valueDate,				//value date
		string receivePay,				//receive or pay
		QCDate startDate,				//start date
		QCDate endDate,					//end date
		vector<QCDate> calendar,		//settlement calendar
		int settlementLag,				//settlement lag
		QCInterestRateLeg::QCStubPeriod stubPeriod,		//stub period
		string periodicity,				//periodicity
		QCDate::QCBusDayAdjRules endDateAdjustment,		//end date adjustment
		QCInterestRateLeg::QCAmortization amortization,	//amortization
		vector<tuple<QCDate, double, double>> amortNotionalByDate, //amortization and notional by date
		double notional,					//notional
		shared_ptr<QCInterestRate> intRate	//interest rate
		);

	QCInterestRateLeg buildIcpLeg(
		string receivePay,				//receive or pay
		QCDate startDate,				//start date
		QCDate endDate,					//end date
		vector<QCDate> calendar,		//settlement calendar
		int settlementLag,				//settlement lag
		QCInterestRateLeg::QCStubPeriod stubPeriod,		//stub period
		string periodicity,				//periodicity
		QCDate::QCBusDayAdjRules endDateAdjustment,		//end date adjustment
		QCInterestRateLeg::QCAmortization amortization,	//amortization
		vector<tuple<QCDate, double, double>> amortNotionalByDate, //amortization and notional by date
		double notional									//notional
		);

	QCInterestRateLeg buildIcpLeg2(
		string receivePay,				//receive or pay
		QCDate startDate,				//start date
		QCDate endDate,					//end date
		vector<QCDate> calendar,		//settlement calendar
		int settlementLag,				//settlement lag
		QCInterestRateLeg::QCStubPeriod stubPeriod,		//stub period
		string periodicity,				//periodicity
		QCDate::QCBusDayAdjRules endDateAdjustment,		//end date adjustment
		QCInterestRateLeg::QCAmortization amortization,	//amortization
		vector<tuple<QCDate, double, double>> amortNotionalByDate, //amortization and notional by date
		double notional									//notional
		);

	QCInterestRateLeg buildIcpLeg3(
		QCDate valueDate,
		string receivePay,				//receive or pay
		QCDate startDate,				//start date
		QCDate endDate,					//end date
		vector<QCDate> calendar,		//settlement calendar
		int settlementLag,				//settlement lag
		QCInterestRateLeg::QCStubPeriod stubPeriod,		//stub period
		string periodicity,				//periodicity
		QCDate::QCBusDayAdjRules endDateAdjustment,		//end date adjustment
		QCInterestRateLeg::QCAmortization amortization,	//amortization
		double notional									//notional
		);


	QCInterestRateLeg buildFloatingRateLeg(
		string receivePay,					//receive or pay
		QCDate startDate,					//start date
		QCDate endDate,						//end date
		vector<QCDate> settleCalendar,		//settlement calendar
		int settlementLag,					//settlement lag
		QCInterestRateLeg::QCStubPeriod stubPeriod,					//stub period
		string settlePeriodicity,			//settlement periodicity
		QCDate::QCBusDayAdjRules endDateAdjustment, //end date adjustment
		QCInterestRateLeg::QCAmortization amortization,				//amortization
		vector<tuple<QCDate, double, double>> amortNotionalByDate,	//amortization and notional by end date
		int fixingLag,								//fixing lag
		QCInterestRateLeg::QCStubPeriod fixingStubPeriod,			//fixing stub period
		string fixingPeriodicity,					//fixing periodicity
		vector<QCDate> fixingCalendar,				//fixing calendar
		pair<string, string> interestRateIndexChars,	//interest rate index tenor (3M, 6M ...)	
		double notional								//notional
		);

	QCInterestRateLeg buildFloatingRateLeg2(
		string receivePay,					//receive or pay
		QCDate startDate,					//start date
		QCDate endDate,						//end date
		vector<QCDate> settleCalendar,		//settlement calendar
		int settlementLag,					//settlement lag
		QCInterestRateLeg::QCStubPeriod stubPeriod,					//stub period
		string settlePeriodicity,			//settlement periodicity
		QCDate::QCBusDayAdjRules endDateAdjustment, //end date adjustment
		QCInterestRateLeg::QCAmortization amortization,				//amortization
		vector<tuple<QCDate, double, double>> amortNotionalByDate,	//amortization and notional by end date
		int fixingLag,								//fixing lag
		QCInterestRateLeg::QCStubPeriod fixingStubPeriod,			//fixing stub period
		string fixingPeriodicity,					//fixing periodicity
		vector<QCDate> fixingCalendar,				//fixing calendar
		pair<string, string> interestRateIndexChars,	//interest rate index tenor (3M, 6M ...)	
		double notional								//notional
		);

	QCInterestRateLeg buildFloatingRateLeg3(
		QCDate valueDate,
		string receivePay,					//receive or pay
		QCDate startDate,					//start date
		QCDate endDate,						//end date
		vector<QCDate> settleCalendar,		//settlement calendar
		int settlementLag,					//settlement lag
		QCInterestRateLeg::QCStubPeriod stubPeriod,					//stub period
		string settlePeriodicity,			//settlement periodicity
		QCDate::QCBusDayAdjRules endDateAdjustment, //end date adjustment
		QCInterestRateLeg::QCAmortization amortization,				//amortization
		int fixingLag,								//fixing lag
		QCInterestRateLeg::QCStubPeriod fixingStubPeriod,			//fixing stub period
		string fixingPeriodicity,					//fixing periodicity
		vector<QCDate> fixingCalendar,				//fixing calendar
		pair<string, string> interestRateIndexChars,	//interest rate index tenor (3M, 6M ...)	
		double notional								//notional
		);

};

#endif //QCFACTORYFUNCTIONS_H