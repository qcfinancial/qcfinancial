#ifndef IBORMULTICURRENCYCASHFLOW_H
#define IBORMULTICURRENCYCASHFLOW_H

#include<tuple>
#include<memory>

#include "IborCashflow.h"
#include "FXRateIndex.h"
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
			std::shared_ptr<FXRateIndex>,   /* fx rate index */
			double,							/* fx rate index value */
			double,							/* amortization in settlement currency */
			double  						/* interest in settlement currency */

		> IborMultiCurrencyCashflowWrapper;

		class IborMultiCurrencyCashflow : public IborCashflow
		{
		public:

		};
	}
}

#endif //IBORMULTICURRENCYCASHFLOW_H