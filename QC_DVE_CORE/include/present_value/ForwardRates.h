#ifndef FORWARDRATES_H
#define FORWARDRATES_H

#include<vector>
#include<memory>

#include "asset_classes/ZeroCouponCurve.h"
#include "time/QCDate.h"
#include "cashflows/Cashflow.h"
#include "cashflows/IborCashflow.h"
#include "cashflows/IborCashflow2.h"
#include "cashflows/IcpClpCashflow.h"
#include "cashflows/IcpClpCashflow2.h"
#include "cashflows/LinearInterestRateCashflow.h"
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

			std::shared_ptr<IborCashflow2> setRateIborCashflow(const QCDate& valuationDate, Cashflow& iborCashflow,
				ZeroCouponCurve& curve)
			{
				auto iborCashflow_ = dynamic_cast<IborCashflow2&>(iborCashflow);
				std::vector<double> derivatives(curve.getLength(), 0.0);
				auto fixingDate = iborCashflow_.getFixingDates()[0];
				if (valuationDate > fixingDate)
				{
					return std::make_shared<IborCashflow2>(iborCashflow_);
				}
				QCDate fecha1 = iborCashflow_.getInterestRateIndex()->getStartDate(fixingDate);
				QCDate fecha2 = iborCashflow_.getInterestRateIndex()->getEndDate(fixingDate);
				long t1 = valuationDate.dayDiff(fecha1);
				long t2 = valuationDate.dayDiff(fecha2);
				auto tasaForward = curve.getForwardRate(t1, t2);
				iborCashflow_.getInterestRateIndex()->setRateValue(tasaForward);
				return std::make_shared<IborCashflow2>(iborCashflow_);
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

			std::shared_ptr<IcpClpCashflow2> setRateIcpClpCashflow(const QCDate& valuationDate,
			        double icpValuationDate,
			        Cashflow& icpClpCashflow,
			        ZeroCouponCurve& curve)
			{
			    std::vector<double> zeroDerivatives;
			    zeroDerivatives.resize(curve.getLength());
			    for (size_t i = 0; i < curve.getLength(); ++i)
                {
			        zeroDerivatives.at(i) = 0.0;
                }
				auto icpClpCashflow_ = dynamic_cast<IcpClpCashflow2&>(icpClpCashflow);

				if (valuationDate >= icpClpCashflow_.getEndDate())
				{
				    icpClpCashflow_.setStartDateICPDerivatives(zeroDerivatives);
				    icpClpCashflow_.setEndDateICPDerivatives(zeroDerivatives);
                    icpClpCashflow_.setDf(0.0);
					return std::make_shared<IcpClpCashflow2>(icpClpCashflow_);
				}
				else if ((icpClpCashflow_.getStartDate() < valuationDate) &&
				(valuationDate < icpClpCashflow_.getEndDate()))
				{
                    auto t = valuationDate.dayDiff(icpClpCashflow_.getSettlementDate());
                    icpClpCashflow_.setDf(curve.getDiscountFactorAt(t));

					t = valuationDate.dayDiff(icpClpCashflow_.getEndDate());
					icpClpCashflow_.setStartDateICPDerivatives(zeroDerivatives);

					icpClpCashflow_.setEndDateICP(icpValuationDate / curve.getDiscountFactorAt(t));
					vector<double> endDateDerivatives;
					endDateDerivatives.resize(curve.getLength());
					for (size_t i = 0; i < curve.getLength(); ++i)
                    {
					    endDateDerivatives.at(i) = icpValuationDate * curve.wfDerivativeAt(i);
                    }
					icpClpCashflow_.setEndDateICPDerivatives(endDateDerivatives);
					return std::make_shared<IcpClpCashflow2>(icpClpCashflow_);
				}
				else if (icpClpCashflow_.getStartDate() == valuationDate)
				{
                    auto t = valuationDate.dayDiff(icpClpCashflow_.getSettlementDate());
                    icpClpCashflow_.setDf(curve.getDiscountFactorAt(t));

                    t = valuationDate.dayDiff(icpClpCashflow_.getEndDate());
					icpClpCashflow_.setStartDateICP(icpValuationDate);
                    icpClpCashflow_.setStartDateICPDerivatives(zeroDerivatives);

					icpClpCashflow_.setEndDateICP(icpValuationDate / curve.getDiscountFactorAt(t));
                    vector<double> endDateDerivatives;
                    endDateDerivatives.resize(curve.getLength());
                    for (size_t i = 0; i < curve.getLength(); ++i)
                    {
                        endDateDerivatives.at(i) = icpValuationDate * curve.wfDerivativeAt(i);
                    }
                    icpClpCashflow_.setEndDateICPDerivatives(endDateDerivatives);
					return std::make_shared<IcpClpCashflow2>(icpClpCashflow_);
				}
				else
				{
                    auto t1 = valuationDate.dayDiff(icpClpCashflow_.getSettlementDate());
                    icpClpCashflow_.setDf(curve.getDiscountFactorAt(t1));

					t1 = valuationDate.dayDiff(icpClpCashflow_.getStartDate());
					icpClpCashflow_.setStartDateICP(icpValuationDate / curve.getDiscountFactorAt(t1));
                    vector<double> startDateDerivatives;
                    startDateDerivatives.resize(curve.getLength());
                    for (size_t i = 0; i < curve.getLength(); ++i)
                    {
                        startDateDerivatives.at(i) = icpValuationDate * curve.wfDerivativeAt(i);
                    }
                    icpClpCashflow_.setStartDateICPDerivatives(startDateDerivatives);

                    auto t2 = valuationDate.dayDiff(icpClpCashflow_.getEndDate());
					icpClpCashflow_.setEndDateICP(icpValuationDate / curve.getDiscountFactorAt(t2));
                    vector<double> endDateDerivatives;
                    endDateDerivatives.resize(curve.getLength());
                    for (size_t i = 0; i < curve.getLength(); ++i)
                    {
                        endDateDerivatives.at(i) = icpValuationDate * curve.wfDerivativeAt(i);
                    }
                    icpClpCashflow_.setEndDateICPDerivatives(endDateDerivatives);

                    return std::make_shared<IcpClpCashflow2>(icpClpCashflow_);
				}
			}

			void setRatesIcpClpLeg(const QCDate& valuationDate, double icpValuationDate, Leg& icpClpLeg,
				ZeroCouponCurve& curve)
			{
				for (size_t i = 0; i < icpClpLeg.size(); ++i)
				{
					auto cashflow = setRateIcpClpCashflow(valuationDate, icpValuationDate,
					        *(icpClpLeg.getCashflowAt(i)), curve);
					icpClpLeg.setCashflowAt(cashflow, i);
				}
			}

            ~ForwardRates()
            {
            }


        private:
			std::vector<double> _startDateICPDerivatives;
            std::vector<double> _endDateICPDerivatives;
			std::vector<vector<double>> _derivatives2;
		};
	}
}

#endif //FORWARDRATES_H