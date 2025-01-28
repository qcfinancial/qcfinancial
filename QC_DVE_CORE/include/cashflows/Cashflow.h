#ifndef CASHFLOW_H
#define CASHFLOW_H

#define NO_CONSTRUCTOR_VALIDATION

#include<memory>

#include "asset_classes/QCCurrency.h"
#include "time/QCDate.h"

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
			* @author	Alvaro D�az V.
			* @date	26/09/2017
			*/
			Cashflow() = default;

			/**
			* @fn	virtual double Cashflow::amount();
			*
			* @brief	Gets the amount
			*
			* @author	Alvaro D�az V.
			* @date	26/09/2017
			*
			* @return	A double.
			*/
			virtual double amount() = 0;

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
			virtual shared_ptr<QCCurrency> ccy() = 0;

			/**
			* @fn	virtual QCDate Cashflow::date();
			*
			* @brief	Gets the payment date of the cashflow.
			*
			* @author	Alvaro Díaz V.
			* @date	26/09/2017
			*
			* @return	A QCDate.
			*/
			virtual QCDate date() = 0;

			virtual QCDate endDate() = 0;

            void savePresentValue(double presentValue)
            {
                _presentValue = presentValue;
            };

            void saveDiscountFactor(double discountFactor)
            {
                _discountFactor = discountFactor;
            };

            [[nodiscard]] virtual double getPresentValue() const
            {
                return _presentValue;
            };

            [[nodiscard]] virtual double getDiscountFactor() const
            {
                return _discountFactor;
            };

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
			virtual bool isExpired(const QCDate& refDate)
			{
				return refDate > date();
			}

            [[nodiscard]] virtual std::string getType() const = 0;

			/**
			 * @fn	virtual Cashflow::~Cashflow()
			 *
			 * @brief	Destructor
			 *
			 * @author	Alvaro D�az V.
			 * @date	04/10/2017
			 */
			virtual ~Cashflow() = default;

        protected:

            double _presentValue = 0.0;

            double _discountFactor = 0.0;
		};

		/**
		 * @enum	RecPay
		 *
		 * @brief	Values that represent whether a cashflow is received or paid.
		 */
		enum RecPay
		{
			///< An enum constant representing the receive option
			Receive,


			///< An enum constant representing the pay option
			Pay
		};

	}
}

#endif //CASHFLOW_H