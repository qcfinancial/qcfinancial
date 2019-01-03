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
		const unsigned int DEFAULT_TRA_DECIMAL_PLACES = 6;

		const std::vector<double> DEFAULT_ICP_UF = { DEFAULT_ICP, DEFAULT_ICP, DEFAULT_UF, DEFAULT_UF };
		typedef std::tuple<
			QCDate,                 /* Start Date */
			QCDate,                 /* End Date */
			QCDate,                 /* Settlement Date */
			double,                 /* Nominal */
			double,                 /* Amortization */
			bool,                   /* Amortization is cashflow */
			shared_ptr<QCCurrency>, /* Nominal Currency (always CLF) */
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
			* @fn	    void IcpClfCashflow::setTraDecimalPlaces(unsigned int decimalPlaces);
			*
			* @brief	Sets the number of decimal places for TRA.
			*
			* @author	Alvaro Díaz V.
			* @date	    24/09/2018
			*
			* @param	decimalPlaces	The decimal places.
			*/
			void setTraDecimalPlaces(unsigned int decimalPlaces);

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
			 * @fn	    double IcpClfCashflow::getRateValue();
			 *
			 * @brief	Gets the value of the associated interest rate
			 *
			 * @author	Alvaro Díaz V.
			 * @date	25/09/2018
			 *
			 * @return	The rate value.
			 */
			double getRateValue();

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
			 * @fn	    double IcpClfCashflow::getTra(QCDate& accrualDate, double icpValue, double ufValue);
			 *
			 * @brief	Calculates and returns the TRA of the cashflow for the given date, icpValue
			 * 			and ufValue (both of which correspond to date).
			 *
			 * @author	Alvaro Díaz V.
			 * @date	25/09/2018
			 *
			 * @param [in,out]	accrualDate	The accrual date.
			 * @param 		  	icpValue   	The icp value.
			 * @param 		  	ufValue	   	The uf value.
			 *
			 * @return	The tra.
			 */
			double getTra(QCDate& accrualDate, double icpValue, double ufValue);

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
			 * @fn	double IcpClfCashflow::getStartDateUf() const;
			 *
			 * @brief	Gets the value of UF start date
			 *
			 * @author	Alvaro Díaz V.
			 * @date	25/09/2018
			 *
			 * @return	The start date uf.
			 */
			double getStartDateUf() const;

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
			 * @fn	    double IcpClfCashflow::getEndDateUf() const;
			 *
			 * @brief	Gets the value of UF at end date
			 *
			 * @author	Alvaro Díaz V.
			 * @date	25/09/2018
			 *
			 * @return	The end date uf.
			 */
			double getEndDateUf() const;

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
			/** @brief	The interest rate index. It is always Lin ACT360. */
			QCInterestRate 	_rate = QCInterestRate(0.0,
				                                   std::make_shared<QCAct360>(QCAct360()),
												   std::make_shared<QCLinearWf>(QCLinearWf()));

			/** @brief	The default amount of decimal places for TRA. */
			unsigned int _traDecimalPlaces;

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