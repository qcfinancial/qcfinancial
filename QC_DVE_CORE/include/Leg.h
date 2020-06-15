#ifndef LEG_H
#define LEG_H

#include<memory>
#include<vector>

#include "cashflows/Cashflow.h"
#include "cashflows/FixedRateCashflow.h"

namespace QCode
{
	namespace Financial
	{
		using FixedRateLeg = std::vector<std::shared_ptr<FixedRateCashflow>>;

		/**
		 * @class	Leg
		 *
		 * @brief	Wrapper class that provides the basis for an arbitrary leg of a financial product .
		 *
		 * @author	Alvaro Díaz V.
		 * @date	07/07/2018
		 */
		class Leg
		{
		public:

			/**
			 * @fn	Leg::Leg();
			 *
			 * @brief	Default constructor
			 *
			 * @author	Alvaro Díaz V.
			 * @date	07/07/2018
			 */
			Leg();

			/**
			 * @fn	std::shared_ptr<Cashflow> Leg::getCashflowAt(size_t pos);
			 *
			 * @brief	Gets cashflow at position. Position must be between 0 and number of cashflows - 1.
			 *
			 * @author	Alvaro Díaz V.
			 * @date	07/07/2018
			 *
			 * @param	pos	The position of the cashflow.
			 *
			 * @return	The cashflow at.
			 */
			std::shared_ptr<Cashflow> getCashflowAt(size_t pos);

			/**
			 * @fn	void Leg::setCashflowAt(shared_ptr<Cashflow> cashflow, size_t pos);
			 *
			 * @brief	Sets cashflow at the specified position.
			 *
			 * @author	Alvaro Díaz V.
			 * @date	07/07/2018
			 *
			 * @param	cashflow	The cashflow.
			 * @param	pos			The position.
			 */
			void setCashflowAt(shared_ptr<Cashflow> cashflow, size_t pos);

			/**
			 * @fn	void Leg::appendCashflow(shared_ptr<Cashflow> cashflow);
			 *
			 * @brief	Appends a cashflow (at the last position).
			 *
			 * @author	Alvaro Díaz V.
			 * @date	07/07/2018
			 *
			 * @param	cashflow	The cashflow.
			 */
			void appendCashflow(shared_ptr<Cashflow> cashflow);

			/**
			 * @fn	size_t Leg::size();
			 *
			 * @brief	Gets the size of the leg (number of cashflows).
			 *
			 * @author	Alvaro Díaz V.
			 * @date	07/07/2018
			 *
			 * @return	A size_t.
			 */
			size_t size();

			/**
			 * @fn	void Leg::resize(size_t newSize);
			 *
			 * @brief	Resizes the Leg given the new size
			 *
			 * @author	Alvaro D�az V.
			 * @date	07/07/2018
			 *
			 * @param	newSize	Size of the new.
			 */
			void resize(size_t newSize);

			/**
			 * @fn	virtual Leg::~Leg();
			 *
			 * @brief	Destructor
			 *
			 * @author	Alvaro D�az V.
			 * @date	07/07/2018
			 */
			virtual ~Leg();

		private:
			/** @brief	The cashflows */
			vector<shared_ptr<Cashflow>> _cashflows;
		};
	}
}
#endif //LEG_H