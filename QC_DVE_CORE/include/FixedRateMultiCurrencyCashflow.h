#ifndef FIXEDRATEMULTICURRENCYCASHFLOW_H
#define FIXEDRATEMULTICURRENCYCASHFLOW_H

#include<tuple>
#include<memory>

#include "FixedRateCashflow.h"
#include "FXRateIndex.h"

namespace QCode
{
	namespace Financial
	{
		/**
		 * @typedef	std::tuple< QCDate, QCDate, QCDate, double, double, double, bool, shared_ptr<QCCurrency>, QCInterestRate, QCDate, std::shared_ptr<QCCurrency>, std::shared_ptr<FXRateIndex>, double, double, double, > FixedRateMultiCurrencyCashflowWrapper
		 *
		 * @brief	Defines an alias representing the fixed rate multi currency cashflow wrapper
		 */
		typedef std::tuple<
			QCDate,							/* start date 0 */
			QCDate,							/* end date 1 */
			QCDate,							/* settlement date 2 */
			double,							/* notional 3 */
			double,							/* amortization 4 */
			double,							/* interest 5 */
			bool,							/* amortization is cashflow 6 */
			shared_ptr<QCCurrency>,			/* notional currency 7 */
			QCInterestRate,                 /* interest rate 8 */
			QCDate,							/* fx index fixing date 9 */
			std::shared_ptr<QCCurrency>,    /* settlement currency 10 */
			std::shared_ptr<FXRateIndex>,   /* fx rate index 11 */
			double,							/* fx rate index value 12 */
			double,							/* amortization in settlement currency 13 */
			double  						/* interest in settlement currency 14 */
		> FixedRateMultiCurrencyCashflowWrapper;

		class FixedRateMultiCurrencyCashflow : public FixedRateCashflow
		{
		public:

			/**
			 * @enum	element
			 *
			 * @brief	Values that represent the elements of a FixedRateMultiCurrencyCashflowWrapper
			 */
			enum element
			{
				///< An enum constant representing the start date option
				startDate,

				///< An enum constant representing the end date option
				endDate,

				///< An enum constant representing the settlement date option
				settDate,

				///< An enum constant representing the notional option
				notional,

				///< An enum constant representing the amortization option
				amortization,

				///< An enum constant representing the interest option
				interest,

				///< An enum constant representing the amortization is cashflow option
				amortIsCashflow,

				///< An enum constant representing the notional currency option
				notionalCurrency,

				///< An enum constant representing the interest rate option
				interestRate,

				///< An enum constant representing the fx index fixing date option
				fxIndexFixingDate,

				///< An enum constant representing the settlement currency option
				settCurrency,

				///< An enum constant representing the fx rate index code option
				fxRateIndexCode,

				///< An enum constant representing the fx rate index value option
				fxRateIndexValue,

				///< An enum constant representing the amortization in settlement currency option
				amortInSettCurrency,

				///< An enum constant representing the interest in settlement currency option
				interestInSettCurrency
			};

