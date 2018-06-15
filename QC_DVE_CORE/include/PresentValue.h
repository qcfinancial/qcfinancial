#ifndef PRESENTVALUE_H
#define PRESENTVALUE_H

#include <vector>

#include "Cashflow.h"
#include "InterestRateCurve.h"
#include "Leg.h"

#include "QCInterestRate.h"

namespace QCode
{
	namespace Financial
	{
		/**
		* @class	PresentValue
		*
		* @brief	Class that calculates the present value ande derivatives of a cashflow
		*           using a disocunting curve.
		*
		* @author	Alvaro Díaz V.
		* @date	    28/05/2018
		*/
		class PresentValue
		{
		public:

			/**
			* @fn	PresentValue::PresentValue();
			*
			* @brief	Default constructor
			*
			* @author	Alvaro Díaz V.
			* @date	28/05/2018
			*/
			PresentValue() :_derivative(0.0), _rate(0.0)
			{
			}

			PresentValue(const PresentValue& other)
				: _derivative(other._derivative),
				  _derivatives(other._derivatives),
				  _rate(other._rate)
			{
			}

			PresentValue(PresentValue&& other)
				: _derivative(other._derivative),
				  _derivatives(std::move(other._derivatives)),
				  _rate(other._rate)
			{
			}

			PresentValue& operator=(const PresentValue& other)
			{
				if (this == &other)
					return *this;
				_derivative = other._derivative;
				_derivatives = other._derivatives;
				_rate = other._rate;
				return *this;
			}

			PresentValue& operator=(PresentValue&& other)
			{
				if (this == &other)
					return *this;
				_derivative = other._derivative;
				_derivatives = std::move(other._derivatives);
				_rate = other._rate;
				return *this;
			}

			/**
			* @fn	    double PresentValue::pv(QCDate& valuationDate,
			*                                   std::shared_ptr<Cashflow> cashflow,
			*                                   QCInterestRate& intRate);
			*
			* @brief	Calculates the present value of the cashflow and its derivative with respect to
			* 			the interest rate given.
			*
			* @param    valuationDate: date at which present value will be calculated.
			* @param    cashflow: amount which will be present valued.
			* @param	intRate: interest rate from which to obtain the appropriate discount factor.
			*
			* @author	Alvaro Díaz V.
			* @date	    28/05/2018
			*
			* @return	A double with the present value. Derivative is stored in the class and has
			*           its own set of accessors.
			*/
			double pv(QCDate& valuationDate, const std::shared_ptr<Cashflow>& cashflow,
				QCInterestRate& intRate)
			{
				auto days = valuationDate.dayDiff(cashflow->date());
				if (days < 0)
				{
					_derivative = 0.0;
					return 0.0;
				}
				if (days == 0)
				{
					_derivative = 0.0;
					return cashflow->amount();
				}
				else
				{
					double amount = cashflow->amount();
					double wf = intRate.wf(days);
					double dwf = intRate.dwf(days);
					double d2wf = intRate.d2wf(days);
					double pv = amount / wf;
					//wf(r) ---> d/dr(1/wf(r)) = (-dwf(r)/dr)/wf(r)**2
					_derivative = -amount * dwf * pow(wf, -2.0);
					_derivative2 = -amount * (d2wf * pow(wf, -2.0) - 2.0 * pow(wf, -3.0) * dwf * dwf);
					_rate = intRate.getValue();
					return pv;
				}
			}

			/**
			* @fn	    double PresentValue::pv(QCDate& valuationDate, 
			                                    std::shared_ptr<Cashflow> cashflow,
												std::shared_ptr<InterestRateCurve> curve);
			*
			* @brief	Calculates the present value of the cashflow and its derivatives with respect to
			* 			every vertex of the discounting curve.
			*
			* @param    valuationDate: date at which present value will be calculated.
			* @param    cashflow: amount which will be present valued.
			* @param	curve: curve from which to obtain the appropriate discount factor.
			*
			* @author	Alvaro Díaz V.
			* @date	    28/05/2018
			*
			* @return	A double with the present value. Derivatives are stored in the class and have
			*           their own set of accessors.
			*/
			double pv(QCDate& valuationDate, const std::shared_ptr<Cashflow>& cashflow,
				      const std::shared_ptr<InterestRateCurve>& curve)
			{
				_resetDerivatives(curve->getLength());

				const auto days = valuationDate.dayDiff(cashflow->date());
				if (days < 0)
				{
					std::fill(_derivatives.begin(), _derivatives.end(), 0);
					return 0.0;
				}
				if (days == 0)
				{
					std::fill(_derivatives.begin(), _derivatives.end(), 0);
					return cashflow->amount();
				}
				else
				{
					const double amount = cashflow->amount();
					const double pv = amount * curve->getDiscountFactorAt(days);
					_rate = curve->getRateAt(days);
					for (size_t i = 0; i < _derivatives.size(); ++i)
					{
						_derivatives[i] = curve->dfDerivativeAt((unsigned)i) * amount;
					}
					return pv;
				}
			}

