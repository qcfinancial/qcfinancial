#include "FixedRateBond.h"

namespace QCode
{
	namespace Financial
	{
		FixedRateBond::FixedRateBond(Leg& fixedRateLeg) :_fixedRateLeg(fixedRateLeg)
		{
		}

		double FixedRateBond::accruedInterest(const QCDate& valueDate)
		{
			for (size_t i = 0; i < _fixedRateLeg.size(); ++i)
			{
				auto flow = std::dynamic_pointer_cast<FixedRateCashflow>(_fixedRateLeg.getCashflowAt(i));
				if (flow->getStartDate() <= valueDate && valueDate <= flow->getEndDate())
				{
					return flow->accruedInterest(valueDate);
				}
			}
			return 0.0;
		}
	}
}



