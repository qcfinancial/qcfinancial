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
		class LegFactory
		{
		public:
			enum TypeOfCashflow
			{
				fixedRateCashflow,
				iborCashflow,
				icpClpCashflow,
				icpClfCashflow
			};

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
				std::shared_ptr<QCCurrency> currency);

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

			~LegFactory();

		private:
			LegFactory();
			
			static void customizeAmortization(RecPay recPay,
											  Leg& leg,
											  CustomNotionalAmort notionalAndAmort,
											  TypeOfCashflow typeOfCashflow);

		};

	}
}

#endif //LEGFACTORY_H