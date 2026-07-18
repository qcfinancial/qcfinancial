#ifndef OPERATION_H
#define OPERATION_H

#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "Leg.h"
#include "cashflows/Cashflow.h"

namespace QCode
{
	namespace Financial
	{
		/**
		 * @class	Operation
		 *
		 * @brief	Immutable container of one or more legs identified by a caller-supplied
		 * 			opaque key. qcfinancial attaches no meaning to the key. Legs are numbered
		 * 			from 1 in construction order and carry a RecPay direction as metadata
		 * 			(amounts keep the signs baked in by LegFactory).
		 */
		class Operation
		{
		public:
			Operation(long long key, std::vector<Leg> legs, std::vector<RecPay> recPay) :
				_key(key),
				_legs(std::move(legs)),
				_recPay(std::move(recPay)),
				_maxSettlementSerial(0)
			{
				if (_legs.empty())
				{
					throw std::invalid_argument(
						"Operation " + std::to_string(_key) + ": at least one leg is required.");
				}
				if (_legs.size() != _recPay.size())
				{
					throw std::invalid_argument(
						"Operation " + std::to_string(_key) +
						": legs and rec_pay must have the same length.");
				}
				for (auto& leg : _legs)
				{
					if (leg.size() == 0)
					{
						throw std::invalid_argument(
							"Operation " + std::to_string(_key) + ": legs must not be empty.");
					}
					for (size_t i = 0; i < leg.size(); ++i)
					{
						auto serial = leg.getCashflowAt(i)->date().excelSerial();
						if (serial > _maxSettlementSerial)
						{
							_maxSettlementSerial = serial;
						}
					}
				}
			}

			[[nodiscard]] long long getKey() const
			{
				return _key;
			}

			[[nodiscard]] size_t numberOfLegs() const
			{
				return _legs.size();
			}

			// Non-const because Leg and Cashflow expose only non-const accessors;
			// the Python binding exposes no mutating surface.
			Leg& getLeg(size_t i)
			{
				return _legs.at(i);
			}

			[[nodiscard]] RecPay getRecPay(size_t i) const
			{
				return _recPay.at(i);
			}

			/**
			 * @brief	Largest settlement date (excel serial) across all cashflows.
			 * 			Used by Portfolio to size per-query discount factor tables.
			 */
			[[nodiscard]] long getMaxSettlementSerial() const
			{
				return _maxSettlementSerial;
			}

		private:
			long long _key;
			std::vector<Leg> _legs;
			std::vector<RecPay> _recPay;
			long _maxSettlementSerial;
		};
	}
}

#endif //OPERATION_H
