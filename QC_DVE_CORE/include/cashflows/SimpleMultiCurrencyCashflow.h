#ifndef SIMPLEMULTICURRENCYCASHFLOW_H
#define SIMPLEMULTICURRENCYCASHFLOW_H

#include "cashflows/SimpleCashflow.h"
#include "asset_classes/FXRateIndex.h"

namespace QCode
{
	namespace Financial
	{
		/**
		 * @typedef	std::tuple< QCDate, double, shared_ptr<QCCurrency>, 
		 * 			QCDate, std::shared_ptr<QCCurrency>, std::shared_ptr<FXRateIndex>, 
		 * 			double, double > SimpleMultiCurrencyCashflowWrapper
		 *
		 * @brief	Defines an alias representing the simple multi currency cashflow wrapper
		 */
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

		/**
		 * @class	SimpleMultiCurrencyCashflow
		 *
		 * @brief	A simple multi currency cashflow. A multi currency cashflow is a cashflow
		 * 			calculated in a given currency but payed/exchanged in a different currency.
		 *
		 * @author	Alvaro D�az V.
		 * @date	07/07/2018
		 */
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
			* @author	Alvaro D�az V.
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
			* @author	Alvaro D�az V.
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
			* @author	Alvaro D�az V.
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
			* @author	Alvaro D�az V.
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
			* @author	Alvaro D�az V.
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
			* @author	Alvaro D�az V.
			* @date	    16/11/2017
			*
			* @return	A tuple&lt;QCDate,double,std::shared_ptr<QCCurrency>,
			                       QCDate, std::string, std::string&gt;
			*/
			std::shared_ptr<SimpleMultiCurrencyCashflowWrapper> wrap();

			/**
			 * @fn	SimpleMultiCurrencyCashflow::~SimpleMultiCurrencyCashflow();
			 *
			 * @brief	Destructor
			 *
			 * @author	Alvaro D�az V.
			 * @date	14/09/2018
			 */
			~SimpleMultiCurrencyCashflow();

		private:

			/** @brief	The fx rate index fixing date */
			QCDate _fxRateIndexFixingDate;

			/** @brief	The settlement currency */
			std::shared_ptr<QCCurrency> _settlementCurrency;


			/** @brief	Fx rate index used to perform the calculation. */
			std::shared_ptr<FXRateIndex> _fxRateIndex;

			/** @brief	The fx rate index value */
			double _fxRateIndexValue;


			/** @brief	Message that describes errors encountered when constructing the object */
			std::string _validateMsg;
			/**
			 * @fn	bool SimpleMultiCurrencyCashflow::_validateFxRateIndex();
			 *
			 * @brief	Validates the FX rate index. It has to be an index that is compatible with
			 * 			the notional currency and the settlement currency.
			 *
			 * @author	Alvaro D�az V.
			 * @date	14/09/2018
			 *
			 * @return	True if it succeeds, false if it fails.
			 */
			bool _validate();
		};
	}
}
#endif //SIMPLEMULTICURRENCYCASHFLOW_H