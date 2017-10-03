#include<memory>

#include "LegFactory.h"
#include "FixedRateCashflow.h"

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
				fixedRateleg.at(i) = std::make_shared<FixedRateCashflow>(frc);
				++i;
			}

			return fixedRateleg;
		}

		LegFactory::~LegFactory()
		{
		}

	}
}
