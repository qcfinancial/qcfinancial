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
		 * 			from 1 in construction order. Receive/pay direction is not stored: it is
		 * 			already expressed by the signs LegFactory bakes into the amounts.
		 */
		class Operation
		{
		public:
			Operation(long long key, std::vector<Leg> legs) :
				_key(key),
				_legs(std::move(legs)),
				_maxSettlementSerial(0)
			{
				if (_legs.empty())
				{
					throw std::invalid_argument(
						"Operation " + std::to_string(_key) + ": at least one leg is required.");
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
			long _maxSettlementSerial;
		};
	}
}

#endif //OPERATION_H
