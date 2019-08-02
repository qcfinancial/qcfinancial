#ifndef LEGFACTORY_H
#define LEGFACTORY_H

#include<memory>

#include "cashflows/Cashflow.h"
#include "cashflows/FixedRateCashflow.h"
#include "Leg.h"
#include "asset_classes/InterestRateIndex.h"
#include "asset_classes/FXRateIndex.h"
#include "asset_classes/Tenor.h"

#include "QCInterestRateLeg.h"
#include "time/QCBusinessCalendar.h"
#include "asset_classes/QCInterestRate.h"

namespace QCode
{
	namespace Financial
	{
		/**
		 * @class	LegFactory
		 *
		 * @brief	A leg factory. This class provides many static methods that allow the user to build
		 * 			often used Legs (fixed rate legs, floating rate legs etc.).
		 *
		 * @author	Alvaro Diaz V.
		 * @date	07/07/2018
		 */
		class LegFactory
		{
		public:

			/**
			 * @enum	TypeOfCashflow
			 *
			 * @brief	Values that represent type of cashflows
			 */
			enum TypeOfCashflow
			{

				///< An enum constant representing the fixed rate cashflow option
				fixedRateCashflow,

				///< An enum constant representing the fixed rate cashflow option
				fixedRateCashflow2,

				///< An enum constant representing the ibor cashflow option
				iborCashflow,

				///< An enum constant representing the icp clp cashflow option
				icpClpCashflow,

				///< An enum constant representing the icp clf cashflow option
				icpClfCashflow
			};

			/**
			 * @fn	static Leg LegFactory::buildBulletFixedRateLeg( RecPay recPay, QCDate startDate, QCDate endDate, QCDate::QCBusDayAdjRules endDateAdjustment, Tenor settlementPeriodicity, QCInterestRateLeg::QCStubPeriod settlementStubPeriod, QCBusinessCalendar settlementCalendar, unsigned int settlementLag, double notional, bool doesAmortize, QCInterestRate rate, std::shared_ptr<QCCurrency> currency, bool forBonds = false);
			 *
			 * @brief	Builds bullet fixed rate leg
			 *
			 * @author	Alvaro Diaz V.
			 * @date	07/07/2018
			 *
			 * @param	recPay				 	Indicates if the cashflows in the leg are received or payed.
			 * @param	startDate			 	The start date.
			 * @param	endDate				 	The end date.
			 * @param	endDateAdjustment	 	The end date adjustment.
			 * @param	settlementPeriodicity	The settlement periodicity.
			 * @param	settlementStubPeriod 	The settlement stub period.
			 * @param	settlementCalendar   	The settlement calendar.
			 * @param	settlementLag		 	The settlement lag.
			 * @param	notional			 	The notional.
			 * @param	doesAmortize		 	True to does amortize.
			 * @param	rate				 	The rate.
			 * @param	currency			 	The currency.
			 * @param	forBonds			 	(Optional) True to for bonds. This forces settlement
			 * 									date to coincide with end date for each period.
			 * 									This allows the correct calculation of present value
			 * 									using yield to maturity and the usual fixed rate market
			 * 									conventions.
			 *
			 * @return	A Leg.
			 */
			static Leg buildBulletFixedRateLeg(
				RecPay recPay,
				QCDate startDate,
				QCDate endDate,
				QCDate::QCBusDayAdjRules endDateAdjustment,
				Tenor settlementPeriodicity,
				QCInterestRateLeg::QCStubPeriod settlementStubPeriod,
				QCBusinessCalendar settlementCalendar,
				unsigned int settlementLag,
				double notional,
				bool doesAmortize,
				QCInterestRate rate,
				std::shared_ptr<QCCurrency> currency,
				bool forBonds = false);

