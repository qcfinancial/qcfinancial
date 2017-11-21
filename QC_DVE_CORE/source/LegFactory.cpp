#include<memory>
#include<algorithm>
#include<tuple>

#include "LegFactory.h"
#include "FixedRateCashflow.h"
#include "IborCashflow.h"
#include "IcpClpCashflow.h"

#include "QCInterestRatePeriodsFactory.h"

namespace QCode
{
	namespace Financial
	{
		LegFactory::LegFactory()
		{
		}

		Leg LegFactory::buildBulletFixedRateLeg(
			RecPay recPay,
			QCDate startDate,
			QCDate endDate,
			QCDate::QCBusDayAdjRules endDateAdjustment,
			string settlementPeriodicity,
			QCInterestRateLeg::QCStubPeriod settlementStubPeriod,
			QCBusinessCalendar settlementCalendar,
			unsigned int settlementLag,
			double notional,
			bool doesAmortize,
			QCInterestRate rate,
			std::shared_ptr<QCCurrency> currency)
		{
			// Make all the holidays in the calendar into a shared_ptr.
			auto settCal = std::make_shared<DateList>(settlementCalendar.getHolidays());

			// Minus sign is set if cashflows are paid.
			int sign;
			if (recPay == Receive)
			{
				sign = 1;
			}
			else
			{
				sign = -1;
			}

			// Instantiate factory and build the corresponding periods.
			QCInterestRatePeriodsFactory pf{ startDate, endDate, endDateAdjustment,
				settlementPeriodicity, settlementStubPeriod, settCal, settlementLag,
				// The next parameters are useful only for IborLegs. Arbitrary values
				// are given to them in this case.
				settlementPeriodicity, settlementStubPeriod, settCal, 0, 0, settlementPeriodicity };
			auto periods = pf.getPeriods();

			// Load the periods into the structure of FixedRateCashflow and contruct the Leg.
			Leg fixedRateleg;
			size_t numPeriods = periods.size();
			fixedRateleg.resize(numPeriods);
			size_t i = 0;
			for (const auto& period : periods)
			{
				QCDate thisStartDate = get<QCInterestRateLeg::intRtPrdElmntStartDate>(period);
				QCDate thisEndDate = get<QCInterestRateLeg::intRtPrdElmntEndDate>(period);
				QCDate settlementDate = get<QCInterestRateLeg::intRtPrdElmntSettlmntDate>(period);
				double amort = 0.0;
				if (i == numPeriods - 1)
				{
					amort = sign * notional;
				}
				FixedRateCashflow frc{ thisStartDate, thisEndDate, settlementDate,
					sign * notional, amort, doesAmortize, rate, currency };
				fixedRateleg.setCashflowAt(std::make_shared<FixedRateCashflow>(frc), i);
				++i;
			}

			return fixedRateleg;
		}

		Leg LegFactory::buildCustomAmortFixedRateLeg(
			RecPay recPay,
			QCDate startDate,
			QCDate endDate,
			QCDate::QCBusDayAdjRules endDateAdjustment,
			string settlementPeriodicity,
			QCInterestRateLeg::QCStubPeriod settlementStubPeriod,
			QCBusinessCalendar settlementCalendar,
			unsigned int settlementLag,
			CustomNotionalAmort notionalAndAmort,
			bool doesAmortize,
			QCInterestRate rate,
			std::shared_ptr<QCCurrency> currency)
		{
			Leg fixedRateLeg = buildBulletFixedRateLeg(
				recPay,
				startDate,
				endDate,
				endDateAdjustment,
				settlementPeriodicity,
				settlementStubPeriod,
				settlementCalendar,
				settlementLag,
				1.0,
				doesAmortize,
				rate,
				currency);

			//// Ahora modificar nominal y amortización en el objeto Leg recién construido.
			//size_t notionalAndAmortSize = notionalAndAmort.getSize();
			//size_t fixedRateLegSize = fixedRateLeg.size();
			//size_t minSize = std::min(notionalAndAmortSize, fixedRateLegSize);
			//int sign;
			//if (recPay == Receive)
			//{
			//	sign = 1;
			//}
			//else
			//{
			//	sign = -1;
			//}
			//for (size_t i = 0; i < minSize; ++i)
			//{
			//	std::dynamic_pointer_cast<FixedRateCashflow>(fixedRateLeg.getCashflowAt(fixedRateLegSize - 1 - i))
			//		->setNominal(sign * std::get<0>(notionalAndAmort.customNotionalAmort[notionalAndAmortSize - 1 - i]));
			//	std::dynamic_pointer_cast<FixedRateCashflow>(fixedRateLeg.getCashflowAt(fixedRateLegSize - 1 - i))
			//		->setAmortization(sign * std::get<1>(notionalAndAmort.customNotionalAmort[notionalAndAmortSize - 1 - i]));
			//}

			customizeAmortization(recPay, fixedRateLeg, notionalAndAmort, LegFactory::fixedRateCashflow);
			return fixedRateLeg;
		}