			/**
			 * @fn	FixedRateMultiCurrencyCashflow::FixedRateMultiCurrencyCashflow(
			 * 		const QCDate& startDate, const QCDate& endDate, const QCDate& settlementDate, 
			 * 		double nominal, double amortization, bool doesAmortize, const QCInterestRate& rate, 
			 * 		std::shared_ptr<QCCurrency> notionalCurrency, const QCDate& fxRateIndexFixingDate, 
			 * 		std::shared_ptr<QCCurrency> settlementCurrency, 
			 * 		std::shared_ptr<FXRateIndex> fxRateIndex, double fxRateIndexValue = 1.0);
			 *
			 * @brief	Constructor
			 *
			 * @author	Alvaro Díaz V.
			 * @date	19/07/2018
			 *
			 * @param	startDate			 	The start date.
			 * @param	endDate				 	The end date.
			 * @param	settlementDate		 	The settlement date.
			 * @param	nominal				 	The nominal.
			 * @param	amortization		 	The amortization.
			 * @param	doesAmortize		 	True to does amortize.
			 * @param	rate				 	The rate.
			 * @param	notionalCurrency	 	The notional currency.
			 * @param	fxRateIndexFixingDate	The fx rate index fixing date.
			 * @param	settlementCurrency   	The settlement currency.
			 * @param	fxRateIndex			 	The fx rate index.
			 * @param	fxRateIndexValue	 	(Optional) The fx rate index value.
			 */
			FixedRateMultiCurrencyCashflow(const QCDate& startDate,
				const QCDate& endDate,
				const QCDate& settlementDate,
				double nominal,
				double amortization,
				bool doesAmortize,
				const QCInterestRate& rate,
				std::shared_ptr<QCCurrency> notionalCurrency,
				const QCDate& fxRateIndexFixingDate,
				std::shared_ptr<QCCurrency> settlementCurrency,
				std::shared_ptr<FXRateIndex> fxRateIndex,
				double fxRateIndexValue = 1.0);

			/**
			 * @fn		double FixedRateMultiCurrencyCashflow::amount();
			 *
			 * @brief	Gets the amount of the cashflow in settlement currency
			 *
			 * @author	Alvaro Díaz V.
			 * @date	19/07/2018
			 *
			 * @return	A double.
			 */
			double amount() override;

			/**
			 * @fn		shared_ptr<QCCurrency> FixedRateMultiCurrencyCashflow::settlementCcy();
			 *
			 * @brief	Gets the settlement currency of the cashflow
			 *
			 * @author	Alvaro Díaz V.
			 * @date	19/07/2018
			 *
			 * @return	A shared_ptr&lt;QCCurrency&gt;
			 */
			shared_ptr<QCCurrency> settlementCcy();

			/**
			 * @fn		void FixedRateMultiCurrencyCashflow::setFxRateIndexValue(double fxRateIndexValue);
			 *
			 * @brief	Sets fx rate index value
			 *
			 * @author	Alvaro Díaz V.
			 * @date	19/07/2018
			 *
			 * @param	fxRateIndexValue	The fx rate index value.
			 */
			void setFxRateIndexValue(double fxRateIndexValue);

			/**
			 * @fn		std::shared_ptr<FixedRateMultiCurrencyCashflowWrapper> FixedRateMultiCurrencyCashflow::wrap();
			 *
			 * @brief	Wraps the cashflow in a FixedRateMultiCurrencyCashflowWrapper
			 *
			 * @author	Alvaro Díaz V.
			 * @date	19/07/2018
			 *
			 * @return	A std::shared_ptr&lt;FixedRateMultiCurrencyCashflowWrapper&gt;
			 */
			std::shared_ptr<FixedRateMultiCurrencyCashflowWrapper> wrap();

			/**
			 * @fn	virtual FixedRateMultiCurrencyCashflow::~FixedRateMultiCurrencyCashflow();
			 *
			 * @brief	Destructor
			 *
			 * @author	Alvaro Díaz V.
			 * @date	19/07/2018
			 */
			virtual ~FixedRateMultiCurrencyCashflow();

		protected:

			/**
			 * @fn	    bool FixedRateMultiCurrencyCashflow::_validate();
			 *
			 * @brief	Validates that the object is properly constructed.
			 *
			 * @author	Alvaro Díaz V.
			 * @date	23/09/2018
			 *
			 * @return	True if it succeeds, false if it fails.
			 */
			bool _validate();

		private:
			/** @brief	The effects rate index fixing date */
			QCDate _fxRateIndexFixingDate;

			/** @brief	The settlement currency */
			std::shared_ptr<QCCurrency> _settlementCurrency;

			/** @brief	Fx rate index used to perform the calculation. */
			std::shared_ptr<FXRateIndex> _fxRateIndex;

			/** @brief	The fx rate index value */
			double _fxRateIndexValue;

		};

	}
}

#endif //FIXEDRATEMULTICURRENCYCASHFLOW_H
