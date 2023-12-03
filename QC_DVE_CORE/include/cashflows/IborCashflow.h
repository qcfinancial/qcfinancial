#ifndef IBORCASHFLOW_H
#define IBORCASHFLOW_H

#include<tuple>
#include<memory>
#include<string>

#include "cashflows/Cashflow.h"
#include "time/QCDate.h"
#include "asset_classes/QCInterestRate.h"
#include "asset_classes/QCCurrency.h"
#include "asset_classes/InterestRateIndex.h"

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
			double					 /* Interest rate value */
		> IborCashflowWrapper;

		/**
		 * @class	IborCashflow
		 *
		 * @brief	An Ibor based cashflow.
		 *
		 * @author	Alvaro Diaz V.
		 * @date	29/09/2017
		 */
		class IborCashflow : public Cashflow
		{
		public:

			/**
			 * @fn	IborCashflow::IborCashflow(std::shared_ptr<InterestRateIndex> index,
			 *                                 const QCDate& startDate, const QCDate& endDate,
			 *                                 const QCDate& fixingDate, const QCDate& settlementDate,
			 *                                 double nominal, double amortization, bool doesAmortize,
			 *                                 const QCInterestRate& rate, std::shared_ptr<QCCurrency> currency,
			 *                                 double spread, double gearing);
			 *
			 * @brief	Constructor
			 *
			 * @author	Alvaro Díaz V.
			 * @date	29/09/2017
			 *
			 * @param	iborIndexCode 	The associated interest rate index.
			 * @param	startDate	  	The start date.
			 * @param	endDate		  	The end date.
			 * @param	fixingDate	  	The fixing date.
			 * @param	settlementDate	The settlement date.
			 * @param	nominal		  	The nominal.
			 * @param	amortization  	The amortization.
			 * @param	doesAmortize  	Indicates whether amortization is cashflow or not.
			 * @param	rate		  	The rate.
			 * @param	currency	  	The currency.
			 * @param	spread		  	The spread.
			 * @param	gearing		  	The gearing.
			 */
			IborCashflow(std::shared_ptr<InterestRateIndex> index,
						 const QCDate& startDate,
						 const QCDate& endDate,
						 const QCDate& fixingDate,
						 const QCDate& settlementDate,
						 double nominal,
						 double amortization,
						 bool doesAmortize,
						 std::shared_ptr<QCCurrency> currency,
						 double spread,
						 double gearing);

            virtual std::string getType() const;

			/**
			 * @fn	double IborCashflow::amount();
			 *
			 * @brief	Gets the amount.
			 *
			 * @author	Alvaro Díaz V.
			 * @date	29/09/2017
			 *
			 * @return	A double.
			 */
			double amount() override;


            virtual double settlementAmount();


            virtual shared_ptr<QCCurrency> settlementCurrency();

			/**
			 * @fn	shared_ptr<QCCurrency> IborCashflow::ccy();
			 *
			 * @brief	Gets the ccy
			 *
			 * @author	Alvaro Díaz V.
			 * @date	29/09/2017
			 *
			 * @return	A shared_ptr&lt;QCCurrency&gt;
			 */
			shared_ptr<QCCurrency> ccy() override;

			/**
			 * @fn	QCDate IborCashflow::date();
			 *
			 * @brief	Gets the date
			 *
			 * @author	Alvaro Díaz V.
			 * @date	29/09/2017
			 *
			 * @return	A QCDate.
			 */
			QCDate date() override;

			/**
			 * @fn	QCDate IborCashflow::getStartDate();
			 *
			 * @brief	Gets the start date of the cashflow
			 *
			 * @author	Alvaro Díaz V.
			 * @date	23/09/2018
			 *
			 * @return	The start date.
			 */
			QCDate getStartDate() const;

			/**
			 * @fn	QCDate IborCashflow::getEndDate() const;
			 *
			 * @brief	Gets the end date of the cashflow (date used to calculate interest)
			 *
			 * @author	Alvaro Díaz V.
			 * @date	23/09/2018
			 *
			 * @return	The end date.
			 */
			QCDate getEndDate() const;

			/**
			* @fn	    QCDate IborCashflow::getFixingDate();
			*
			* @brief	Returns the fixing date.
			*
			* @author	Alvaro Díaz V.
			* @date	    05/12/2017
			*
			* @return	QCDate;
			*/
			QCDate getFixingDate() const;

			/**
			 * @fn	QCDate IborCashflow::getIndexStartDate() const;
			 *
			 * @brief	Gets the interest rate index start date
			 *
			 * @author	A Diaz V
			 * @date	21-01-2019
			 *
			 * @returns	The index start date.
			 */
			QCDate getIndexStartDate();

			/**
			 * @fn	QCDate IborCashflow::getindexEndDate() const;
			 *
			 * @brief	Get the interest rate index end date
			 *
			 * @author	A Diaz V
			 * @date	21-01-2019
			 *
			 * @returns	A QCDate.
			 */
			QCDate getIndexEndDate();

			/**
			* @fn	void iborCashflow::setNominal(double nominal);
			*
			* @brief	Sets the nominal amount.
			*
			* @author	Alvaro Díaz V.
			* @date	05/10/2017
			*
			* @param	nominal	The nominal.
			*/
			void setNominal(double nominal);

			/**
			 * @fn	double IborCashflow::getNominal() const;
			 *
			 * @brief	Gets the nominal
			 *
			 * @author	Alvaro Díaz V.
			 * @date	23/09/2018
			 *
			 * @return	The nominal.
			 */
			double getNominal() const;

			/**
			* @fn	void iborCashflow::setAmortization(double amortization);
			*
			* @brief	Sets the amortization
			*
			* @author	Alvaro Díaz V.
			* @date	05/10/2017
			*
			* @param	amortization	The amortization.
			*/
			void setAmortization(double amortization);

			/**
			 * @fn	double IborCashflow::getAmortization() const;
			 *
			 * @brief	Gets the amortization
			 *
			 * @author	Alvaro Díaz V.
			 * @date	23/09/2018
			 *
			 * @return	The amortization.
			 */
			double getAmortization() const;

			/**
			* @fn	void iborCashflow::setInterestRateValue(double value);
			*
			* @brief	Sets the rate value of the interest rate index 
			*
			* @author	Alvaro Díaz V.
			* @date	    05/12/2017
			*
			* @param	value	The value of the interest rate.
			*/
			void setInterestRateValue(double value);

			/**
			 * @fn	double IborCashflow::getInterestRateValue() const;
			 *
			 * @brief	Gets the value of the interest rate index.
			 *
			 * @author	Alvaro Díaz V.
			 * @date	24/09/2018
			 *
			 * @return	The interest rate value.
			 */
			double getInterestRateValue() const;

			/**
			 * @fn	double IborCashflow::accruedInterest(const QCDate& valueDate);
			 *
			 * @brief	Gets the accrued interest given a value date.
			 *
			 * @author	Alvaro Díaz V.
			 * @date	23/09/2018
			 *
			 * @param	valueDate	The value date.
			 *
			 * @return	A double.
			 */
			double accruedInterest(const QCDate& valueDate);

			/**
			 * @fn	shared_ptr<IborCashflowWrapper> IborCashflow::wrap();
			 *
			 * @brief	Wraps the cashflow in a IborCashflowWrapper
			 *
			 * @author	Alvaro Díaz V.
			 * @date	29/09/2017
			 *
			 * @return	A shared_ptr&lt;IborCashflowWrapper&gt;
			 */
			shared_ptr<IborCashflowWrapper> wrap();

			/**
			 * @fn	virtual IborCashflow::~IborCashflow();
			 *
			 * @brief	Destructor
			 *
			 * @author	Alvaro Díaz V.
			 * @date	29/09/2017
			 */
			virtual ~IborCashflow();

		protected:
			/** @brief	The interest rate index value */
			double _rateValue;

			/**
			 * @fn	void IborCashflow::_calculateInterest();
			 *
			 * @brief	Calculates the interest using (rate*gearing + spread)
			 *
			 * @author	Alvaro Díaz V.
			 * @date	29/09/2017
			 */
			void _calculateInterest();

			/** @brief	The associated interest rate index */
			std::shared_ptr<InterestRateIndex> _index;

			/** @brief	The start date */
			QCDate _startDate;

			/** @brief	The end date */  
			QCDate _endDate;

			/** @brief	The fixing date */
			QCDate _fixingDate;

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

			/** @brief	The currency */
			shared_ptr<QCCurrency> _currency;

			/** @brief	The spread */
			double _spread;

			/** @brief	The gearing */
			double _gearing;

			/** @brief	Message that describes errors encountered when constructing the object */
			std::string _validateMsg;

			/**
			 * @fn	bool IborCashflow::_validate();
			 *
			 * @brief	Validates that the object is properly constructed. More precisely,
			 * 			the following checks are performed:
			 * 			- startDate < endDate  
			 * 			- settlementDate >= endDate  
			 * 			- fixingDate <= startDate
			 *
			 * @author	Alvaro Díaz V.
			 * @date	23/09/2018
			 *
			 * @return	True if it succeeds, false if it fails.
			 */
			bool _validate();
		};

	}
}

#endif //IBORCASHFLOW_H