			/**
			 * @fn	static Leg LegFactory::buildBulletFixedRateLeg2( RecPay recPay, QCDate startDate, QCDate endDate, QCDate::QCBusDayAdjRules endDateAdjustment, Tenor settlementPeriodicity, QCInterestRateLeg::QCStubPeriod settlementStubPeriod, QCBusinessCalendar settlementCalendar, unsigned int settlementLag, double notional, bool doesAmortize, QCInterestRate rate, std::shared_ptr<QCCurrency> currency, bool forBonds = false);
			 *
			 * @brief	Builds bullet fixed rate leg 2 (cashflows are of type FixedRateCashflow2)
			 *
			 * @author	A Diaz V
			 * @date	21-03-2019
			 *
			 * @param	recPay				 	Indicates if the cashflows in the leg are received or payed.
			 * @param	startDate			 	The start date.
			 * @param	endDate				 	The end date.
			 * @param	endDateAdjustment	 	The end date adjustment.
			 * @param	settlementPeriodicity	The settlement periodicity.
			 * @param	settlementStubPeriod 	The settlement stub period.
			 * @param	settlementCalendar   	The settlement calendar.
			 * @param	settlementLag		 	The settlement lag.
			 * @param	notional			 	The notional.
			 * @param	doesAmortize		 	True to does amortize.
			 * @param	rate				 	The rate.
			 * @param	currency			 	The currency.
			 * @param	forBonds			 	(Optional) True to for bonds. This forces settlement
			 * 									date to coincide with end date for each period.
			 * 									This allows the correct calculation of present value
			 * 									using yield to maturity and the usual fixed rate market
			 * 									conventions.
			 *
			 * @returns	A Leg.
			 */
			static Leg buildBulletFixedRateLeg2(
				RecPay recPay,
				QCDate startDate,
				QCDate endDate,
				QCDate::QCBusDayAdjRules endDateAdjustment,
				Tenor settlementPeriodicity,
				QCInterestRateLeg::QCStubPeriod settlementStubPeriod,
				QCBusinessCalendar settlementCalendar,
				unsigned int settlementLag,
				double notional,
				bool doesAmortize,
				QCInterestRate rate,
				std::shared_ptr<QCCurrency> currency,
				bool forBonds = false);
			/**
			 * @fn	static Leg LegFactory::buildBulletFixedRateMultiCurrencyLeg( RecPay recPay, QCDate startDate, QCDate endDate, QCDate::QCBusDayAdjRules endDateAdjustment, Tenor settlementPeriodicity, QCInterestRateLeg::QCStubPeriod settlementStubPeriod, QCBusinessCalendar settlementCalendar, unsigned int settlementLag, double notional, bool doesAmortize, QCInterestRate rate, std::shared_ptr<QCCurrency> notionalCurrency, std::shared_ptr<QCCurrency> settlementCurrency, std::shared_ptr<FXRateIndex> fxRateIndex, unsigned int fxRateIndexFixingLag, bool forBonds = false);
			 *
			 * @brief	Builds bullet fixed rate multi currency leg
			 *
			 * @author	A Diaz V
			 * @date	05-03-2019
			 *
			 * @param	recPay				 	Indicates if the cashflows in the leg are received or payed.
			 * @param	startDate			 	The start date.
			 * @param	endDate				 	The end date.
			 * @param	endDateAdjustment	 	The end date adjustment.
			 * @param	settlementPeriodicity	The settlement periodicity.
			 * @param	settlementStubPeriod 	The settlement stub period.
			 * @param	settlementCalendar   	The settlement calendar.
			 * @param	settlementLag		 	The settlement lag.
			 * @param	notional			 	The notional.
			 * @param	doesAmortize		 	True to does amortize.
			 * @param	rate				 	The rate.
			 * @param	notionalCurrency	 	The notional currency.
			 * @param	settlementCurrency   	The settlement currency.
			 * @param	fxRateIndex			 	FX rate index used to convert cashflow to settlement currency.
			 * @param	fxRateIndexFixingLag 	FX rate index fixing lag (with respect to settlement date).
			 * @param	forBonds			 	(Optional) True to for bonds. This forces settlement
			 * 									date to coincide with end date for each period.
			 * 									This allows the correct calculation of present value
			 * 									using yield to maturity and the usual fixed rate market
			 * 									conventions.
			 *
			 * @returns	A Leg.
			 */
			static Leg buildBulletFixedRateMultiCurrencyLeg(
				RecPay recPay,
				QCDate startDate,
				QCDate endDate,
				QCDate::QCBusDayAdjRules endDateAdjustment,
				Tenor settlementPeriodicity,
				QCInterestRateLeg::QCStubPeriod settlementStubPeriod,
				QCBusinessCalendar settlementCalendar,
				unsigned int settlementLag,
				double notional,
				bool doesAmortize,
				QCInterestRate rate,
				std::shared_ptr<QCCurrency> notionalCurrency,
				std::shared_ptr<QCCurrency> settlementCurrency,
				std::shared_ptr<FXRateIndex> fxRateIndex,
				unsigned int fxRateIndexFixingLag,
				bool forBonds = false);


