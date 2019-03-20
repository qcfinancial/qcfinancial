#ifndef FIXEDRATECASHFLOW2_H
#define FIXEDRATECASHFLOW2_H

#include "LinearInterestRateCashflow.h"
#include "QCInterestRate.h"
#include "QCCurrency.h"

namespace QCode
{
	namespace Financial
	{
		class FixedRateCashflow2 : public LinearInterestRateCashflow
		{
		public:
			/**
			 * @fn	FixedRateCashflow2::FixedRateCashflow2(const QCDate& startDate, const QCDate& endDate, const QCDate& settlementDate,
			 * 		double nominal, double amortization, bool doesAmortize,
			 * 		const QCInterestRate& rate, std::shared_ptr<QCCurrency> currency);
			 *
			 * @brief	Constructor
			 *
			 * @author	A Diaz V
			 * @date	19-03-2019
			 *
			 * @param	startDate	  	The start date.
			 * @param	endDate		  	The end date.
			 * @param	settlementDate	The settlement date.
			 * @param	nominal		  	The nominal.
			 * @param	amortization  	The amortization.
			 * @param	doesAmortize  	True if the amortization corresponds to an actual cashflow.
			 * @param	rate		  	The rate.
			 * @param	currency	  	The currency.
			 */
			FixedRateCashflow2(const QCDate& startDate,
				const QCDate& endDate,
				const QCDate& settlementDate,
				double nominal,
				double amortization,
				bool doesAmortize,
				const QCInterestRate& rate,
				std::shared_ptr<QCCurrency> currency);

			virtual double amount() override;

			/**
			* @fn	QCCurrency FixedRateCashflow::ccy();
			*
			* @brief	Gets the currency of the cashflow.
			*
			* @author	Alvaro Díaz V.
			* @date	27/09/2017
			*
			* @return	A QCCurrency.
			*/
			shared_ptr<QCCurrency> ccy() override;

			/**
			* @fn	QCDate FixedRateCashflow::date();
			*
			* @brief	Gets the payment date of the cashflow.
			*
			* @author	Alvaro Díaz V.
			* @date	27/09/2017
			*
			* @return	A QCDate.
			*/
			QCDate date() override;

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
			virtual const QCDate& getStartDate() const override;

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
			virtual const QCDate& getEndDate() const override;

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
			virtual const QCDate& getSettlementDate() const override;

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
			virtual const DateList& getFixingDates() const override;

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
			virtual double getNominal() const override;

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
			virtual double getAmortization() const override;

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
			virtual double amortization() const override;

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
			virtual double interest() override;

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
			virtual double interest(const TimeSeries& fixings) override;

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
			virtual double fixing() override;

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
			virtual double fixing(const TimeSeries& fixings) override;

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
			virtual double accruedInterest(const QCDate& fecha) override;

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
			virtual double accruedInterest(const QCDate& fecha, const TimeSeries& fixings) override;

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
			virtual double accruedFixing(const QCDate& fecha) override;

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
			virtual double accruedFixing(const QCDate& fecha, const TimeSeries& fixings) override;

			/**
			 * @fn	virtual bool FixedRateCashflow2::doesAmortize() const override;
			 *
			 * @brief	Determines if the amortization is a real cashflow
			 *
			 * @author	A Diaz V
			 * @date	20-03-2019
			 *
			 * @returns	True if amortization is a cashflow, false if it is not.
			 */
			virtual bool doesAmortize() const override;

			/**
			 * @fn	virtual FixedRateCashflow2::~FixedRateCashflow2();
			 *
			 * @brief	Destructor
			 *
			 * @author	A Diaz V
			 * @date	19-03-2019
			 */
			virtual ~FixedRateCashflow2();

		protected:

			/** @brief	Calculates interest */
			double _calculateInterest(const QCDate& fecha);

			/** @brief	The start date */
			QCDate _startDate;

			/** @brief	The end date */
			QCDate _endDate;

			/** @brief	The settlement date */
			QCDate _settlementDate;


			/** @brief	List of dates corresponding to ficing dates. In this case only contains start date */
			vector<QCDate> _fixingDates;

			/** @brief	The nominal */
			double _nominal;

			/** @brief	The amortization */
			double _amortization;

			/** @brief	True if amortization is part of the cashflow */
			bool _doesAmortize;

			/** @brief	The rate */
			QCInterestRate _rate;

			/** @brief	The currency */
			shared_ptr<QCCurrency> _currency;

			/** @brief	Message that describes errors encountered when constructing the object */
			std::string _validateMsg;

			/**
			* @fn	bool FixedRateCashflow::_validate();
			*
			* @brief	Validates that the object is properly constructed.
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

#endif //FIXEDRATECASHFLOW2_H