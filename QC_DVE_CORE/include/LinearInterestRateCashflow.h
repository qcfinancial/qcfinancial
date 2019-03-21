#ifndef INTERESTRATECASHFLOW_H
#define INTERESTRATECASHFLOW_H

#include "Cashflow.h"
#include "QCDate.h"
#include "TypeAliases.h"

namespace QCode
{
	namespace Financial
	{
		/**
		 * @class	InterestRateCashflow
		 *
		 * @brief	Provides an interface for linear cashflows associated to interest rates.
		 *
		 * @author	A Diaz V
		 * @date	19-03-2019
		 */
		class LinearInterestRateCashflow : public Cashflow
		{
		public:

			/**
			 * @fn	virtual std::shared_ptr<QCCurrency> LinearInterestRateCashflow::getInitialCcy() const = 0;
			 *
			 * @brief	Gets initial currency. Subclasses must preserve the currency of the cashflow and provide this method even when the cashflow
			 * 			has been quantized.
			 *
			 * @author	A Diaz V
			 * @date	21-03-2019
			 *
			 * @returns	The initial ccy.
			 */
			virtual std::shared_ptr<QCCurrency> getInitialCcy() const = 0;

			/**
			 * @fn	virtual const QCDate& LinearInterestRateCashflow::getStartDate() const = 0;
			 *
			 * @brief	Gets the start date. It corresponds to the first date used when calculating interest.
			 *
			 * @author	A Diaz V
			 * @date	19-03-2019
			 *
			 * @returns	The start date.
			 */
			virtual const QCDate& getStartDate() const = 0;

			/**
			 * @fn	virtual const QCDate& LinearInterestRateCashflow::getEndDate() const = 0;
			 *
			 * @brief	Gets end date. It corresponds to the last date used when calculating interest.
			 *
			 * @author	A Diaz V
			 * @date	19-03-2019
			 *
			 * @returns	The end date.
			 */
			virtual const QCDate& getEndDate() const = 0;

			/**
			 * @fn	virtual const QCDate& LinearInterestRateCashflow::getSettlementDate() const = 0;
			 *
			 * @brief	Gets settlement date. Corresponds to the date where the cashflow actually settles.
			 *
			 * @author	A Diaz V
			 * @date	19-03-2019
			 *
			 * @returns	The settlement date.
			 */
			virtual const QCDate& getSettlementDate() const = 0;

			/**
			 * @fn	virtual const std::vector<QCDate>& LinearInterestRateCashflow::getFixingDates() const = 0;
			 *
			 * @brief	Gets fixing dates. Corresponds to the fixing dates of the index used to calculate the final fixing.
			 *
			 * @author	A Diaz V
			 * @date	19-03-2019
			 *
			 * @returns	The fixing dates.
			 */
			virtual const DateList& getFixingDates() const = 0;

			/**
			 * @fn	virtual double LinearInterestRateCashflow::getNominal() const = 0;
			 *
			 * @brief	Gets the nominal. Amount to which fixing is applied to calculate interest.
			 *
			 * @author	A Diaz V
			 * @date	19-03-2019
			 *
			 * @returns	The nominal.
			 */
			virtual double getNominal() const = 0;

			/**
			 * @fn	virtual double LinearInterestRateCashflow::nominal(const QCDate& fecha) const = 0;
			 *
			 * @brief	Returns the nominal. Subclasses should, in this method, apply all relevant calculations.
			 *
			 * @author	A Diaz V
			 * @date	21-03-2019
			 *
			 * @param	fecha	The fecha.
			 *
			 * @returns	A double.
			 */
			virtual double nominal(const QCDate& fecha) const = 0;

			/**
			 * @fn	virtual double LinearInterestRateCashflow::getAmortization() const = 0;
			 *
			 * @brief	Gets the amortization of the cashflow. It is a pure getter, it will return a value even when the amortization is not a real cashflow.
			 *
			 * @author	A Diaz V
			 * @date	19-03-2019
			 *
			 * @returns	The amortization.
			 */
			virtual double getAmortization() const = 0;

			/**
			 * @fn	virtual double LinearInterestRateCashflow::amortization() const = 0;
			 *
			 * @brief	Gets the amortization if doesAmortize = true. Subclasses should, in this method, apply all relevant calculations.
			 *
			 * @author	A Diaz V
			 * @date	20-03-2019
			 *
			 * @returns	A double.
			 */
			virtual double amortization() const = 0;

			/**
			 * @fn	virtual double LinearInterestRateCashflow::interest() = 0;
			 *
			 * @brief	Gets the total interest associated to the cashflow. Use this overload if all the parameters are
			 * 			fixed at construction time.
			 *
			 * @author	A Diaz V
			 * @date	19-03-2019
			 *
			 * @returns	A double.
			 */
			virtual double interest() = 0;