			/**
			 * @fn	static Leg LegFactory::buildCustomAmortFixedRateLeg( RecPay recPay, QCDate startDate, QCDate endDate, QCDate::QCBusDayAdjRules endDateAdjustment, Tenor settlementPeriodicity, QCInterestRateLeg::QCStubPeriod settlementStubPeriod, QCBusinessCalendar settlementCalendar, unsigned int settlementLag, CustomNotionalAmort notionalAndAmort, bool doesAmortize, QCInterestRate rate, std::shared_ptr<QCCurrency> currency);
			 *
			 * @brief	Builds custom amort fixed rate leg
			 *
			 * @author	Alvaro Diaz V.
			 * @date	07/07/2018
			 *
			 * @param	recPay				 	Indicates if the cashflows in the leg are received or payed.
			 * @param	startDate			 	The start date.
			 * @param	endDate				 	The end date.
			 * @param	endDateAdjustment	 	The end date adjustment.
			 * @param	settlementPeriodicity	The settlement periodicity.
			 * @param	settlementStubPeriod 	The settlement stub period.
			 * @param	settlementCalendar   	The settlement calendar.
			 * @param	settlementLag		 	The settlement lag.
			 * @param	notionalAndAmort	 	The notional and amort.
			 * @param	doesAmortize		 	True to does amortize.
			 * @param	rate				 	The rate.
			 * @param	currency			 	The currency.
			 *
			 * @return	A Leg.
			 */
			static Leg buildCustomAmortFixedRateLeg(
				RecPay recPay,
				QCDate startDate,
				QCDate endDate,
				QCDate::QCBusDayAdjRules endDateAdjustment,
				Tenor settlementPeriodicity,
				QCInterestRateLeg::QCStubPeriod settlementStubPeriod,
				QCBusinessCalendar settlementCalendar,
				unsigned int settlementLag,
				CustomNotionalAmort notionalAndAmort,
				bool doesAmortize,
				QCInterestRate rate,
				std::shared_ptr<QCCurrency> currency); 

			/**
			 * @fn	static Leg LegFactory::buildCustomAmortFixedRateLeg2( RecPay recPay, QCDate startDate, QCDate endDate, QCDate::QCBusDayAdjRules endDateAdjustment, Tenor settlementPeriodicity, QCInterestRateLeg::QCStubPeriod settlementStubPeriod, QCBusinessCalendar settlementCalendar, unsigned int settlementLag, CustomNotionalAmort notionalAndAmort, bool doesAmortize, QCInterestRate rate, std::shared_ptr<QCCurrency> currency);
			 *
			 * @brief	Builds custom amort fixed rate leg 2
			 *
			 * @author	A Diaz V
			 * @date	22-03-2019
			 *
			 * @param	recPay				 	Indicates if the cashflows in the leg are received or payed.
			 * @param	startDate			 	The start date.
			 * @param	endDate				 	The end date.
			 * @param	endDateAdjustment	 	The end date adjustment.
			 * @param	settlementPeriodicity	The settlement periodicity.
			 * @param	settlementStubPeriod 	The settlement stub period.
			 * @param	settlementCalendar   	The settlement calendar.
			 * @param	settlementLag		 	The settlement lag.
			 * @param	notionalAndAmort	 	The notional and amort.
			 * @param	doesAmortize		 	True to does amortize.
			 * @param	rate				 	The rate.
			 * @param	currency			 	The currency.
			 *
			 * @returns	A Leg.
			 */
			static Leg buildCustomAmortFixedRateLeg2(
				RecPay recPay,
				QCDate startDate,
				QCDate endDate,
				QCDate::QCBusDayAdjRules endDateAdjustment,
				Tenor settlementPeriodicity,
				QCInterestRateLeg::QCStubPeriod settlementStubPeriod,
				QCBusinessCalendar settlementCalendar,
				unsigned int settlementLag,
				CustomNotionalAmort notionalAndAmort,
				bool doesAmortize,
				QCInterestRate rate,
				std::shared_ptr<QCCurrency> currency);