			/**
			* @fn	    double PresentValue::pv(QCDate& valuationDate,
			*                                   Leg& leg,
			*                                   QCInterestRate& intRate);
			*
			* @brief	Calculates the present value of each cashflow in leg and its derivative with respect to
			* 			the interest rate.
			*
			* @param    valuationDate: date at which present value will be calculated.
			* @param    leg: cashflows which will be present valued.
			* @param	intRate: interest rate from which to obtain the appropriate discount factor.
			*
			* @author	Alvaro Díaz V.
			* @date	    28/05/2018
			*
			* @return	A double with the present value. Derivatives are stored in the class and have
			*           their own set of accessors.
			*/
			double pv(QCDate& valuationDate, Leg& leg, QCInterestRate& intRate)
			{
				_resetDerivatives(leg.size());

				double result = 0.0;
				for (size_t i = 0; i < leg.size(); ++i)
				{
					result += pv(valuationDate, leg.getCashflowAt(i), intRate);
					_derivatives[i] = _derivative;
					_derivatives2[i] = _derivative2;
				}
				return result;
			}

			/**
			* @fn	    double PresentValue::pv(QCDate& valuationDate,
			*                                   Leg& leg,
			*                                   std::shared_ptr<InterestRateCurve> curve);
			*
			* @brief	Calculates the present value of each cashflow in leg and its derivatives with respect to
			* 			every vertex of the discounting curve.
			*
			* @param    valuationDate: date at which present value will be calculated.
			* @param    leg: cashflows which will be present valued.
			* @param	curve: curve from which to obtain the appropriate discount factor.
			*
			* @author	Alvaro Díaz V.
			* @date	    28/05/2018
			*
			* @return	A double with the present value. Derivatives are stored in the class and have
			*           their own set of accessors.
			*/
			double pv(QCDate& valuationDate, Leg& leg, const std::shared_ptr<InterestRateCurve>& curve)
			{
				std::vector<double> tempDerivatives;
				tempDerivatives.resize(curve->getLength());
				std::fill(tempDerivatives.begin(), tempDerivatives.end(), 0.0);
				_resetDerivatives(curve->getLength());

				double result = 0.0;
				for (size_t i = 0; i < leg.size(); ++i)
				{
					result += pv(valuationDate, leg.getCashflowAt(i), curve);
					for (size_t j = 0; j < tempDerivatives.size(); ++j)
					{
						tempDerivatives[j] += _derivatives[j];
					}
				}
				_derivatives = tempDerivatives;
				return result;
			}

			/**
			* @fn      std::vector<double> getDerivatives(): returns the derivatives with respect to 
			*          each vertex of the discounting curve of the last present value calculated.
			*          
			* @author	Alvaro Díaz V.
			* @date	    28/05/2018
			*
			* @return	A std::vector<double> with the derivatives. The indexes of this vector
			*           coincides with the indices of the vertices of the curve.
			*/
			std::vector<double> getDerivatives() const
			{
				return _derivatives;
			}

			/**
			* @fn      std::vector<double> getDerivatives(): returns the 2nd derivatives of the present
			*          value of a single cashflow with respect to each vertex of the discounting curve.
			*         
			* @author	Alvaro Díaz V.
			* @date	    28/05/2018
			*
			* @return	A std::vector<double> with the derivatives. The indexes of this vector
			*           coincides with the indices of the vertices of the curve.
			*/
			std::vector<double> get2Derivatives() const
			{
				return _derivatives2;
			}

			/**
			* @fn      std::double getDerivative(): returns the derivative with respect to
			*          a single interest rate.
			*
			* @author	Alvaro Díaz V.
			* @date	    28/05/2018
			*
			* @return	A double.
			*/
			double getDerivative() const
			{
				return _derivative;
			}

			/**
			* @fn      std::double get2Derivative(): returns the 2nd derivative of the present
			*          value of a single cashflow with respect to a single interest rate.
			*
			* @author	Alvaro Díaz V.
			* @date	    28/05/2018
			*
			* @return	A double.
			*/
			double get2Derivative() const
			{
				return _derivative2;
			}


			/**
			* @fn      std::double getRate(): returns the rate used to calculate the pv
			* 		   when a single rate is used.
			*
			* @author	Alvaro Díaz V.
			* @date	    28/05/2018
			*
			* @return	A double.
			*/
			double getRate() const
			{
				return _rate;
			}

			virtual ~PresentValue()
			= default;

		private:

			/**
			* @brief	Variable that holds the derivative of the present value of a single
			*           cashflow with respect to a single interest rate.
			*/
			double _derivative;

			/**
			* @brief	Variable that holds the 2nd derivative of the present value of a single
			*           cashflow with respect to a single interest rate.
			*/
			double _derivative2;

			/**
			* @brief	Variable that holds the derivatives of the present value of a single
			*           cashflow with respect to the vertices of a discounting curve.
			*/
			std::vector<double> _derivatives;

			/**
			* @brief	Variable that holds the 2nd derivatives of the present value of a single
			*           cashflow with respect to the vertices of a discounting curve.
			*/
			std::vector<double> _derivatives2;

			/**
			* @brief	Resets the container for derivatives.
			*/
			void _resetDerivatives(size_t newSize)
			{
				_derivatives.clear();
				_derivatives.resize(newSize);
				_derivatives2.clear();
				_derivatives2.resize(newSize);
				return;
			}

			/**
			* @brief     Holds the value of rate used in the calculation of pv
			*            when a single rate is used. 
			*/
			double _rate;
		};

	}
}

#endif //PRESENTVALUE_H