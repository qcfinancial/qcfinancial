#ifndef LEG_H
#define LEG_H

#include<memory>
#include<vector>

#include "Cashflow.h"
#include "FixedRateCashflow.h"

namespace QCode
{
	namespace Financial
	{
		using FixedRateLeg = std::vector<std::shared_ptr<FixedRateCashflow>>;

		class Leg
		{
		public:
			Leg();
			std::shared_ptr<Cashflow> getCashflowAt(size_t pos);
			void setCashflowAt(shared_ptr<Cashflow> cashflow, size_t pos);
			void appendCashflow(shared_ptr<Cashflow> cashflow);
			size_t size();
			void resize(size_t newSize);
			virtual ~Leg();

		private:
			vector<shared_ptr<Cashflow>> _cashflows;
		};
	}
}
#endif //LEG_H