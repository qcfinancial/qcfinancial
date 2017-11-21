#ifndef SIMPLEMULTICURRENCYCASHFLOW_H
#define SIMPLEMULTICURRENCYCASHFLOW_H

#include "SimpleCashflow.h"
#include "FXRateIndex.h"

namespace QCode
{
	namespace Financial
	{
		typedef std::tuple<
			QCDate,                        /* end date */
			double,                        /* nominal */
			shared_ptr<QCCurrency>,        /* nominal currency*/
			QCDate,                        /* FX Rate fixing date */
			std::shared_ptr<QCCurrency>,   /* Settlement currency */
			std::shared_ptr<FXRateIndex>,  /* FX Rate Index code */
			double,                        /* FX Rate Index value */
			double                         /* Amount */
		> SimpleMultiCurrencyCashflowWrapper;

		class SimpleMultiCurrencyCashflow : public SimpleCashflow
		{
		public:
			/**
			* @fn	    SimpleMultiCurrencyCashflow::SimpleCashflow(const QCDate& endDate,
			*                                                   double amount, const QCCurrency& currency
			*                                                   const QCDate& fxRateIndexFixingDate,
			*                                                   std::shared_ptr<QCCurrency> settlementCurrency,
			*                                                   std::shared_ptr<FXRateIndex> fxRateIndex);
			*
			* @brief	Constructor. The parameter endDate is the date in which the cashflow
			* 			actually occurs.
			*
			* @author	Alvaro Díaz V.
			* @date	    16/11/2017
			*
			* @param	endDate		  	            The end date.
			* @param	nominal		  	            The nominal or amount of cashflow.
			* @param	currency	  	            The currency.
			* @param	fxRateIndexFixingDate	  	The date corresponding to the FX Rate Index value.
			* @param	settlementCurrency	  	    The currency in which the cashflow occurs.
			* @param	fxRateIndex	  	            The FXRateIndex.
			*/
			SimpleMultiCurrencyCashflow(const QCDate& endDate,
										double nominal,
										std::shared_ptr<QCCurrency> currency,
										const QCDate& fxRateIndexFixingDate,
										std::shared_ptr<QCCurrency> settlementCurrency,
										std::shared_ptr<FXRateIndex> fxRateIndex,
										double fxRateIndexValue = 1.0);

			/**
			* @fn	    double SimpleMultiCurrencyCashflow::amount();
			*
			* @brief	Gets the amount of the cashflow in settlement currency.
			*
			* @author	Alvaro Díaz V.
			* @date	16/11/2017
			*
			* @return	A double.
			*/
			double amount();

			/**
			* @fn	    double SimpleMultiCurrencyCashflow::nominal();
			*
			* @brief	Gets the amount of nominal in nominal currency.
			*
			* @author	Alvaro Díaz V.
			* @date	16/11/2017
			*
			* @return	A double.
			*/
			double nominal();

			/**
			* @fn	    QCCurrency SimpleMultiCurrencyCashflow::ccy();
			*
			* @brief	Gets the settlement currency of the cashflow.
			*
			* @author	Alvaro Díaz V.
			* @date	    16/11/2017
			*
			* @return	A QCCurrency.
			*/
			shared_ptr<QCCurrency> settlementCcy();

			/**
			* @fn	    QCCurrency SimpleMultiCurrencyCashflow::setFxRateIndexValue();
			*
			* @brief	Sets the value of the FX Rate Index.
			*
			* @author	Alvaro Díaz V.
			* @date	    16/11/2017
			*
			* @param	fxRateIndexValue The value of the Fx Rate.
			*/
			void setFxRateIndexValue(double fxRateIndexValue);

			/**
			* @fn	    tuple<QCDate, double, std::shared_ptr<QCCurrency>,
			                  QCDate, std::string, std::string> SimpleMultiCurrencyCashflow::show();
			*
			* @brief	Wraps the cashflow in a SimpleMultiCurrencyCashflowWrapper
			*
			* @author	Alvaro Díaz V.
			* @date	    16/11/2017
			*
			* @return	A tuple&lt;QCDate,double,std::shared_ptr<QCCurrency>,
			                       QCDate, std::string, std::string&gt;
			*/
			std::shared_ptr<SimpleMultiCurrencyCashflowWrapper> wrap();

		private:
			QCDate _fxRateIndexFixingDate;
			std::shared_ptr<QCCurrency> _settlementCurrency;
			std::shared_ptr<FXRateIndex> _fxRateIndex;
			double _fxRateIndexValue;

		};
	}
}
#endif //SIMPLEMULTICURRENCYCASHFLOW_H