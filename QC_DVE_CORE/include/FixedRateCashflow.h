#ifndef FIXEDRATECASHFLOW_H
#define FIXEDRATECASHFLOW_H

#include<tuple>
#include<memory>

#include "Cashflow.h"
#include "QCDate.h"
#include "QCInterestRate.h"
#include "QCCurrency.h"

namespace QCode
{
	namespace Financial
	{
		/**
		 * @typedef	std::tuple<QCDate, QCDate, QCDate, double, double, double, bool, shared_ptr<QCCurrency>, QCInterestRate> FixedRateCashflowWrapper
		 *
		 * @brief	Defines an alias representing a FixedRateCashflow wrapper
		 */
		typedef std::tuple<QCDate, QCDate, QCDate,
			double, double, double, bool,
			shared_ptr<QCCurrency>,
			QCInterestRate> FixedRateCashflowWrapper;

		/**
		 * @class	FixedRateCashflow
		 *
		 * @brief	A fixed rate cashflow.
		 *
		 * @author	Alvaro Díaz V.
		 * @date	27/09/2017
		 */
		class FixedRateCashflow : public Cashflow
		{
		public:

			/**
			* @enum	Element
			*
			* @brief	Values that represent the different elements of a FixedRateCashflow
			*/
			enum Element
			{
				///< An enum constant representing the start date
				startDate,

				///< An enum constant representing the end date
				endDate,

				///< An enum constant representing the settlement date
				settlementDate,

				///< An enum constant representing the notional
				notional,

				///< An enum constant representing the amortization
				amortization,

				///< An enum constant representing the interest
				interest,

				///< An enum constant representing the amort is cashflow
				amortIsCashflow,

				///< An enum constant representing the currency
				currency
			};

			/**
			 * @fn	FixedRateCashflow::FixedRateCashflow(const QCDate& startDate, const QCDate& endDate, const QCDate& settlementDate, double nominal, double amortization, bool doesAmortize, const QCInterestRate& rate, const QCCurrency& currency);
			 *
			 * @brief	Constructor. The parameters startDate and endDate are the dates between which the
			 * 			rate accrues.
			 *
			 * @author	Alvaro Díaz V.
			 * @date	27/09/2017
			 *
			 * @param	startDate	  	The start date.
			 * @param	endDate		  	The end date.
			 * @param	settlementDate	The settlement date.
			 * @param	nominal		  	The nominal.
			 * @param	amortization  	The amortization.
			 * @param	doesAmortize  	Indicates whether amortization is cashflow or not.
			 * @param	rate		  	The rate.
			 * @param	currency	  	The currency.
			 */
			FixedRateCashflow(const QCDate& startDate,
							  const QCDate& endDate,
							  const QCDate& settlementDate,
							  double nominal,
							  double amortization,
							  bool doesAmortize,
							  const QCInterestRate& rate,
							  std::shared_ptr<QCCurrency> currency);

			/**
			 * @fn	double FixedRateCashflow::amount();
			 *
			 * @brief	Gets the amount of the cashflow.
			 *
			 * @author	Alvaro Díaz V.
			 * @date	27/09/2017
			 *
			 * @return	A double.
			 */
			virtual double amount();

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
			shared_ptr<QCCurrency> ccy();

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
			QCDate date();

			/**
			* @fn	QCDate FixedRateCashflow::getStartDate();
			*
			* @brief	Gets the start date of the cashflow.
			*
			* @author	Alvaro Díaz V.
			* @date	27/09/2017
			*
			* @return	A const QCDate&.
			*/
			QCDate getStartDate() const;

			/**
			* @fn	QCDate FixedRateCashflow::getEndDate();
			*
			* @brief	Gets the end date of the cashflow.
			*
			* @author	Alvaro Díaz V.
			* @date	27/09/2017
			*
			* @return	A const QCDate&.
			*/
			QCDate getEndDate() const;

			/**
			* @fn	double FixedRateCashflow::getNominal();
			*
			* @brief	Gets the nominal of the cashflow.
			*
			* @author	Alvaro Díaz V.
			* @date	27/09/2017
			*
			* @return	A double.
			*/
			double getNominal() const;