		Leg LegFactory::buildBulletIborLeg(
			RecPay recPay,
			QCDate startDate,
			QCDate endDate,
			QCDate::QCBusDayAdjRules endDateAdjustment,
			string settlementPeriodicity,
			QCInterestRateLeg::QCStubPeriod settlementStubPeriod,
			QCBusinessCalendar settlementCalendar,
			unsigned int settlementLag,
			string fixingPeriodicity,
			QCInterestRateLeg::QCStubPeriod fixingStubPeriod,
			QCBusinessCalendar fixingCalendar,
			unsigned int fixingLag,
			std::shared_ptr<InterestRateIndex> index,
			double notional,
			bool doesAmortize,
			std::shared_ptr<QCCurrency> currency,
			double spread,
			double gearing)
		{
			// Make all the holidays in the calendar into a shared_ptr.
			auto settCal = std::make_shared<DateList>(settlementCalendar.getHolidays());
			auto fixCal = std::make_shared<DateList>(fixingCalendar.getHolidays());

			// Minus sign is set if cashflows are paid.
			int sign;
			if (recPay == Receive)
			{
				sign = 1;
			}
			else
			{
				sign = -1;
			}

			//Se da de alta la fabrica de periods
			QCInterestRatePeriodsFactory factory{ startDate, endDate,
				endDateAdjustment,
				settlementPeriodicity,
				settlementStubPeriod,
				settCal,
				settlementLag,
				fixingPeriodicity,
				fixingStubPeriod,
				fixCal,
				fixingLag,
				index->getDaysOfStartLag(),
				index->getTenor().getString() };

			//Se generan los periodos
			auto periods = factory.getPeriods();

			// Load the periods into the structure of FixedRateCashflow and contruct the Leg.
			Leg iborLeg;
			size_t numPeriods = periods.size();
			iborLeg.resize(numPeriods);
			size_t i = 0;
			for (const auto& period : periods)
			{
				QCDate thisStartDate = get<QCInterestRateLeg::intRtPrdElmntStartDate>(period);
				QCDate thisEndDate = get<QCInterestRateLeg::intRtPrdElmntEndDate>(period);
				QCDate thisFixingDate = get<QCInterestRateLeg::intRtPrdElmntFxngDate>(period);
				QCDate settlementDate = get<QCInterestRateLeg::intRtPrdElmntSettlmntDate>(period);
				double amort = 0.0;
				if (i == numPeriods - 1)
				{
					amort = sign * notional;
				}
				IborCashflow frc{ index, thisStartDate, thisEndDate, thisFixingDate, settlementDate, 
					sign * notional, amort, doesAmortize, currency, spread, gearing };
				iborLeg.setCashflowAt(std::make_shared<IborCashflow>(frc), i);
				++i;
			}

			return iborLeg;

		}

