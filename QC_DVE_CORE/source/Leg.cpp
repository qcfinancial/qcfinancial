#include "Leg.h"

#include <utility>

namespace QCode
{
	namespace Financial
	{
		Leg::Leg() = default;

		std::shared_ptr<Cashflow> Leg::getCashflowAt(size_t pos)
		{
			return _cashflows.at(pos);
		}

		void Leg::setCashflowAt(shared_ptr<Cashflow> cashflow, size_t pos)
		{
			_cashflows.at(pos) = std::move(cashflow);
		}

		void Leg::appendCashflow(const shared_ptr<Cashflow>& cashflow)
		{
			_cashflows.push_back(cashflow);
		}

		size_t Leg::size()
		{
			return _cashflows.size();
		}

		void Leg::resize(size_t newSize)
		{
			_cashflows.resize(newSize);
		}

		Leg::~Leg() = default;

        std::vector<std::shared_ptr<Cashflow>> Leg::getCashflows() const {
            return _cashflows;
        }
    }
}