			/**
			 * @fn	void FixedRateCashflow::setNominal(double nominal);
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
			 * @fn	void FixedRateCashflow::setAmortization(double amortization);
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
			 * @fn	tuple<QCDate, QCDate, QCDate, double, double, double, bool, QCCurrency> FixedRateCashflow::show();
			 *
			 * @brief	Wraps the cashflow in a FixedRateCashflowWrapper
			 *
			 * @author	Alvaro Díaz V.
			 * @date	27/09/2017
			 *
			 * @return	A tuple&lt;QCDate,QCDate,QCDate,double,double,double,bool,QCCurrency&gt;
			 */
			shared_ptr<FixedRateCashflowWrapper> wrap();

			/**
			* @fn	double FixedRateCashflow::accruedInterest(const QCDate&) const;
			*
			* @brief	Gets the accrued interest given a value date.
			*
			* @author	Alvaro Díaz V.
			* @date	27/09/2017
			*
			* @return	A double.
			*/
			double accruedInterest(const QCDate& valueDate);

			/**
			* @fn	virtual FixedRateCashflow::~FixedRateCashflow();
			*
			* @brief	Destructor
			*
			* @author	Alvaro Díaz V.
			* @date	27/09/2017
			*/
			virtual ~FixedRateCashflow();

		protected:

			/** @brief	Calculates interest */
			void _calculateInterest();

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

			/** @brief	The rate */
			QCInterestRate _rate;

			/** @brief	The currency */
			shared_ptr<QCCurrency> _currency;
		};

		/**
		 * @typedef	std::vector<shared_ptr<FixedRateCashflow>> FixedRateLeg
		 *
		 * @brief	Defines an alias representing a Leg made of Fixed Rate Cashflows.
		 */
		//typedef std::vector<shared_ptr<FixedRateCashflow>> FixedRateLeg;

		/**
		 * @struct	CustomNotionalAmort
		 *
		 * @brief	Represents a customized notional and amortization schedule.
		 *
		 * @author	Alvaro Díaz V.
		 * @date	07/10/2017
		 */
		struct CustomNotionalAmort
		{
			/**
			 * @fn	CustomNotionalAmort()
			 *
			 * @brief	Default constructor
			 *
			 * @author	Alvaro Díaz V.
			 * @date	07/10/2017
			 */
			CustomNotionalAmort()
			{
			}

			/**
			 * @fn	void setSize(size_t n)
			 *
			 * @brief	Sets a size (length)
			 *
			 * @author	Alvaro Díaz V.
			 * @date	07/10/2017
			 *
			 * @param	n	A size_t to process.
			 */
			void setSize(size_t n)
			{
				customNotionalAmort.resize(n);
			}

			/**
			 * @fn	size_t getSize()
			 *
			 * @brief	Gets the size (length) of the customized amortization schedule.
			 *
			 * @author	Alvaro Díaz V.
			 * @date	07/10/2017
			 *
			 * @return	The size.
			 */
			size_t getSize()
			{
				return customNotionalAmort.size();
			}

			/**
			 * @fn	void setNotionalAmortAt(size_t n, double notional, double amortization)
			 *
			 * @brief	Sets notional and amortization at position n
			 *
			 * @author	Alvaro Díaz V.
			 * @date	07/10/2017
			 *
			 * @param	n				A size_t to process.
			 * @param	notional		The notional.
			 * @param	amortization	The amortization.
			 */
			void setNotionalAmortAt(size_t n, double notional, double amortization)
			{
				get<0>(customNotionalAmort[n]) = notional;
				get<1>(customNotionalAmort[n]) = amortization;
			}

			double getNotionalAt(size_t n)
			{
				return get<0>(customNotionalAmort[n]);
			}

			double getAmortAt(size_t n)
			{
				return get<1>(customNotionalAmort[n]);
			}

			/**
			 * @fn	void pushbackNotionalAmort(double notional, double amortization)
			 *
			 * @brief	Appends at the end a notional and a amortization
			 *
			 * @author	Alvaro Díaz V.
			 * @date	07/10/2017
			 *
			 * @param	notional		The notional.
			 * @param	amortization	The amortization.
			 */
			void pushbackNotionalAmort(double notional, double amortization)
			{
				customNotionalAmort.push_back(make_tuple(notional, amortization));
			}

			/**
			 * @fn	~CustomNotionalAmort()
			 *
			 * @brief	Destructor
			 *
			 * @author	Alvaro Díaz V.
			 * @date	07/10/2017
			 */
			~CustomNotionalAmort()
			{
			}

			/** @brief	The custom notional and amortization schedule */
			std::vector<std::tuple<double, double>> customNotionalAmort;
		};
	};
}

#endif //FIXEDRATECASHFLOW_H