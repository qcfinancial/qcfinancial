#ifndef CHILEANFIXEDRATEBOND_H
#define CHILEANFIXEDRATEBOND_H

#include <memory>

#include "Leg.h"
#include "FixedRateBond.h"

namespace QCode
{
	namespace Financial
	{
		/**
		 *
		 */
		class ChileanFixedRateBond : public FixedRateBond
		{
		public:
		/**
		*
		*/
			ChileanFixedRateBond(Leg& fixedRateLeg,
			const QCInterestRate& tera);
			double valorPar(const QCDate& valueDate);
			double price(const QCDate& valueDate, const QCInterestRate& yieldToMaturity);
			double duration() const;
			double convexity() const;
			double settlementValue(double notional, std::shared_ptr<QCCurrency> currency,
				const QCDate& valueDate, const QCInterestRate& yieldToMaturity);

		private:
			QCInterestRate _tera;
			double _duration;
			double _convexity;
		};
	}
}

#endif //CHILEANFIXEDRATEBOND_H