		Leg LegFactory::buildCustomAmortIborLeg(
			RecPay recPay,
			QCDate startDate,
			QCDate endDate,
			QCDate::QCBusDayAdjRules endDateAdjustment,
			string settlementPeriodicity,
			QCInterestRateLeg::QCStubPeriod settlementStubPeriod,
			QCBusinessCalendar settlementCalendar,
			unsigned int settlementLag,
			CustomNotionalAmort notionalAndAmort,
			string fixingPeriodicity,
			QCInterestRateLeg::QCStubPeriod fixingStubPeriod,
			QCBusinessCalendar fixingCalendar,
			unsigned int fixingLag,
			std::shared_ptr<InterestRateIndex> index,
			bool doesAmortize,
			std::shared_ptr<QCCurrency> currency,
			double spread,
			double gearing)
		{
			// Primero construir una pata bullet
			Leg iborLeg = buildBulletIborLeg(recPay,
											 startDate,
											 endDate,
											 endDateAdjustment,
											 settlementPeriodicity,
											 settlementStubPeriod,
											 settlementCalendar,
											 settlementLag,
											 fixingPeriodicity,
											 fixingStubPeriod,
											 fixingCalendar,
											 fixingLag,
											 index,
											 1.0,
											 doesAmortize,
											 currency,
											 spread,
											 gearing);

			//// Ahora modificar nominal y amortización en el objeto Leg recién construido.
			//size_t notionalAndAmortSize = notionalAndAmort.getSize();
			//size_t iborLegSize = iborLeg.size();
			//size_t minSize = std::min(notionalAndAmortSize, iborLegSize);
			//int sign;
			//if (recPay == Receive)
			//{
			//	sign = 1;
			//}
			//else
			//{
			//	sign = -1;
			//}
			//for (size_t i = 0; i < minSize; ++i)
			//{
			//	std::dynamic_pointer_cast<FixedRateCashflow>(iborLeg.getCashflowAt(iborLegSize - 1 - i))
			//		->setNominal(sign * std::get<0>(notionalAndAmort.customNotionalAmort[notionalAndAmortSize - 1 - i]));
			//	std::dynamic_pointer_cast<FixedRateCashflow>(iborLeg.getCashflowAt(iborLegSize - 1 - i))
			//		->setAmortization(sign * std::get<1>(notionalAndAmort.customNotionalAmort[notionalAndAmortSize - 1 - i]));
			//}

			std::cout << "custom amort ibor leg: done bullet" << std::endl;
			customizeAmortization(recPay, iborLeg, notionalAndAmort, LegFactory::iborCashflow);
			return iborLeg;
		}

		void LegFactory::customizeAmortization(RecPay recPay,
											   Leg& leg,
											   CustomNotionalAmort notionalAndAmort,
											   TypeOfCashflow typeOfCashflow)
		{
			// Ahora modificar nominal y amortización en el objeto Leg recién construido.
			size_t notionalAndAmortSize = notionalAndAmort.getSize();
			size_t legSize = leg.size();
			if (legSize == 0)
			{
				throw invalid_argument("LegFactory::customizeAmortization. Leg has 0 cashflows");
			}
			size_t minSize = std::min(notionalAndAmortSize, legSize);
			std::cout << "legSize: " << legSize << std::endl;
			std::cout << "notionalAndAmortSize: " << notionalAndAmortSize << std::endl;

			int sign;
			if (recPay == Receive)
			{
				sign = 1;
			}
			else
			{
				sign = -1;
			}
			if (typeOfCashflow == LegFactory::fixedRateCashflow)
			{
				for (size_t i = 0; i < minSize; ++i)
				{
					std::dynamic_pointer_cast<FixedRateCashflow>(leg.getCashflowAt(legSize - 1 - i))
						->setNominal(sign * std::get<0>(notionalAndAmort.customNotionalAmort[notionalAndAmortSize - 1 - i]));
					std::dynamic_pointer_cast<FixedRateCashflow>(leg.getCashflowAt(legSize - 1 - i))
						->setAmortization(sign * std::get<1>(notionalAndAmort.customNotionalAmort[notionalAndAmortSize - 1 - i]));
				}
			}
			if (typeOfCashflow == LegFactory::iborCashflow)
			{
				for (size_t i = 0; i < minSize; ++i)
				{
					std::dynamic_pointer_cast<IborCashflow>(leg.getCashflowAt(legSize - 1 - i))
						->setNominal(sign * std::get<0>(notionalAndAmort.customNotionalAmort[notionalAndAmortSize - 1 - i]));
					std::dynamic_pointer_cast<IborCashflow>(leg.getCashflowAt(legSize - 1 - i))
						->setAmortization(sign * std::get<1>(notionalAndAmort.customNotionalAmort[notionalAndAmortSize - 1 - i]));
				}
			}
			if (typeOfCashflow == LegFactory::icpClpCashflow)
			{
				for (size_t i = 0; i < minSize; ++i)
				{
					std::dynamic_pointer_cast<IcpClpCashflow>(leg.getCashflowAt(legSize - 1 - i))
						->setNominal(sign * std::get<0>(notionalAndAmort.customNotionalAmort[notionalAndAmortSize - 1 - i]));
					std::dynamic_pointer_cast<IcpClpCashflow>(leg.getCashflowAt(legSize - 1 - i))
						->setAmortization(sign * std::get<1>(notionalAndAmort.customNotionalAmort[notionalAndAmortSize - 1 - i]));
				}
			}


		}