			/**
			 * @fn	static Leg LegFactory::buildBulletIborLeg( RecPay recPay, QCDate startDate, QCDate endDate, QCDate::QCBusDayAdjRules endDateAdjustment, Tenor settlementPeriodicity, QCInterestRateLeg::QCStubPeriod settlementStubPeriod, QCBusinessCalendar settlementCalendar, unsigned int settlementLag, Tenor fixingPeriodicity, QCInterestRateLeg::QCStubPeriod fixingStubPeriod, QCBusinessCalendar fixingCalendar, unsigned int fixingLag, std::shared_ptr<InterestRateIndex> index, double notional, bool doesAmortize, std::shared_ptr<QCCurrency> currency, double spread, double gearing);
			 *
			 * @brief	Builds bullet ibor leg
			 *
			 * @author	Alvaro Diaz V.
			 * @date	07/07/2018
			 *
			 * @param	recPay				 	Indicates if the cashflows in the leg are received or payed.
			 * @param	startDate			 	The start date.
			 * @param	endDate				 	The end date.
			 * @param	endDateAdjustment	 	The end date adjustment.
			 * @param	settlementPeriodicity	The settlement periodicity.
			 * @param	settlementStubPeriod 	The settlement stub period.
			 * @param	settlementCalendar   	The settlement calendar.
			 * @param	settlementLag		 	The settlement lag.
			 * @param	fixingPeriodicity	 	The fixing periodicity.
			 * @param	fixingStubPeriod	 	The fixing stub period.
			 * @param	fixingCalendar		 	The fixing calendar.
			 * @param	fixingLag			 	The fixing lag.
			 * @param	index				 	Zero-based index of the.
			 * @param	notional			 	The notional.
			 * @param	doesAmortize		 	True to does amortize.
			 * @param	currency			 	The currency.
			 * @param	spread				 	The spread.
			 * @param	gearing				 	The gearing.
			 *
			 * @returns	A Leg.
			 */
			static Leg buildBulletIborLeg(
				RecPay recPay,
				QCDate startDate,
				QCDate endDate,
				QCDate::QCBusDayAdjRules endDateAdjustment,
				Tenor settlementPeriodicity,
				QCInterestRateLeg::QCStubPeriod settlementStubPeriod,
				QCBusinessCalendar settlementCalendar,
				unsigned int settlementLag,
				Tenor fixingPeriodicity,
				QCInterestRateLeg::QCStubPeriod fixingStubPeriod,
				QCBusinessCalendar fixingCalendar,
				unsigned int fixingLag,
				std::shared_ptr<InterestRateIndex> index,
				double notional,
				bool doesAmortize,
				std::shared_ptr<QCCurrency> currency,
				double spread,
				double gearing);


			/**
			 * @fn	static Leg LegFactory::buildBulletIborMultiCurrencyLeg( RecPay recPay, QCDate startDate, QCDate endDate, QCDate::QCBusDayAdjRules endDateAdjustment, Tenor settlementPeriodicity, QCInterestRateLeg::QCStubPeriod settlementStubPeriod, QCBusinessCalendar settlementCalendar, unsigned int settlementLag, Tenor fixingPeriodicity, QCInterestRateLeg::QCStubPeriod fixingStubPeriod, QCBusinessCalendar fixingCalendar, unsigned int fixingLag, std::shared_ptr<InterestRateIndex> index, double notional, bool doesAmortize, std::shared_ptr<QCCurrency> currency, double spread, double gearing);
			 *
			 * @brief	Builds bullet ibor multi currency leg
			 *
			 * @author	A Diaz V
			 * @date	05-03-2019
			 *
			 * @param	recPay				 	The record pay.
			 * @param	startDate			 	The start date.
			 * @param	endDate				 	The end date.
			 * @param	endDateAdjustment	 	The end date adjustment.
			 * @param	settlementPeriodicity	The settlement periodicity.
			 * @param	settlementStubPeriod 	The settlement stub period.
			 * @param	settlementCalendar   	The settlement calendar.
			 * @param	settlementLag		 	The settlement lag.
			 * @param	fixingPeriodicity	 	The fixing periodicity.
			 * @param	fixingStubPeriod	 	The fixing stub period.
			 * @param	fixingCalendar		 	The fixing calendar.
			 * @param	fixingLag			 	The fixing lag.
			 * @param	index				 	Zero-based index of the.
			 * @param	notional			 	The notional.
			 * @param	doesAmortize		 	True to does amortize.
			 * @param	currency			 	The currency.
			 * @param	spread				 	The spread.
			 * @param	gearing				 	The gearing.
			 * @param	settlementCurrency   	The settlement currency.
			 * @param	fxRateIndex			 	FX rate index used to convert cashflow to settlement currency.
			 * @param	fxRateIndexFixingLag 	FX rate index fixing lag (with respect to settlement date).
			 *
			 * @returns	A Leg.
			 */

