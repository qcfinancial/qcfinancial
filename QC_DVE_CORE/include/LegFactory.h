#ifndef LEGFACTORY_H
#define LEGFACTORY_H

#include<memory>

#include "Cashflow.h"
#include "FixedRateCashflow.h"
#include "Leg.h"
#include "InterestRateIndex.h"
#include "Tenor.h"

#include "QCInterestRateLeg.h"
#include "QCBusinessCalendar.h"
#include "QCInterestRate.h"

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
		 * @author	Alvaro Díaz V.
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
			 * @author	Alvaro Díaz V.
			 * @date	07/07/2018
			 *
			 * @param	recPay				 	The record pay.
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
			static Leg LegFactory::buildBulletFixedRateLeg(
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
			 * @fn	static Leg LegFactory::buildCustomAmortFixedRateLeg( RecPay recPay, QCDate startDate, QCDate endDate, QCDate::QCBusDayAdjRules endDateAdjustment, Tenor settlementPeriodicity, QCInterestRateLeg::QCStubPeriod settlementStubPeriod, QCBusinessCalendar settlementCalendar, unsigned int settlementLag, CustomNotionalAmort notionalAndAmort, bool doesAmortize, QCInterestRate rate, std::shared_ptr<QCCurrency> currency);
			 *
			 * @brief	Builds custom amort fixed rate leg
			 *
			 * @author	Alvaro Díaz V.
			 * @date	07/07/2018
			 *
			 * @param	recPay				 	The record pay.
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
			static Leg LegFactory::buildCustomAmortFixedRateLeg(
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
			 * @author	Alvaro Díaz V.
			 * @date	07/07/2018
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
			 *
			 * @return	A Leg.
			 */
			static Leg LegFactory::buildBulletIborLeg(
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
			 * @fn	static Leg LegFactory::buildCustomAmortIborLeg( RecPay recPay, QCDate startDate, QCDate endDate, QCDate::QCBusDayAdjRules endDateAdjustment, Tenor settlementPeriodicity, QCInterestRateLeg::QCStubPeriod settlementStubPeriod, QCBusinessCalendar settlementCalendar, unsigned int settlementLag, CustomNotionalAmort notionalAndAmort, Tenor fixingPeriodicity, QCInterestRateLeg::QCStubPeriod fixingStubPeriod, QCBusinessCalendar fixingCalendar, unsigned int fixingLag, std::shared_ptr<InterestRateIndex> index, bool doesAmortize, std::shared_ptr<QCCurrency> currency, double spread, double gearing);
			 *
			 * @brief	Builds custom amort ibor leg
			 *
			 * @author	Alvaro Díaz V.
			 * @date	07/07/2018
			 *
			 * @param	recPay				 	The record pay.
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
			static Leg LegFactory::buildCustomAmortIborLeg(
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
			 * @author	Alvaro Díaz V.
			 * @date	07/07/2018
			 *
			 * @param	recPay				 	The record pay.
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
			static Leg LegFactory::buildBulletIcpClpLeg(
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
			 * @author	Alvaro Díaz V.
			 * @date	07/07/2018
			 *
			 * @param	recPay				 	The record pay.
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
			static Leg LegFactory::buildCustomAmortIcpClpLeg(
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
			 * @author	Alvaro Díaz V.
			 * @date	07/07/2018
			 *
			 * @param	recPay				 	The record pay.
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
			static Leg LegFactory::buildBulletIcpClfLeg(
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
			 * @author	Alvaro Díaz V.
			 * @date	07/07/2018
			 *
			 * @param	recPay				 	The record pay.
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
			static Leg LegFactory::buildCustomAmortIcpClfLeg(
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
			* @author	Alvaro Díaz V.
			* @date	07/07/2018
			*
			* @param 		  	recPay				The record pay.
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
			 * @author	Alvaro Díaz V.
			 * @date	07/07/2018
			 */
			~LegFactory();

		private:
			/**
			 * @fn	LegFactory::LegFactory();
			 *
			 * @brief	Default constructor
			 *
			 * @author	Alvaro Díaz V.
			 * @date	07/07/2018
			 */
			LegFactory();
		};

	}
}

#endif //LEGFACTORY_H