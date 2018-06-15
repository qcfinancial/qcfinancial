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
			double accruedInterest(const QCDate& valueDate);

		protected:
			Leg _fixedRateLeg;
		};
	}
}

#endif //FIXEDRATEBOND_H