			static Leg buildBulletIborMultiCurrencyLeg(
				RecPay recPay,
				QCDate startDate,
				QCDate endDate,
				QCDate::QCBusDayAdjRules endDateAdjustment,
				Tenor settlementPeriodicity,
				QCInterestRateLeg::QCStubPeriod settlementStubPeriod,
				QCBusinessCalendar settlementCalendar,
				unsigned int settlementLag,
				Tenor fixingPeriodicity,
				QCInterestRateLeg::QCStubPeriod fixingStubPeriod,
				QCBusinessCalendar fixingCalendar,
				unsigned int fixingLag,
				std::shared_ptr<InterestRateIndex> index,
				double notional,
				bool doesAmortize,
				std::shared_ptr<QCCurrency> notionalCurrency,
				double spread,
				double gearing,
				std::shared_ptr<QCCurrency> settlementCurrency,
				std::shared_ptr<FXRateIndex> fxRateIndex,
				unsigned int fxRateIndexFixingLag
				);





			/**
			 * @fn	static Leg LegFactory::buildCustomAmortIborLeg( RecPay recPay, QCDate startDate, QCDate endDate, QCDate::QCBusDayAdjRules endDateAdjustment, Tenor settlementPeriodicity, QCInterestRateLeg::QCStubPeriod settlementStubPeriod, QCBusinessCalendar settlementCalendar, unsigned int settlementLag, CustomNotionalAmort notionalAndAmort, Tenor fixingPeriodicity, QCInterestRateLeg::QCStubPeriod fixingStubPeriod, QCBusinessCalendar fixingCalendar, unsigned int fixingLag, std::shared_ptr<InterestRateIndex> index, bool doesAmortize, std::shared_ptr<QCCurrency> currency, double spread, double gearing);
			 *
			 * @brief	Builds custom amort ibor leg
			 *
			 * @author	Alvaro Diaz V.
			 * @date	07/07/2018
			 *
			 * @param	recPay				 	Indicates if the cashflows in the leg are received or payed.
			 * @param	startDate			 	The start date.
			 * @param	endDate				 	The end date.
			 * @param	endDateAdjustment	 	The end date adjustment.
			 * @param	settlementPeriodicity	The settlement periodicity.
			 * @param	settlementStubPeriod 	The settlement stub period.
			 * @param	settlementCalendar   	The settlement calendar.
			 * @param	settlementLag		 	The settlement lag.
			 * @param	notionalAndAmort	 	The notional and amort.
			 * @param	fixingPeriodicity	 	The fixing periodicity.
			 * @param	fixingStubPeriod	 	The fixing stub period.
			 * @param	fixingCalendar		 	The fixing calendar.
			 * @param	fixingLag			 	The fixing lag.
			 * @param	index				 	Zero-based index of the.
			 * @param	doesAmortize		 	True to does amortize.
			 * @param	currency			 	The currency.
			 * @param	spread				 	The spread.
			 * @param	gearing				 	The gearing.
			 *
			 * @return	A Leg.
			 */
			static Leg buildCustomAmortIborLeg(
				RecPay recPay,
				QCDate startDate,
				QCDate endDate,
				QCDate::QCBusDayAdjRules endDateAdjustment,
				Tenor settlementPeriodicity,
				QCInterestRateLeg::QCStubPeriod settlementStubPeriod,
				QCBusinessCalendar settlementCalendar,
				unsigned int settlementLag,
				CustomNotionalAmort notionalAndAmort,
				Tenor fixingPeriodicity,
				QCInterestRateLeg::QCStubPeriod fixingStubPeriod,
				QCBusinessCalendar fixingCalendar,
				unsigned int fixingLag,
				std::shared_ptr<InterestRateIndex> index,
				bool doesAmortize,
				std::shared_ptr<QCCurrency> currency,
				double spread,
				double gearing);

