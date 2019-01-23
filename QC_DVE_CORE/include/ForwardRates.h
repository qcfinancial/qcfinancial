#ifndef FORWARDRATES_H
#define FORWARDRATES_H

#include<vector>
#include<memory>

#include "ZeroCouponCurve.h"
#include "QCDate.h"
#include "Cashflow.h"
#include "IborCashflow.h"
#include "IcpClpCashflow.h"
#include "Leg.h"

namespace QCode
{
	namespace Financial
	{
		class ForwardRates
		{
		public:
			ForwardRates()
			{}

			std::shared_ptr<IborCashflow> setRateIborCashflow(const QCDate& valuationDate, Cashflow& iborCashflow,
				ZeroCouponCurve& curve)
			{
				auto iborCashflow_ = dynamic_cast<IborCashflow&>(iborCashflow);
				std::vector<double> derivatives(curve.getLength(), 0.0);
				if (valuationDate >= iborCashflow_.getFixingDate())
				{
					return std::make_shared<IborCashflow>(iborCashflow_);
				}
				QCDate fecha1 = iborCashflow_.getIndexStartDate();
				QCDate fecha2 = iborCashflow_.getIndexEndDate();
				long t1 = valuationDate.dayDiff(fecha1);
				long t2 = valuationDate.dayDiff(fecha2);
				auto tasaForward = curve.getForwardRate(t1, t2);
				iborCashflow_.setInterestRateValue(tasaForward);
				return std::make_shared<IborCashflow>(iborCashflow_);
			}

			void setRatesIborLeg(const QCDate& valuationDate, Leg& iborLeg, ZeroCouponCurve& curve)
			{
				_derivatives2.resize(iborLeg.size(), vector<double>(curve.getLength(), 0.0));
				for (size_t i = 0; i < iborLeg.size(); ++i)
				{
					auto cashflow = setRateIborCashflow(valuationDate, *(iborLeg.getCashflowAt(i)), curve);
					iborLeg.setCashflowAt(cashflow, i);
				}
			}

			std::shared_ptr<IcpClpCashflow> setRateIcpClpCashflow(const QCDate& valuationDate, double icpValuationDate, Cashflow& icpClpCashflow,
				ZeroCouponCurve& curve)
			{
				auto icpClpCashflow_ = dynamic_cast<IcpClpCashflow&>(icpClpCashflow);
				if (valuationDate >= icpClpCashflow_.getEndDate())
				{
					return std::make_shared<IcpClpCashflow>(icpClpCashflow_);
				}
				else if ((icpClpCashflow_.getStartDate() < valuationDate) && (valuationDate < icpClpCashflow_.getEndDate()))
				{
					auto t = valuationDate.dayDiff(icpClpCashflow_.getEndDate());
					icpClpCashflow_.setEndDateICP(icpValuationDate / curve.getDiscountFactorAt(t));
					return std::make_shared<IcpClpCashflow>(icpClpCashflow_);
				}
				else if (icpClpCashflow_.getStartDate() == valuationDate)
				{
					auto t = valuationDate.dayDiff(icpClpCashflow_.getEndDate());
					icpClpCashflow_.setStartDateICP(icpValuationDate);
					icpClpCashflow_.setEndDateICP(icpValuationDate / curve.getDiscountFactorAt(t));
					return std::make_shared<IcpClpCashflow>(icpClpCashflow_);
				}
				else
				{
					auto t1 = valuationDate.dayDiff(icpClpCashflow_.getStartDate());
					auto t2 = valuationDate.dayDiff(icpClpCashflow_.getEndDate());
					icpClpCashflow_.setStartDateICP(icpValuationDate / curve.getDiscountFactorAt(t1));
					icpClpCashflow_.setEndDateICP(icpValuationDate / curve.getDiscountFactorAt(t2));
					std::vector<double> derivatives(curve.getLength(), 2.0);
					return std::make_shared<IcpClpCashflow>(icpClpCashflow_);
				}
			}

			void setRatesIcpClpLeg(const QCDate& valuationDate, double icpValuationDate, Leg& icpClpLeg,
				ZeroCouponCurve& curve)
			{
				_derivatives2.resize(icpClpLeg.size(), vector<double>(curve.getLength(), 0.0));
				for (size_t i = 0; i < icpClpLeg.size(); ++i)
				{
					auto cashflow = setRateIcpClpCashflow(valuationDate, icpValuationDate, *(icpClpLeg.getCashflowAt(i)), curve);
					icpClpLeg.setCashflowAt(cashflow, i);
				}
			}

            ~ForwardRates()
            {
            }


        private:
			std::vector<double> _derivatives;
			std::vector<vector<double>> _derivatives2;
		};
	}
}

#endif //FORWARDRATES_H