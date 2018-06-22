#ifndef FIXEDRATEBOND_H
#define FIXEDRATEBOND_H

#include "Leg.h"
#include "FixedRateCashflow.h"

namespace QCode
{
	namespace Financial
	{
		class FixedRateBond
		{
		public:
			FixedRateBond(Leg& fixedRateLeg);
			double presentValue(const QCDate& valueDate, const QCInterestRate& yieldToMaturity);
			virtual double price(const QCDate& valueDate, const QCInterestRate& yieldToMaturity);
			double accruedInterest(const QCDate& valueDate);
			double duration() const;
			double convexity() const;
			Leg getLeg() const;

		protected:
			Leg _fixedRateLeg;
			double _duration;
			double _convexity;
		};
	}
}

#endif //FIXEDRATEBOND_H