			/**
			 * @fn	static Leg LegFactory::buildBulletIcpClpLeg( RecPay recPay, QCDate startDate, QCDate endDate, QCDate::QCBusDayAdjRules endDateAdjustment, Tenor settlementPeriodicity, QCInterestRateLeg::QCStubPeriod settlementStubPeriod, QCBusinessCalendar settlementCalendar, unsigned int settlementLag, double notional, bool doesAmortize, double spread, double gearing);
			 *
			 * @brief	Builds bullet icp clp leg
			 *
			 * @author	Alvaro Diaz V.
			 * @date	07/07/2018
			 *
			 * @param	recPay				 	Indicates if the cashflows in the leg are received or payed.
			 * @param	startDate			 	The start date.
			 * @param	endDate				 	The end date.
			 * @param	endDateAdjustment	 	The end date adjustment.
			 * @param	settlementPeriodicity	The settlement periodicity.
			 * @param	settlementStubPeriod 	The settlement stub period.
			 * @param	settlementCalendar   	The settlement calendar.
			 * @param	settlementLag		 	The settlement lag.
			 * @param	notional			 	The notional.
			 * @param	doesAmortize		 	True to does amortize.
			 * @param	spread				 	The spread.
			 * @param	gearing				 	The gearing.
			 *
			 * @return	A Leg.
			 */
			static Leg buildBulletIcpClpLeg(
				RecPay recPay,
				QCDate startDate,
				QCDate endDate,
				QCDate::QCBusDayAdjRules endDateAdjustment,
				Tenor settlementPeriodicity,
				QCInterestRateLeg::QCStubPeriod settlementStubPeriod,
				QCBusinessCalendar settlementCalendar,
				unsigned int settlementLag,
				double notional,
				bool doesAmortize,
				double spread,
				double gearing);

			/**
			 * @fn	static Leg LegFactory::buildCustomAmortIcpClpLeg( RecPay recPay, QCDate startDate, QCDate endDate, QCDate::QCBusDayAdjRules endDateAdjustment, Tenor settlementPeriodicity, QCInterestRateLeg::QCStubPeriod settlementStubPeriod, QCBusinessCalendar settlementCalendar, unsigned int settlementLag, CustomNotionalAmort notionalAndAmort, bool doesAmortize, double spread, double gearing);
			 *
			 * @brief	Builds custom amort icp clp leg
			 *
			 * @author	Alvaro Diaz V.
			 * @date	07/07/2018
			 *
			 * @param	recPay				 	Indicates if the cashflows in the leg are received or payed.
			 * @param	startDate			 	The start date.
			 * @param	endDate				 	The end date.
			 * @param	endDateAdjustment	 	The end date adjustment.
			 * @param	settlementPeriodicity	The settlement periodicity.
			 * @param	settlementStubPeriod 	The settlement stub period.
			 * @param	settlementCalendar   	The settlement calendar.
			 * @param	settlementLag		 	The settlement lag.
			 * @param	notionalAndAmort	 	The notional and amort.
			 * @param	doesAmortize		 	True to does amortize.
			 * @param	spread				 	The spread.
			 * @param	gearing				 	The gearing.
			 *
			 * @return	A Leg.
			 */
			static Leg buildCustomAmortIcpClpLeg(
				RecPay recPay,
				QCDate startDate,
				QCDate endDate,
				QCDate::QCBusDayAdjRules endDateAdjustment,
				Tenor settlementPeriodicity,
				QCInterestRateLeg::QCStubPeriod settlementStubPeriod,
				QCBusinessCalendar settlementCalendar,
				unsigned int settlementLag,
				CustomNotionalAmort notionalAndAmort,
				bool doesAmortize,
				double spread,
				double gearing);

