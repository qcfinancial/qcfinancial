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

        int Leg::getCurrentCashflowIndex(QCDate date) {
        	if (date < _cashflows.at(0) -> startDate() ||
        		date > _cashflows.at(_cashflows.size() - 1) -> startDate())  {
        		return -1;
        	}
        	return static_cast<int>(std::distance(_cashflows.begin(), std::find_if(
        			_cashflows.begin(), _cashflows.end(),
        			[&date](const shared_ptr<Cashflow>& cf) {
        				return cf->startDate() <= date && date < cf->endDate();
        			}
        	)));
        }
	}
}
