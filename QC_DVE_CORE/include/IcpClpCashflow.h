#ifndef ICPCLPCASHFLOW_H
#define ICPCLPCASHFLOW_H

#define DEFAULT_ICP 10000.00

#include<tuple>
#include<memory>

#include "Cashflow.h"
#include "QCInterestRate.h"
#include "QCAct360.h"
#include "QCLinearWf.h"

namespace QCode
{
	namespace Financial
	{
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
			double,                 /* Rate */
			double,                 /* Interest */
			double,                 /* Spread */
			double                  /* Gearing */
		> IcpClpCashflowWrapper;

		class IcpClpCashflow : public Cashflow
		{
		public:
			/**
			* @fn	IcpClpCashflow::IcpClpCashflow(const QCDate& startDate, const QCDate& endDate,
			*                                      const QCDate& settlementDate,
			*                                      double nominal, double amortization, bool doesAmortize,
			*                                      double spread, double gearing);
			*
			* @brief	Constructor
			*
			* @author	Alvaro Díaz V.
			* @date	    17/11/2017
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
			IcpClpCashflow(const QCDate& startDate,
						   const QCDate& endDate,
						   const QCDate& settlementDate,
						   double nominal,
						   double amortization,
						   bool doesAmortize,
						   double spread,
						   double gearing,
						   double startDateICP = DEFAULT_ICP,
						   double endDateICP = DEFAULT_ICP);

			/**
			* @fn	    double IcpClpCashflow::amount();
			*
			* @brief	Gets the amount
			*
			* @author	Alvaro Díaz V.
			* @date	    17/11/2017
			*
			* @return	A double.
			*/
			double amount();

			/**
			* @fn	    double IcpClpCashflow::accruedInterest(QCDate& accrualDate, double icpValue);
			*
			* @brief	Gets the value of accrued interest at given date, given the value of ICP.
			*
			* @author	Alvaro Díaz V.
			* @date	    17/11/2017
			*
			* @return	A double.
			*/
			double accruedInterest(QCDate& accrualDate, double icpValue);

			/**
			* @fn	    void IcpClpCashflow::setStartDateIcp(double icpValue);
			*
			* @brief	Sets the value of start date ICP
			*
			* @author	Alvaro Díaz V.
			* @date	    21/11/2017
			*
			*/
			void setStartDateICP(double icpValue);

			/**
			* @fn	    void IcpClpCashflow::setEndDateIcp(double icpValue);
			*
			* @brief	Sets the value of end date ICP
			*
			* @author	Alvaro Díaz V.
			* @date	    21/11/2017
			*
			*/
			void setEndDateICP(double icpValue);

			/**
			* @fn	    shared_ptr<QCCurrency> IcpClpCashflow::ccy();
			*
			* @brief	Gets the ccy which in this case is always CLP
			*
			* @author	Alvaro Díaz V.
			* @date	    17/11/2017
			*
			* @return	A shared_ptr&lt;QCCurrency&gt;
			*/
			shared_ptr<QCCurrency> ccy();

			/**
			* @fn	    QCDate IcpClpCashflow::date();
			*
			* @brief	Gets the date
			*
			* @author	Alvaro Díaz V.
			* @date	    17/11/2017
			*
			* @return	A QCDate.
			*/
			QCDate date();

			/**
			* @fn	    void IcpClpCashflow::setNominal(double nominal);
			*
			* @brief	Sets the nominal amount.
			*
			* @author	Alvaro Díaz V.
			* @date	    17/11/2017
			*
			* @param	nominal	The nominal.
			*/
			void setNominal(double nominal);

			/**
			* @fn	    void IcpClpCashflow::setAmortization(double amortization);
			*
			* @brief	Sets the amortization
			*
			* @author	Alvaro Díaz V.
			* @date	    17/11/2017
			*
			* @param	amortization	The amortization.
			*/
			void setAmortization(double amortization);

			/**
			* @fn	shared_ptr<IcpClpCashflowWrapper> IcpClpCashflow::wrap();
			*
			* @brief	Wraps the cashflow in a IcpClpCashflowWrapper
			*
			* @author	Alvaro Díaz V.
			* @date	    21/11/2017
			*
			* @return	A shared_ptr&lt;IcpClpCashflowWrapper&gt;
			*/
			shared_ptr<IcpClpCashflowWrapper> wrap();

			/**
			* @fn	    double IcpClpCashflow::getRateValue();
			*
			* @brief	Gets the value of the associated interest rate
			*
			* @author	Alvaro Díaz V.
			* @date	    17/11/2017
			*
			* @return   A double
			*/
			double getRateValue();

			/**
			* @fn	    double IcpClpCashflow::getTypeOfRate();
			*
			* @brief	Gets the type of the associated interest rate as string (LinAct360 ...)
			*
			* @author	Alvaro Díaz V.
			* @date	    17/11/2017
			*
			* @return   A string
			*/
			std::string getTypeOfRate();

			/**
			* @fn	IcpClpCashflow::~IcpClpCashflow();
			*
			* @brief	Destructor
			*
			* @author	Alvaro Díaz V.
			* @date	    17/11/2017
			*
			*/
			virtual ~IcpClpCashflow();
		
		private:
			/** @brief	The interest rate index. It is always Lin ACT360. */
			QCInterestRate 	_rate = QCInterestRate(0.0,
												   std::make_shared<QCAct360>(QCAct360()),
												   std::make_shared<QCLinearWf>(QCLinearWf())
												   );

			/** @brief	Calculates interest (final or accrued) given a date and ICP value. */
			double _calculateInterest(QCDate& date, double icpValue);

			/** @brief	ICP at start date */
			double _startDateICP;

			/** @brief	ICP at end date */
			double _endDateICP;

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

			/** @brief	The nominal currency. It is always CLP. */
			shared_ptr<QCCurrency> _currency;

			/** @brief	The spread */
			double _spread;

			/** @brief	The gearing */
			double _gearing;

		};
	}
}
#endif //ICPCLPCASHFLOW_H