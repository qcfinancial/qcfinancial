#ifndef CASHFLOW_H
#define CASHFLOW_H

#include<memory>

#include "QCCurrency.h"
#include "QCDate.h"

namespace QCode
{
	namespace Financial
	{
		/**
		* @class	Cashflow
		*
		* @brief	Abstract class that provides the basis for every type of cashflow.
		*
		* @author	Alvaro Díaz V.
		* @date	26/09/2017
		*/
		class Cashflow
		{
		public:

			/**
			* @fn	Cashflow::Cashflow();
			*
			* @brief	Default constructor
			*
			* @author	Alvaro Díaz V.
			* @date	26/09/2017
			*/
			Cashflow()
			{
			}

			/**
			* @fn	virtual double Cashflow::amount() = 0;
			*
			* @brief	Gets the amount
			*
			* @author	Alvaro Díaz V.
			* @date	26/09/2017
			*
			* @return	A double.
			*/
			virtual double amount()
			{
				return 0.0;
			}

			/**
			* @fn	virtual QCCurrency Cashflow::ccy() = 0;
			*
			* @brief	Gets the currency of the cashflow.
			*
			* @author	Alvaro Díaz V.
			* @date	26/09/2017
			*
			* @return	A QCCurrency.
			*/
			virtual shared_ptr<QCCurrency> ccy()
			{
				return shared_ptr<QCCurrency>(new QCCurrency);
			}

			/**
			* @fn	virtual QCDate Cashflow::date() = 0;
			*
			* @brief	Gets the date of the cashflow.
			*
			* @author	Alvaro Díaz V.
			* @date	26/09/2017
			*
			* @return	A QCDate.
			*/
			virtual QCDate date()
			{
				return QCDate();
			}

			/**
			* @fn	bool Cashflow::isExpired(const QCDate& refDate)
			*
			* @brief	Query if the cashflow is expired with respect to 'refDate'.
			*
			* @author	Alvaro Díaz V.
			* @date	26/09/2017
			*
			* @param	refDate	The reference date.
			*
			* @return	True if expired, false if not.
			*/
			bool isExpired(const QCDate& refDate)
			{
				return refDate > date();
			}

			virtual ~Cashflow()
			{
			}
		};

		/**
		 * @typedef	std::vector<std::shared_ptr<Cashflow>> Leg
		 *
		 * @brief	Defines an alias representing a Leg that is made of arbitrary cashflows.
		 */
		typedef std::vector<std::shared_ptr<Cashflow>> Leg;

		/**
		 * @enum	RecPay
		 *
		 * @brief	Values that represent whether a cashflow is received or paid.
		 */
		enum RecPay
		{
			Receive,
			Pay
		};
	}
}

#endif //CASHFLOW_H