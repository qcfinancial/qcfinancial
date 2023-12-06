#ifndef IBORMULTICURRENCYCASHFLOW_H
#define IBORMULTICURRENCYCASHFLOW_H

#include<tuple>
#include<memory>

#include "cashflows/IborCashflow.h"
#include "asset_classes/FXRateIndex.h"
#include "TypeAliases.h"

namespace QCode
{
	namespace Financial
	{

		typedef std::tuple<
			QCDate,                  /* Start date */
			QCDate,                  /* End date */
			QCDate,                  /* Fixing date */
			QCDate,                  /* Settlement date */
			double,                  /* Nominal */
			double,                  /* Amortization */
			double,                  /* Interest */
			bool,                    /* Amortization is cashflow */
			double,                  /* Total cashflow */
			shared_ptr<QCCurrency>,  /* Nominal currency */
			std::string,             /* Interest rate index code */
			QCInterestRate,			 /* Interest rate */
			double,                  /* Spread */
			double,                  /* Gearing */
			double,					 /* Interest rate value */
			QCDate,							/* fx index fixing date */
			std::shared_ptr<QCCurrency>,    /* settlement currency */
			std::shared_ptr<FXRateIndex>,   /* fx rate index */
			double,							/* fx rate index value */
			double,							/* amortization in settlement currency */
			double  						/* interest in settlement currency */

		> IborMultiCurrencyCashflowWrapper;

		class IborMultiCurrencyCashflow : public IborCashflow
		{
		public:

			/**
			 * @fn	IborMultiCurrencyCashflow::IborMultiCurrencyCashflow(std::shared_ptr<InterestRateIndex> index, const QCDate& startDate, const QCDate& endDate, const QCDate& fixingDate, const QCDate& settlementDate, double nominal, double amortization, bool doesAmortize, std::shared_ptr<QCCurrency> nominalCurrency, double spread, double gearing, const QCDate& fxRateIndexFixingDate, std::shared_ptr<QCCurrency> settlementCurrency, std::shared_ptr<FXRateIndex> fxRateIndex, double fxRateIndexValue = 1.0);
			 *
			 * @brief	Constructor
			 *
			 * @author	A Diaz V
			 * @date	01-03-2019
			 *
			 * @param	index				 	The associated interest rate index.
			 * @param	startDate			 	The start date.
			 * @param	endDate				 	The end date.
			 * @param	fixingDate			 	The fixing date.
			 * @param	settlementDate		 	The settlement date.
			 * @param	nominal				 	The nominal.
			 * @param	amortization		 	The amortization.
			 * @param	doesAmortize		 	Indicates whether amortization is cashflow or not.
			 * @param	nominalCurrency		 	The currency corresponding to nominal.
			 * @param	spread				 	The spread.
			 * @param	gearing				 	The gearing.
			 * @param	fxRateIndexFixingDate	The fx rate index fixing date.
			 * @param	settlementCurrency   	The settlement currency.
			 * @param	fxRateIndex			 	Fx rate index object.
			 * @param	fxRateIndexValue	 	(Optional) The fx rate index value.
			 */
			IborMultiCurrencyCashflow(std::shared_ptr<InterestRateIndex> index,
				const QCDate& startDate,
				const QCDate& endDate,
				const QCDate& fixingDate,
				const QCDate& settlementDate,
				double nominal,
				double amortization,
				bool doesAmortize,
				std::shared_ptr<QCCurrency> nominalCurrency,
				double spread,
				double gearing,
				const QCDate& fxRateIndexFixingDate,
				std::shared_ptr<QCCurrency> settlementCurrency,
				std::shared_ptr<FXRateIndex> fxRateIndex,
				double fxRateIndexValue = 1.0);

            [[nodiscard]] std::string getType() const override;

