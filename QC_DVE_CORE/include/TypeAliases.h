#ifndef TYPEALIASES_H
#define TYPEALIASES_H

#include <map>
#include <string>
#include <tuple>

#include "time/QCDate.h"
#include "asset_classes/QCInterestRate.h"
#include "asset_classes/QCCurrency.h"

namespace QCode
{
	namespace Financial
	{
		/**
		* @typedef	std::vector<QCDate> DateList
		*
		* @brief	Defines an alias representing a list (std::vector) of dates.
		*/
		// typedef std::vector<QCDate> DateList;

		/** @brief	The default FX rate index value used when constructing multicurrency cashflows*/
		const double DEFAULT_FX_RATE_INDEX_VALUE = 1.00;

		/**
		 * @typedef	std::map<QCDate, double> TimeSeries
		 *
		 * @brief	Defines an alias representing a time series. The key is a date represented by a QCDate object and the value is a double.
		 */
		typedef std::map<QCDate, double> TimeSeries;
		
		/**
		 * @struct	FXVariation
		 *
		 * @brief	Provides a container for the FX variation (reajustes) associated to interest and
		 * 			nominal for interest rate type cashflows.
		 *
		 * @author	A Diaz V
		 * @date	28-02-2019
		 */
		struct FXVariation
		{
			FXVariation(double interest, double nominal) : interestVariation(interest), nominalVariation(nominal)
			{
			}

			double interestVariation;
			double nominalVariation;
		};

		/**
		* @typedef	std::tuple<QCDate, QCDate, QCDate, double, double, double, bool, shared_ptr<QCCurrency>, QCInterestRate> FixedRateCashflowWrapper
		*
		* @brief	Defines an alias representing a tuple which in turn represents a FixedRateCashflow.
		* 			The fields in the tuple correspond to:
		* 			start date, end date, settlement date, nominal, amortization, total flow, amortization is cashflow, currency and interest rate. 
		*/
		typedef std::tuple<QCDate,
			QCDate,
			QCDate,
			double,
			double,
			double,
			bool,
			shared_ptr<QCCurrency>,
			QCInterestRate> FixedRateCashflowWrapper;
	}
}

#endif // TYPEALIASES_H