		Leg LegFactory::buildBulletIcpClpLeg(
			RecPay recPay,
			QCDate startDate,
			QCDate endDate,
			QCDate::QCBusDayAdjRules endDateAdjustment,
			string settlementPeriodicity,
			QCInterestRateLeg::QCStubPeriod settlementStubPeriod,
			QCBusinessCalendar settlementCalendar,
			unsigned int settlementLag,
			double notional,
			bool doesAmortize,
			double spread,
			double gearing)
		{
			// Make all the holidays in the calendar into a shared_ptr.
			auto settCal = std::make_shared<DateList>(settlementCalendar.getHolidays());

			// Minus sign is set if cashflows are paid.
			int sign;
			if (recPay == Receive)
			{
				sign = 1;
			}
			else
			{
				sign = -1;
			}

			// Instantiate factory and build the corresponding periods.
			QCInterestRatePeriodsFactory pf{ startDate, endDate, endDateAdjustment,
				settlementPeriodicity, settlementStubPeriod, settCal, settlementLag,
				// The next parameters are useful only for IborLegs. Arbitrary values
				// are given to them in this case.
				settlementPeriodicity, settlementStubPeriod, settCal, 0, 0, settlementPeriodicity };
			auto periods = pf.getPeriods();

			// Load the periods into the structure of FixedRateCashflow and contruct the Leg.
			Leg icpClpLeg;
			size_t numPeriods = periods.size();
			icpClpLeg.resize(numPeriods);
			size_t i = 0;
			for (const auto& period : periods)
			{
				QCDate thisStartDate = get<QCInterestRateLeg::intRtPrdElmntStartDate>(period);
				QCDate thisEndDate = get<QCInterestRateLeg::intRtPrdElmntEndDate>(period);
				QCDate settlementDate = get<QCInterestRateLeg::intRtPrdElmntSettlmntDate>(period);
				double amort = 0.0;
				if (i == numPeriods - 1)
				{
					amort = sign * notional;
				}
				IcpClpCashflow icpclpc{ thisStartDate, thisEndDate, settlementDate,
					sign * notional, amort, doesAmortize, spread, gearing, DEFAULT_ICP, DEFAULT_ICP };
				icpClpLeg.setCashflowAt(std::make_shared<IcpClpCashflow>(icpclpc), i);
				++i;
			}

			return icpClpLeg;
		}

		Leg LegFactory::buildCustomAmortIcpClpLeg(
			RecPay recPay,
			QCDate startDate,
			QCDate endDate,
			QCDate::QCBusDayAdjRules endDateAdjustment,
			string settlementPeriodicity,
			QCInterestRateLeg::QCStubPeriod settlementStubPeriod,
			QCBusinessCalendar settlementCalendar,
			unsigned int settlementLag,
			CustomNotionalAmort notionalAndAmort,
			bool doesAmortize,
			double spread,
			double gearing)
		{
			Leg icpClpLeg = buildBulletIcpClpLeg(recPay,
											 startDate,
											 endDate,
											 endDateAdjustment,
											 settlementPeriodicity,
											 settlementStubPeriod,
											 settlementCalendar,
											 settlementLag,
											 100.0,
											 doesAmortize,
											 spread,
											 gearing);

			std::cout << "custom amort icp clp leg: done bullet" << std::endl;
			customizeAmortization(recPay, icpClpLeg, notionalAndAmort, LegFactory::icpClpCashflow);
			return icpClpLeg;

		}

		LegFactory::~LegFactory()
		{
		}

	}
}