			/**
			 * @fn	double IborMultiCurrencyCashflow::amount() override;
			 *
			 * @brief	Gets the amount of the cashflow in settlement currency.
			 *
			 * @author	A Diaz V
			 * @date	01-03-2019
			 *
			 * @returns	A double.
			 */
			double amount() override;


            double settlementAmount() override;

			/**
			 * @fn	shared_ptr<QCCurrency> IborMultiCurrencyCashflow::settlementCcy();
			 *
			 * @brief	Gets the settlement currency of the cashflow.
			 *
			 * @author	A Diaz V
			 * @date	01-03-2019
			 *
			 * @returns	A shared_ptr&lt;QCCurrency&gt;
			 */
			shared_ptr<QCCurrency> settlementCurrency() override;

			/**
			 * @fn	void IborMultiCurrencyCashflow::setFxRateIndexValue(double fxRateIndexValue);
			 *
			 * @brief	Sets the value of the fx rate index value
			 *
			 * @author	A Diaz V
			 * @date	01-03-2019
			 *
			 * @param	fxRateIndexValue	The fx rate index value.
			 */
			void setFxRateIndexValue(double fxRateIndexValue);

			/**
			 * @fn	double IborMultiCurrencyCashflow::accruedInterest(const QCDate& valueDate,
			 * 		const QCDate& fxRateIndexDate, const TimeSeries& fxRateIndexValues);
			 *
			 * @brief	Calculates accrued interest in settlement currency
			 *
			 * @author	A Diaz V
			 * @date	01-03-2019
			 *
			 * @param	valueDate		 	The value date or date for which interest will be calculated.
			 * @param	fxRateIndexDate  	The FX rate index date. Corresponds to the date of the fx rate index used to express interest
			 * 								in settlement currency.
			 * @param	fxRateIndexValues	The FX rate index values.
			 *
			 * @returns	A double.
			 */
			double accruedInterest(const QCDate& valueDate, const QCDate& fxRateIndexDate, const TimeSeries& fxRateIndexValues);

			/**
			 * @fn	FXVariation IborMultiCurrencyCashflow::accruedFXVariation(const QCDate& valueDate, const TimeSeries& fxRateIndexValues);
			 *
			 * @brief	Calculates accrued FX variation expressed in settlement currency
			 *
			 * @author	A Diaz V
			 * @date	01-03-2019
			 *
			 * @param	valueDate		 	The value date or date for which interest will be calculated. This date will also be used
			 * 								for the value of the FX rate index used to convert to settlement currency.
			 * @param	fxRateIndexValues	The FX rate index values.
			 *
			 * @returns	A FXVariation.
			 */
			FXVariation accruedFXVariation(const QCDate& valueDate, const TimeSeries& fxRateIndexValues);

			/**
			 * @fn	std::shared_ptr<IborMultiCurrencyCashflowWrapper> IborMultiCurrencyCashflow::wrap();
			 *
			 * @brief	Wraps the cashflow in a IborMultiCurrencyCashflowWrapper
			 *
			 * @author	A Diaz V
			 * @date	01-03-2019
			 *
			 * @returns	A std::shared_ptr<IborMultiCurrencyCashflowWrapper>;
			 */
			std::shared_ptr<IborMultiCurrencyCashflowWrapper> wrap();

            QCDate getFXFixingDate() const;

			/**
			 * @fn	virtual IborMultiCurrencyCashflow::~IborMultiCurrencyCashflow();
			 *
			 * @brief	Destructor
			 *
			 * @author	A Diaz V
			 * @date	01-03-2019
			 */
			virtual ~IborMultiCurrencyCashflow();

		protected:

			/**
			 * @fn	bool IborMultiCurrencyCashflow::_validate();
			 *
			 * @brief	Validates that the cashflow is properly constructed
			 *
			 * @author	A Diaz V
			 * @date	01-03-2019
			 *
			 * @returns	True if it succeeds, false if it fails.
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

#endif //IBORMULTICURRENCYCASHFLOW_H