			/**
			 * @fn	virtual double LinearInterestRateCashflow::interest(const TimeSeries& fixings) = 0;
			 *
			 * @brief	Gets the total interest associated to the cashflow. Use this overload if the interest depends on
			 * 			intermediate date fixings.
			 *
			 * @author	A Diaz V
			 * @date	19-03-2019
			 *
			 * @param	fixings	Time series for the interest rate index values used to calculate the total interest.
			 *
			 * @returns	A double.
			 */
			virtual double interest(const TimeSeries& fixings) = 0;

			/**
			 * @fn	virtual double LinearInterestRateCashflow::fixing() = 0;
			 *
			 * @brief	Gets the fixing (or final rate) corresponding to the cashflow. Use this overload if all the parameters are
			 * 			fixed at construction time.
			 *
			 * @author	A Diaz V
			 * @date	19-03-2019
			 *
			 * @returns	A double.
			 */
			virtual double fixing() = 0;

			/**
			 * @fn	virtual double LinearInterestRateCashflow::fixing(const TimeSeries& fixings);
			 *
			 * @brief	Gets the fixing (or final rate) corresponding to the cashflow. Use this overload if the interest depends on
			 * 			intermediate date fixings.
			 *
			 * @author	A Diaz V
			 * @date	19-03-2019
			 *
			 * @param	fixings	Time series for the interest rate index values used to calculate the final fixing.
			 *
			 * @returns	A double.
			 */
			virtual double fixing(const TimeSeries& fixings) = 0;

			/**
			 * @fn	virtual double LinearInterestRateCashflow::accruedInterest(const QCDate& fecha) = 0;
			 *
			 * @brief	Accrued interest. Use this overload if all the parameters are fixed at construction time.
			 *
			 * @author	A Diaz V
			 * @date	19-03-2019
			 *
			 * @param	fecha	Date to which accrued interest corresponds.
			 *
			 * @returns	A double.
			 */
			virtual double accruedInterest(const QCDate& fecha) = 0;

			/**
			 * @fn	virtual double LinearInterestRateCashflow::accruedInterest(const QCDate& fecha, const TimeSeries& fixings) = 0;
			 *
			 * @brief	Accrued interest. Use this overload if the interest depends on intermediate date fixings.
			 *
			 * @author	A Diaz V
			 * @date	19-03-2019
			 *
			 * @param	fecha  	Date to which accrued interest corresponds.
			 * @param	fixings	Time series for the interest rate index values used to calculate the accrued fixing.
			 *
			 * @returns	A double.
			 */
			virtual double accruedInterest(const QCDate& fecha, const TimeSeries& fixings) = 0;

			/**
			 * @fn	virtual double LinearInterestRateCashflow::accruedFixing(const QCDate& fecha) = 0;
			 *
			 * @brief	Accrued fixing. Use this overload if all the parameters are fixed at construction time.
			 *
			 * @author	A Diaz V
			 * @date	19-03-2019
			 *
			 * @param	fecha	Date to which accrued fixing corresponds.
			 *
			 * @returns	A double.
			 */
			virtual double accruedFixing(const QCDate& fecha) = 0;

			/**
			 * @fn	virtual double LinearInterestRateCashflow::accruedFixing(const QCDate& fecha, const TimeSeries& fixings) = 0;
			 *
			 * @brief	Accrued fixing. Use this overload if the interest depends on intermediate date fixings.
			 *
			 * @author	A Diaz V
			 * @date	19-03-2019
			 *
			 * @param	fecha  	Date to which accrued fixing corresponds.
			 * @param	fixings	Time series for the interest rate index values used to calculate the accrued fixing.
			 *
			 * @returns	A double.
			 */
			virtual double accruedFixing(const QCDate& fecha, const TimeSeries& fixings) = 0;

			/**
			 * @fn	virtual bool LinearInterestRateCashflow::doesAmortize() const = 0;
			 *
			 * @brief	Determines if the amortization is a real cashflow
			 *
			 * @author	A Diaz V
			 * @date	20-03-2019
			 *
			 * @returns	True if amortization is a cashflow, false if it is not.
			 */
			virtual bool doesAmortize() const = 0;

			/**
			 * @fn	virtual LinearInterestRateCashflow::~LinearInterestRateCashflow()
			 *
			 * @brief	Destructor
			 *
			 * @author	A Diaz V
			 * @date	20-03-2019
			 */
			virtual ~LinearInterestRateCashflow()
			{
			}
		};
	}
}

#endif //INTERESTRATECASHFLOW_H