			/**
			 * @fn	static Leg LegFactory::buildBulletIcpClfLeg( RecPay recPay, QCDate startDate, QCDate endDate, QCDate::QCBusDayAdjRules endDateAdjustment, Tenor settlementPeriodicity, QCInterestRateLeg::QCStubPeriod settlementStubPeriod, QCBusinessCalendar settlementCalendar, unsigned int settlementLag, double notional, bool doesAmortize, double spread, double gearing);
			 *
			 * @brief	Builds bullet icp clf leg
			 *
			 * @author	Alvaro Diaz V.
			 * @date	07/07/2018
			 *
			 * @param	recPay				 	Indicates if the cashflows in the leg are received or payed.
			 * @param	startDate			 	The start date.
			 * @param	endDate				 	The end date.
			 * @param	endDateAdjustment	 	The end date adjustment.
			 * @param	settlementPeriodicity	The settlement periodicity.
			 * @param	settlementStubPeriod 	The settlement stub period.
			 * @param	settlementCalendar   	The settlement calendar.
			 * @param	settlementLag		 	The settlement lag.
			 * @param	notional			 	The notional.
			 * @param	doesAmortize		 	True to does amortize.
			 * @param	spread				 	The spread.
			 * @param	gearing				 	The gearing.
			 *
			 * @return	A Leg.
			 */
			static Leg buildBulletIcpClfLeg(
				RecPay recPay,
				QCDate startDate,
				QCDate endDate,
				QCDate::QCBusDayAdjRules endDateAdjustment,
				Tenor settlementPeriodicity,
				QCInterestRateLeg::QCStubPeriod settlementStubPeriod,
				QCBusinessCalendar settlementCalendar,
				unsigned int settlementLag,
				double notional,
				bool doesAmortize,
				double spread,
				double gearing);

			/**
			 * @fn	static Leg LegFactory::buildCustomAmortIcpClfLeg( RecPay recPay, QCDate startDate, QCDate endDate, QCDate::QCBusDayAdjRules endDateAdjustment, Tenor settlementPeriodicity, QCInterestRateLeg::QCStubPeriod settlementStubPeriod, QCBusinessCalendar settlementCalendar, unsigned int settlementLag, CustomNotionalAmort notionalAndAmort, bool doesAmortize, double spread, double gearing);
			 *
			 * @brief	Builds custom amort icp clf leg
			 *
			 * @author	Alvaro Diaz V.
			 * @date	07/07/2018
			 *
			 * @param	recPay				 	Indicates if the cashflows in the leg are received or payed.
			 * @param	startDate			 	The start date.
			 * @param	endDate				 	The end date.
			 * @param	endDateAdjustment	 	The end date adjustment.
			 * @param	settlementPeriodicity	The settlement periodicity.
			 * @param	settlementStubPeriod 	The settlement stub period.
			 * @param	settlementCalendar   	The settlement calendar.
			 * @param	settlementLag		 	The settlement lag.
			 * @param	notionalAndAmort	 	The notional and amort.
			 * @param	doesAmortize		 	True to does amortize.
			 * @param	spread				 	The spread.
			 * @param	gearing				 	The gearing.
			 *
			 * @return	A Leg.
			 */
			static Leg buildCustomAmortIcpClfLeg(
				RecPay recPay,
				QCDate startDate,
				QCDate endDate,
				QCDate::QCBusDayAdjRules endDateAdjustment,
				Tenor settlementPeriodicity,
				QCInterestRateLeg::QCStubPeriod settlementStubPeriod,
				QCBusinessCalendar settlementCalendar,
				unsigned int settlementLag,
				CustomNotionalAmort notionalAndAmort,
				bool doesAmortize,
				double spread,
				double gearing);

			/**
			* @fn	static void LegFactory::customizeAmortization(RecPay recPay, Leg& leg,
			* 		CustomNotionalAmort notionalAndAmort, TypeOfCashflow typeOfCashflow);
			*
			* @brief	Customize amortization
			*
			* @author	Alvaro Diaz V.
			* @date	07/07/2018
			*
			* @param	        recPay				Indicates if the cashflows in the leg are received or payed.
			* @param [in,out]	leg					The leg.
			* @param 		  	notionalAndAmort	The notional and amort.
			* @param 		  	typeOfCashflow  	Type of the cashflow.
			*/
			static void customizeAmortization(RecPay recPay,
				Leg& leg,
				CustomNotionalAmort notionalAndAmort,
				TypeOfCashflow typeOfCashflow);

			/**
			 * @fn	LegFactory::~LegFactory();
			 *
			 * @brief	Destructor
			 *
			 * @author	Alvaro Diaz V.
			 * @date	07/07/2018
			 */
			~LegFactory();

		private:
			/**
			 * @fn	LegFactory::LegFactory();
			 *
			 * @brief	Default constructor
			 *
			 * @author	Alvaro Diaz V.
			 * @date	07/07/2018
			 */
			LegFactory();
		};

	}
}

#endif //LEGFACTORY_H