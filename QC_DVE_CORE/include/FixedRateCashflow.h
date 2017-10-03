#ifndef FIXEDRATECASHFLOW_H
#define FIXEDRATECASHFLOW_H

#include<tuple>
#include<memory>

#include "Cashflow.h"
#include "QCDate.h"
#include "QCInterestRate.h"
#include "QCCurrency.h"

namespace QCode
{
	namespace Financial
	{
		typedef std::tuple<QCDate, QCDate, QCDate,
			double, double, double, bool,
			shared_ptr<QCCurrency>,
			QCInterestRate> FixedRateCashflowWrapper;

		/**
		 * @class	FixedRateCashflow
		 *
		 * @brief	A fixed rate cashflow.
		 *
		 * @author	Alvaro Díaz V.
		 * @date	27/09/2017
		 */
		class FixedRateCashflow : public Cashflow
		{
		public:

			/**
			* @enum	Element
			*
			* @brief	Values that represent the different elements of a FixedRateCashflow
			*/
			enum Element
			{
				///< An enum constant representing the start date
				startDate,

				///< An enum constant representing the end date
				endDate,

				///< An enum constant representing the settlement date
				settlementDate,

				///< An enum constant representing the notional
				notional,

				///< An enum constant representing the amortization
				amortization,

				///< An enum constant representing the interest
				interest,

				///< An enum constant representing the amort is cashflow
				amortIsCashflow,

				///< An enum constant representing the currency
				currency
			};

			/**
			 * @fn	FixedRateCashflow::FixedRateCashflow(const QCDate& startDate, const QCDate& endDate, const QCDate& settlementDate, double nominal, double amortization, bool doesAmortize, const QCInterestRate& rate, const QCCurrency& currency);
			 *
			 * @brief	Constructor. The parameters startDate and endDate are the dates between which the
			 * 			rate accrues.
			 *
			 * @author	Alvaro Díaz V.
			 * @date	27/09/2017
			 *
			 * @param	startDate	  	The start date.
			 * @param	endDate		  	The end date.
			 * @param	settlementDate	The settlement date.
			 * @param	nominal		  	The nominal.
			 * @param	amortization  	The amortization.
			 * @param	doesAmortize  	Indicates whether amortization is cashflow or not.
			 * @param	rate		  	The rate.
			 * @param	currency	  	The currency.
			 */

			FixedRateCashflow(const QCDate& startDate,
							  const QCDate& endDate,
							  const QCDate& settlementDate,
							  double nominal,
							  double amortization,
							  bool doesAmortize,
							  const QCInterestRate& rate,
							  std::shared_ptr<QCCurrency> currency);

			/**
			 * @fn	double FixedRateCashflow::amount();
			 *
			 * @brief	Gets the amount of the cashflow.
			 *
			 * @author	Alvaro Díaz V.
			 * @date	27/09/2017
			 *
			 * @return	A double.
			 */
			double amount();

			/**
			 * @fn	QCCurrency FixedRateCashflow::ccy();
			 *
			 * @brief	Gets the currency of the cashflow.
			 *
			 * @author	Alvaro Díaz V.
			 * @date	27/09/2017
			 *
			 * @return	A QCCurrency.
			 */
			shared_ptr<QCCurrency> ccy();

			/**
			 * @fn	QCDate FixedRateCashflow::date();
			 *
			 * @brief	Gets the payment date of the cashflow.
			 *
			 * @author	Alvaro Díaz V.
			 * @date	27/09/2017
			 *
			 * @return	A QCDate.
			 */
			QCDate date();

			/**
			 * @fn	tuple<QCDate, QCDate, QCDate, double, double, double, bool, QCCurrency> FixedRateCashflow::show();
			 *
			 * @brief	Wraps the cashflow in a FixedRateCashflowWrapper
			 *
			 * @author	Alvaro Díaz V.
			 * @date	27/09/2017
			 *
			 * @return	A tuple&lt;QCDate,QCDate,QCDate,double,double,double,bool,QCCurrency&gt;
			 */
			shared_ptr<FixedRateCashflowWrapper> wrap();

			/**
			* @fn	virtual FixedRateCashflow::~FixedRateCashflow();
			*
			* @brief	Destructor
			*
			* @author	Alvaro Díaz V.
			* @date	27/09/2017
			*/
			virtual ~FixedRateCashflow();

		private:

			/** @brief	Calculates interest */
			void _calculateInterest();

			/** @brief	The start date */
			QCDate _startDate;

			/** @brief	The end date */
			QCDate _endDate;

			/** @brief	The settlement date */
			QCDate _settlementDate;

			/** @brief	The nominal */
			double _nominal;

			/** @brief	The amortization */
			double _amortization;

			/** @brief	The interest amount calculated */
			double _interest;

			/** @brief	True if amortization is part of the cashflow */
			bool _doesAmortize;

			/** @brief	The rate */
			QCInterestRate _rate;

			/** @brief	The currency */
			shared_ptr<QCCurrency> _currency;
		};

		/**
		 * @typedef	std::vector<shared_ptr<FixedRateCashflow>> FixedRateLeg
		 *
		 * @brief	Defines an alias representing a Leg made of Fixed Rate Cashflows.
		 */
		typedef std::vector<shared_ptr<FixedRateCashflow>> FixedRateLeg;
	};
}

#endif //FIXEDRATECASHFLOW_H