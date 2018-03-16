#ifndef ICPCLFCASHFLOW_H
#define ICPCLFCASHFLOW_H

#include<tuple>
#include<vector>
#include<memory>

#include "IcpClpCashflow.h"
#include "QCInterestRate.h"
#include "QCAct360.h"
#include "QCLinearWf.h"

namespace QCode
{
	namespace Financial
	{
		const double DEFAULT_UF = 27000.00;
		const std::vector<double> DEFAULT_ICP_UF = { DEFAULT_ICP, DEFAULT_ICP, DEFAULT_UF, DEFAULT_UF };
		typedef std::tuple<
			QCDate,                 /* Start Date */
			QCDate,                 /* End Date */
			QCDate,                 /* Settlement Date */
			double,                 /* Nominal */
			double,                 /* Amortization */
			bool,                   /* Amortization is cashflow */
			shared_ptr<QCCurrency>, /* Nominal Currency (always CLP) */
			double,                 /* Start date ICP value */
			double,                 /* End date ICP value */
			double,                 /* Start date UF value */
			double,                 /* End date UF value */
			double,                 /* Rate */
			double,                 /* Interest */
			double,                 /* Spread */
			double                  /* Gearing */
		> IcpClfCashflowWrapper;

		class IcpClfCashflow : public IcpClpCashflow
		{
		public:
			/**
			* @fn	IcpClfCashflow::IcpClfCashflow(const QCDate& startDate, const QCDate& endDate,
			*                                      const QCDate& settlementDate,
			*                                      double nominal, double amortization, bool doesAmortize,
			*                                      double spread, double gearing,
			*                                      double startDateICP, double endDateICP);
			*
			* @brief	Constructor
			*
			* @author	Alvaro Díaz V.
			* @date	    24/11/2017
			*
			* @param	startDate	  	The start date.
			* @param	endDate		  	The end date.
			* @param	settlementDate	The settlement date.
			* @param	nominal		  	The nominal.
			* @param	amortization  	The amortization.
			* @param	doesAmortize  	Indicates whether amortization is cashflow or not.
			* @param	currency	  	The currency.
			* @param	spread		  	The spread.
			* @param	gearing		  	The gearing.
			*/
			IcpClfCashflow(const QCDate& startDate,
						   const QCDate& endDate,
						   const QCDate& settlementDate,
						   double nominal,
						   double amortization,
						   bool doesAmortize,
						   double spread,
						   double gearing,
						   std::vector<double> icpAndUf = DEFAULT_ICP_UF);

			/**
			* @fn	    double IcpClfCashflow::amount();
			*
			* @brief	Gets the amount
			*
			* @author	Alvaro Díaz V.
			* @date	    24/11/2017
			*
			* @return	A double.
			*/
			double amount();

			/**
			* @fn	    double IcpClfCashflow::accruedInterest(QCDate& accrualDate, double icpValue);
			*
			* @brief	Gets the value of accrued interest at given date, given the value of ICP.
			*
			* @author	Alvaro Díaz V.
			* @date	    24/11/2017
			*
			* @return	A double.
			*/
			double accruedInterest(QCDate& accrualDate, double icpValue, double ufValue);

			/**
			* @fn	    void IcpClfCashflow::setStartDateUf(double ufValue);
			*
			* @brief	Sets the value of start date UF
			*
			* @author	Alvaro Díaz V.
			* @date	    24/11/2017
			*
			*/
			void setStartDateUf(double ufValue);

			/**
			* @fn	    void IcpClfCashflow::setEndDateUf(double ufValue);
			*
			* @brief	Sets the value of end date Uf
			*
			* @author	Alvaro Díaz V.
			* @date	    24/11/2017
			*
			*/
			void setEndDateUf(double ufValue);

			/**
			* @fn	    shared_ptr<QCCurrency> IcpClfCashflow::ccy();
			*
			* @brief	Gets the ccy which in this case is always CLF
			*
			* @author	Alvaro Díaz V.
			* @date	    24/11/2017
			*
			* @return	A shared_ptr&lt;QCCurrency&gt;
			*/
			shared_ptr<QCCurrency> ccy();

			/**
			* @fn	shared_ptr<IcpClfCashflowWrapper> IcpClfCashflow::wrap();
			*
			* @brief	Wraps the cashflow in a IcpClpCashflowWrapper
			*
			* @author	Alvaro Díaz V.
			* @date	    24/11/2017
			*
			* @return	A shared_ptr&lt;IcpClfCashflowWrapper&gt;
			*/
			shared_ptr<IcpClfCashflowWrapper> wrap();

			/**
			* @fn	IcpClpCashflow::~IcpClfCashflow();
			*
			* @brief	Destructor
			*
			* @author	Alvaro Díaz V.
			* @date	    24/11/2017
			*
			*/
			virtual ~IcpClfCashflow();

		private:
			/** @brief	Calculates interest (final or accrued) given a date, ICP value and UF value. */
			double _calculateInterest(QCDate& date, double icpValue, double ufValue);

			/** @brief	UF at start date */
			double _startDateUF;

			/** @brief	UF at end date */
			double _endDateUF;
		};
	}
}
#endif //ICPCLPCASHFLOW_H