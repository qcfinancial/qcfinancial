#include "Leg.h"

namespace QCode
{
	namespace Financial
	{
		Leg::Leg()
		{
		}

		std::shared_ptr<Cashflow> Leg::getCashflowAt(size_t pos)
		{
			return _cashflows.at(pos);
		}

		void Leg::setCashflowAt(shared_ptr<Cashflow> cashflow, size_t pos)
		{
			_cashflows.at(pos) = cashflow;
		}

		void Leg::appendCashflow(shared_ptr<Cashflow> cashflow)
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

		Leg::~Leg()
		{
		}
	}
}
