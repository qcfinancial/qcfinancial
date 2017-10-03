#ifndef LEGFACTORY_H
#define LEGFACTORY_H

#include "Cashflow.h"

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
			static Leg LegFactory::buildBulletFixedRateLeg(
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
				std::shared_ptr<QCCurrency> currency);
			~LegFactory();
		
		private:
			LegFactory();
		};

	}
}

#endif //LEGFACTORY_H