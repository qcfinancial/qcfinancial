#ifndef SIMPLECASHFLOW_H
#define SIMPLECASHFLOW_H

#include "Cashflows/Cashflow.h"

namespace QCode
{
	namespace Financial
	{
		/**
		 * @typedef	std::tuple<QCDate, double, shared_ptr<QCCurrency>> SimpleCashflowWrapper
		 *
		 * @brief	Defines an alias representing the simple cashflow wrapper
		 */
		typedef std::tuple<QCDate, double, shared_ptr<QCCurrency>> SimpleCashflowWrapper;

		/**
		 * @class	SimpleCashflow
		 *
		 * @brief	A simple cashflow.
		 *
		 * @author	Alvaro D�az V.
		 * @date	07/07/2018
		 */
		class SimpleCashflow : public Cashflow
		{
		public:
			/**
			* @fn	SimpleCashflow::SimpleCashflow(const QCDate& endDate, double amount, const QCCurrency& currency);
			*
			* @brief	Constructor. The parameter endDate is the date in which the cashflow
			* 			actually occurs.
			*
			* @author	Alvaro D�az V.
			* @date	15/11/2017
			*
			* @param	endDate		  	The end date.
			* @param	nominal		  	The nominal or amount of cashflow.
			* @param	currency	  	The currency.
			*/
			SimpleCashflow(const QCDate& endDate,
						   double nominal,
						   std::shared_ptr<QCCurrency> currency);

			/**
			* @fn	double SimpleCashflow::amount();
			*
			* @brief	Gets the amount of the cashflow.
			*
			* @author	Alvaro D�az V.
			* @date	15/11/2017
			*
			* @return	A double.
			*/
			double amount();

			/**
			* @fn	    QCCurrency SimpleCashflow::ccy();
			*
			* @brief    Gets the currency of the cashflow.
			*
			* @author	Alvaro D�az V.
			* @date	15/11/2017
			*
			* @return	A QCCurrency.
			*/
			shared_ptr<QCCurrency> ccy();

			/**
			* @fn	    QCDate SimpleCashflow::date();
			*
			* @brief	Gets the payment date of the cashflow.
			*
			* @author	Alvaro D�az V.
			* @date	15/11/2017
			*
			* @return	A QCDate.
			*/
			QCDate date();

			/**
			* @fn	void FixedRateCashflow::setNominal(double nominal);
			*
			* @brief	Sets the nominal amount.
			*
			* @author	Alvaro D�az V.
			* @date	15/11/2017
			*
			* @param	nominal	The nominal.
			*/
			void setNominal(double nominal);

			/**
			* @fn	    tuple<QCDate, double, std::shared_ptr<QCCurrency>> SimpleCashflow::show();
			*
			* @brief	Wraps the cashflow in a SimpleCashflowWrapper
			*
			* @author	Alvaro D�az V.
			* @date	    15/11/2017
			*
			* @return	A tuple&lt;QCDate,double,shared_ptr<QCCurrency>&gt;
			*/
			shared_ptr<SimpleCashflowWrapper> wrap();

			/**
			* @fn	virtual FixedRateCashflow::~FixedRateCashflow();
			*
			* @brief	Destructor
			*
			* @author	Alvaro D�az V.
			* @date	15/11/2017
			*/
			virtual ~SimpleCashflow();

		protected:

			/** @brief	The end date */
			QCDate _endDate;

			/** @brief	The nominal */
			double _nominal;

			/** @brief	The currency */
			shared_ptr<QCCurrency> _currency;
		};
	}
}
#endif //SIMPLECASHFLOW_H