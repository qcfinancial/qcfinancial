#ifndef FORWARDRATES_H
#define FORWARDRATES_H

#include<vector>

#include "asset_classes/ZeroCouponCurve.h"
#include "time/QCDate.h"
#include "cashflows/Cashflow.h"
#include "cashflows/IborCashflow2.h"
#include "cashflows/IcpClpCashflow.h"
#include "cashflows/IcpClpCashflow2.h"
#include "cashflows/OvernightIndexCashflow.h"
#include "cashflows/IcpClfCashflow.h"
#include "cashflows/CompoundedOvernightRateCashflow.h"
#include "Leg.h"


    namespace QCode::Financial {
        class ForwardRates {
        public:
            ForwardRates() = default;

            std::shared_ptr<IborCashflow2> setRateIborCashflow(
                    const QCDate &valuationDate,
                    Cashflow &iborCashflow,
                    ZeroCouponCurve &curve) {
                auto iborCashflow_ = dynamic_cast<IborCashflow2 &>(iborCashflow);
                std::vector<double> derivatives;
                derivatives.resize(curve.getLength());
                for (size_t i = 0; i < curve.getLength(); ++i) {
                    derivatives.at(i) = 0.0;
                }
                iborCashflow_.setForwardRateWfDerivatives(derivatives);
                auto fixingDate = iborCashflow_.getFixingDates()[0];
                if (valuationDate > fixingDate) {
                    return std::make_shared<IborCashflow2>(iborCashflow_);
                }
                QCDate fecha1 = iborCashflow_.getInterestRateIndex()->getStartDate(fixingDate);
                QCDate fecha2 = iborCashflow_.getInterestRateIndex()->getEndDate(fixingDate);
                long t1 = valuationDate.dayDiff(fecha1);
                long t2 = valuationDate.dayDiff(fecha2);
                auto iborRate = iborCashflow_.getInterestRateIndex()->getRate();
                auto tasaForward = curve.getForwardRateWithRate(iborRate, t1, t2);
                for (size_t i = 0; i < curve.getLength(); ++i) {
                    derivatives.at(i) = curve.fwdWfDerivativeAt(i);
                }
                iborCashflow_.setRateValue(tasaForward);
                iborCashflow_.setForwardRateWfDerivatives(derivatives);
                auto plazo = valuationDate.dayDiff(iborCashflow_.getSettlementDate());
                return std::make_shared<IborCashflow2>(iborCashflow_);
            }


            std::shared_ptr<IborCashflow> setRateIborCashflow1(
                    const QCDate &valuationDate,
                    Cashflow &iborCashflow,
                    ZeroCouponCurve &curve) {
                auto iborCashflow_ = dynamic_cast<IborCashflow &>(iborCashflow);
                std::vector<double> derivatives;
                derivatives.resize(curve.getLength());
                for (size_t i = 0; i < curve.getLength(); ++i) {
                    derivatives.at(i) = 0.0;
                }
                iborCashflow_.setForwardRateWfDerivatives(derivatives);
                auto fixingDate = iborCashflow_.getFixingDate();
                if (valuationDate > fixingDate) {
                    return std::make_shared<IborCashflow>(iborCashflow_);
                }
                QCDate fecha1 = iborCashflow_.getIndexStartDate();
                QCDate fecha2 = iborCashflow_.getIndexEndDate();
                long t1 = valuationDate.dayDiff(fecha1);
                long t2 = valuationDate.dayDiff(fecha2);
                auto iborRate = iborCashflow_.getInterestRateIndex()->getRate();
                auto tasaForward = curve.getForwardRateWithRate(iborRate, t1, t2);
                for (size_t i = 0; i < curve.getLength(); ++i) {
                    derivatives.at(i) = curve.fwdWfDerivativeAt(i);
                }
                iborCashflow_.setInterestRateValue(tasaForward);
                iborCashflow_.setForwardRateWfDerivatives(derivatives);
                auto plazo = valuationDate.dayDiff(iborCashflow_.getSettlementDate());
                return std::make_shared<IborCashflow>(iborCashflow_);
            }


            std::shared_ptr<IborMultiCurrencyCashflow> setRateIborMccyCashflow1(
                    const QCDate &valuationDate,
                    Cashflow &iborCashflow,
                    ZeroCouponCurve &curve) {
                auto iborCashflow_ = dynamic_cast<IborMultiCurrencyCashflow &>(iborCashflow);
                std::vector<double> derivatives;
                derivatives.resize(curve.getLength());
                for (size_t i = 0; i < curve.getLength(); ++i) {
                    derivatives.at(i) = 0.0;
                }
                iborCashflow_.setForwardRateWfDerivatives(derivatives);
                auto fixingDate = iborCashflow_.getFixingDate();
                if (valuationDate > fixingDate) {
                    return std::make_shared<IborMultiCurrencyCashflow>(iborCashflow_);
                }
                QCDate fecha1 = iborCashflow_.getIndexStartDate();
                QCDate fecha2 = iborCashflow_.getIndexEndDate();
                long t1 = valuationDate.dayDiff(fecha1);
                long t2 = valuationDate.dayDiff(fecha2);
                auto iborRate = iborCashflow_.getInterestRateIndex()->getRate();
                auto tasaForward = curve.getForwardRateWithRate(iborRate, t1, t2);
                for (size_t i = 0; i < curve.getLength(); ++i) {
                    derivatives.at(i) = curve.fwdWfDerivativeAt(i);
                }
                iborCashflow_.setInterestRateValue(tasaForward);
                iborCashflow_.setForwardRateWfDerivatives(derivatives);
                auto plazo = valuationDate.dayDiff(iborCashflow_.getSettlementDate());
                return std::make_shared<IborMultiCurrencyCashflow>(iborCashflow_);
            }


            void setRatesIborLeg(
                    const QCDate &valuationDate,
                    Leg &iborLeg,
                    ZeroCouponCurve &curve) {
                _derivatives2.resize(iborLeg.size(), vector<double>(curve.getLength(), 0.0));
                for (size_t i = 0; i < iborLeg.size(); ++i) {
                    auto cashflow = setRateIborCashflow(valuationDate, *(iborLeg.getCashflowAt(i)), curve);
                    iborLeg.setCashflowAt(cashflow, i);
                }
            }


            void setRatesIborLeg1(
                    const QCDate &valuationDate,
                    Leg &iborLeg,
                    ZeroCouponCurve &curve) {
                _derivatives2.resize(iborLeg.size(), vector<double>(curve.getLength(), 0.0));
                for (size_t i = 0; i < iborLeg.size(); ++i) {
                    auto cashflow = setRateIborCashflow1(valuationDate, *(iborLeg.getCashflowAt(i)), curve);
                    iborLeg.setCashflowAt(cashflow, i);
                }
            }


            void setRatesIborMccyLeg1(
                    const QCDate &valuationDate,
                    Leg &iborLeg,
                    ZeroCouponCurve &curve) {
                _derivatives2.resize(iborLeg.size(), vector<double>(curve.getLength(), 0.0));
                for (size_t i = 0; i < iborLeg.size(); ++i) {
                    auto cashflow = setRateIborMccyCashflow1(valuationDate, *(iborLeg.getCashflowAt(i)), curve);
                    iborLeg.setCashflowAt(cashflow, i);
                }
            }


            void setRatesCompoundedOvernightLeg(
                    const QCDate &valuationDate,
                    Leg &compoundedONLeg,
                    ZeroCouponCurve &curve,
                    const TimeSeries& fixings) {
                _derivatives2.resize(compoundedONLeg.size(), vector<double>(curve.getLength(), 0.0));
                for (size_t i = 0; i < compoundedONLeg.size(); ++i) {
                    auto initialCashflow = std::dynamic_pointer_cast<CompoundedOvernightRateCashflow>(compoundedONLeg.getCashflowAt(i));
                    // Calcular el fixing
                    auto accruedFixing = initialCashflow->accruedFixing(valuationDate, fixings);
                    auto cashflow = setRateCompoundedOvernightCashflow(
                            valuationDate,
                            accruedFixing,
                            *initialCashflow,
                            curve);
                    compoundedONLeg.setCashflowAt(cashflow, i);
                }
            }

            void setRatesCompoundedOvernightLeg2(
                    const QCDate &valuationDate,
                    Leg &compoundedONLeg,
                    ZeroCouponCurve &curve,
                    const TimeSeries& fixings) {
                _derivatives2.resize(compoundedONLeg.size(), vector<double>(curve.getLength(), 0.0));
                for (size_t i = 0; i < compoundedONLeg.size(); ++i) {
                    auto initialCashflow = std::dynamic_pointer_cast<CompoundedOvernightRateCashflow2>(compoundedONLeg.getCashflowAt(i));

                    // Calcular el fixing
                    auto accruedFixing = initialCashflow->accruedFixing(valuationDate, fixings);
                    auto cashflow = setRateCompoundedOvernightCashflow2(
                            valuationDate,
                            accruedFixing,
                            *initialCashflow,
                            curve);
                    compoundedONLeg.setCashflowAt(cashflow, i);
                }
            }


            void setRatesCompoundedOvernightMccyLeg2(
                    const QCDate &valuationDate,
                    Leg &compoundedONLeg,
                    ZeroCouponCurve &curve,
                    const TimeSeries& fixings) {
                _derivatives2.resize(compoundedONLeg.size(), vector<double>(curve.getLength(), 0.0));
                for (size_t i = 0; i < compoundedONLeg.size(); ++i) {
                    auto initialCashflow = std::dynamic_pointer_cast<CompoundedOvernightRateMultiCurrencyCashflow2>(compoundedONLeg.getCashflowAt(i));

                    // Calcular el fixing
                    auto accruedFixing = initialCashflow->accruedFixing(valuationDate, fixings);
                    auto cashflow = setRateCompoundedOvernightMccyCashflow2(
                            valuationDate,
                            accruedFixing,
                            *initialCashflow,
                            curve);
                    compoundedONLeg.setCashflowAt(cashflow, i);
                }
            }



            std::shared_ptr<CompoundedOvernightRateCashflow> setRateCompoundedOvernightCashflow(
                    const QCDate &valuationDate,
                    double accruedFixing,
                    Cashflow &compoundedONRateCashflow,
                    ZeroCouponCurve &curve) {
                std::vector<double> zeroDerivatives(curve.getLength(), 0.0);

                auto compoundedONRateCashflow_ = dynamic_cast<CompoundedOvernightRateCashflow &>(compoundedONRateCashflow);
                auto notional_ = compoundedONRateCashflow_.getNominal();

                if (valuationDate >= compoundedONRateCashflow_.getEndDate())
                {
                    compoundedONRateCashflow_.setAmountDerivatives(zeroDerivatives);
                    compoundedONRateCashflow_.setInitialDateWf(1.0);
                    compoundedONRateCashflow_.getInterestRateIndex()->setRateValue(accruedFixing);
                    auto t = compoundedONRateCashflow_.getStartDate().dayDiff(compoundedONRateCashflow_.getEndDate());
                    auto endDateWf = compoundedONRateCashflow_.getInterestRateIndex()->getRate().wf(t);
                    compoundedONRateCashflow_.setEndDateWf(endDateWf);
                    return std::make_shared<CompoundedOvernightRateCashflow>(compoundedONRateCashflow_);

                }
                else if ((compoundedONRateCashflow_.getStartDate() < valuationDate) &&
                           (valuationDate < compoundedONRateCashflow_.getEndDate()))
                {
                    compoundedONRateCashflow_.setInitialDateWf(1.0);
                    auto t1 = compoundedONRateCashflow_.getStartDate().dayDiff(valuationDate);
                    auto t2 = valuationDate.dayDiff(compoundedONRateCashflow_.getEndDate());
                    compoundedONRateCashflow_.getInterestRateIndex()->setRateValue(accruedFixing);
                    auto accruedWf = compoundedONRateCashflow_.getInterestRateIndex()->getRate().wf(t1);
                    compoundedONRateCashflow_.setEndDateWf(accruedWf / curve.getDiscountFactorAt(t2));
                    for (size_t i = 0; i < curve.getLength(); ++i) {
                        zeroDerivatives.at(i) = notional_ * accruedWf * curve.wfDerivativeAt(i);
                    }
                    compoundedONRateCashflow_.setAmountDerivatives(zeroDerivatives);
                    return std::make_shared<CompoundedOvernightRateCashflow>(compoundedONRateCashflow_);
                }
                else if (compoundedONRateCashflow_.getStartDate() == valuationDate)
                {
                    compoundedONRateCashflow_.setInitialDateWf(1.0);
                    auto t = valuationDate.dayDiff(compoundedONRateCashflow_.getEndDate());
                    auto endDateWf = 1.0 / curve.getDiscountFactorAt(t);
                    compoundedONRateCashflow_.setEndDateWf(endDateWf);
                    for (size_t i = 0; i < curve.getLength(); ++i) {
                        zeroDerivatives.at(i) = notional_ * curve.wfDerivativeAt(i);
                    }
                    compoundedONRateCashflow_.setAmountDerivatives(zeroDerivatives);
                    return std::make_shared<CompoundedOvernightRateCashflow>(compoundedONRateCashflow_);
                }
                else
                {
                    auto t1 = valuationDate.dayDiff(compoundedONRateCashflow_.getStartDate());
                    compoundedONRateCashflow_.setInitialDateWf(1.0 / curve.getDiscountFactorAt(t1));
                    vector<double> startDateDerivatives(curve.getLength(), 0.0);
                    for (size_t i = 0; i < curve.getLength(); ++i) {
                        startDateDerivatives.at(i) = curve.wfDerivativeAt(i);
                    }

                    auto t2 = valuationDate.dayDiff(compoundedONRateCashflow_.getEndDate());
                    compoundedONRateCashflow_.setEndDateWf(1.0 / curve.getDiscountFactorAt(t2));
                    vector<double> endDateDerivatives(curve.getLength(), 0.0);
                    for (size_t i = 0; i < curve.getLength(); ++i) {
                        endDateDerivatives.at(i) = curve.wfDerivativeAt(i);
                    }
                    for (size_t i = 0; i < curve.getLength(); ++i) {
                        zeroDerivatives.at(i) = notional_ * (endDateDerivatives.at(i) / curve.getDiscountFactorAt(t1) -
                                startDateDerivatives.at(i) / curve.getDiscountFactorAt(t2)) *
                                        std::pow(curve.getDiscountFactorAt(t1), 2.0);
                    }
                    compoundedONRateCashflow_.setAmountDerivatives(zeroDerivatives);

                    return std::make_shared<CompoundedOvernightRateCashflow>(compoundedONRateCashflow_);
                }
            }

            std::shared_ptr<CompoundedOvernightRateCashflow2> setRateCompoundedOvernightCashflow2(
                    const QCDate &valuationDate,
                    double accruedFixing,
                    Cashflow &compoundedONRateCashflow,
                    ZeroCouponCurve &curve) {
                std::vector<double> zeroDerivatives(curve.getLength(), 0.0);

                auto compoundedONRateCashflow_ = dynamic_cast<CompoundedOvernightRateCashflow2 &>(compoundedONRateCashflow);
                auto notional_ = compoundedONRateCashflow_.getNotional();

                if (valuationDate >= compoundedONRateCashflow_.getEndDate())
                {
                    compoundedONRateCashflow_.setAmountDerivatives(zeroDerivatives);
                    compoundedONRateCashflow_.setInitialDateWf(1.0);
                    compoundedONRateCashflow_.getInterestRateIndex()->setRateValue(accruedFixing);
                    auto t = compoundedONRateCashflow_.getStartDate().dayDiff(compoundedONRateCashflow_.getEndDate());
                    auto endDateWf = compoundedONRateCashflow_.getInterestRateIndex()->getRate().wf(t);
                    compoundedONRateCashflow_.setEndDateWf(endDateWf);
                    return std::make_shared<CompoundedOvernightRateCashflow2>(compoundedONRateCashflow_);

                }
                else if ((compoundedONRateCashflow_.getStartDate() < valuationDate) &&
                         (valuationDate < compoundedONRateCashflow_.getEndDate()))
                {
                    compoundedONRateCashflow_.setInitialDateWf(1.0);
                    auto t1 = compoundedONRateCashflow_.getStartDate().dayDiff(valuationDate);
                    auto t2 = valuationDate.dayDiff(compoundedONRateCashflow_.getEndDate());
                    compoundedONRateCashflow_.getInterestRateIndex()->setRateValue(accruedFixing);
                    auto accruedWf = compoundedONRateCashflow_.getInterestRateIndex()->getRate().wf(t1);
                    compoundedONRateCashflow_.setEndDateWf(accruedWf / curve.getDiscountFactorAt(t2));
                    for (size_t i = 0; i < curve.getLength(); ++i) {
                        zeroDerivatives.at(i) = notional_ * accruedWf * curve.wfDerivativeAt(i);
                    }
                    compoundedONRateCashflow_.setAmountDerivatives(zeroDerivatives);
                    return std::make_shared<CompoundedOvernightRateCashflow2>(compoundedONRateCashflow_);
                }
                else if (compoundedONRateCashflow_.getStartDate() == valuationDate)
                {
                    compoundedONRateCashflow_.setInitialDateWf(1.0);
                    auto t = valuationDate.dayDiff(compoundedONRateCashflow_.getEndDate());
                    auto endDateWf = 1.0 / curve.getDiscountFactorAt(t);
                    compoundedONRateCashflow_.setEndDateWf(endDateWf);
                    for (size_t i = 0; i < curve.getLength(); ++i) {
                        zeroDerivatives.at(i) = notional_ * curve.wfDerivativeAt(i);
                    }
                    compoundedONRateCashflow_.setAmountDerivatives(zeroDerivatives);
                    return std::make_shared<CompoundedOvernightRateCashflow2>(compoundedONRateCashflow_);
                }
                else
                {
                    auto t1 = valuationDate.dayDiff(compoundedONRateCashflow_.getStartDate());
                    compoundedONRateCashflow_.setInitialDateWf(1.0 / curve.getDiscountFactorAt(t1));
                    vector<double> startDateDerivatives(curve.getLength(), 0.0);
                    for (size_t i = 0; i < curve.getLength(); ++i) {
                        startDateDerivatives.at(i) = curve.wfDerivativeAt(i);
                    }

                    auto t2 = valuationDate.dayDiff(compoundedONRateCashflow_.getEndDate());
                    compoundedONRateCashflow_.setEndDateWf(1.0 / curve.getDiscountFactorAt(t2));
                    vector<double> endDateDerivatives(curve.getLength(), 0.0);
                    for (size_t i = 0; i < curve.getLength(); ++i) {
                        endDateDerivatives.at(i) = curve.wfDerivativeAt(i);
                    }
                    for (size_t i = 0; i < curve.getLength(); ++i) {
                        zeroDerivatives.at(i) = notional_ * (endDateDerivatives.at(i) / curve.getDiscountFactorAt(t1) -
                                                             startDateDerivatives.at(i) / curve.getDiscountFactorAt(t2)) *
                                                std::pow(curve.getDiscountFactorAt(t1), 2.0);
                    }
                    compoundedONRateCashflow_.setAmountDerivatives(zeroDerivatives);

                    return std::make_shared<CompoundedOvernightRateCashflow2>(compoundedONRateCashflow_);
                }
            }

            std::shared_ptr<CompoundedOvernightRateMultiCurrencyCashflow2> setRateCompoundedOvernightMccyCashflow2(
                    const QCDate &valuationDate,
                    double accruedFixing,
                    Cashflow &compoundedONRateCashflow,
                    ZeroCouponCurve &curve) {
                std::vector<double> zeroDerivatives(curve.getLength(), 0.0);

                auto compoundedONRateCashflow_ = dynamic_cast<CompoundedOvernightRateMultiCurrencyCashflow2 &>(compoundedONRateCashflow);
                auto notional_ = compoundedONRateCashflow_.getNotional();

                if (valuationDate >= compoundedONRateCashflow_.getEndDate())
                {
                    compoundedONRateCashflow_.setAmountDerivatives(zeroDerivatives);
                    compoundedONRateCashflow_.setInitialDateWf(1.0);
                    compoundedONRateCashflow_.getInterestRateIndex()->setRateValue(accruedFixing);
                    auto t = compoundedONRateCashflow_.getStartDate().dayDiff(compoundedONRateCashflow_.getEndDate());
                    auto endDateWf = compoundedONRateCashflow_.getInterestRateIndex()->getRate().wf(t);
                    compoundedONRateCashflow_.setEndDateWf(endDateWf);
                    return std::make_shared<CompoundedOvernightRateMultiCurrencyCashflow2>(compoundedONRateCashflow_);

                }
                else if ((compoundedONRateCashflow_.getStartDate() < valuationDate) &&
                         (valuationDate < compoundedONRateCashflow_.getEndDate()))
                {
                    compoundedONRateCashflow_.setInitialDateWf(1.0);
                    auto t1 = compoundedONRateCashflow_.getStartDate().dayDiff(valuationDate);
                    auto t2 = valuationDate.dayDiff(compoundedONRateCashflow_.getEndDate());
                    compoundedONRateCashflow_.getInterestRateIndex()->setRateValue(accruedFixing);
                    auto accruedWf = compoundedONRateCashflow_.getInterestRateIndex()->getRate().wf(t1);
                    compoundedONRateCashflow_.setEndDateWf(accruedWf / curve.getDiscountFactorAt(t2));
                    for (size_t i = 0; i < curve.getLength(); ++i) {
                        zeroDerivatives.at(i) = notional_ * accruedWf * curve.wfDerivativeAt(i);
                    }
                    compoundedONRateCashflow_.setAmountDerivatives(zeroDerivatives);
                    return std::make_shared<CompoundedOvernightRateMultiCurrencyCashflow2>(compoundedONRateCashflow_);
                }
                else if (compoundedONRateCashflow_.getStartDate() == valuationDate)
                {
                    compoundedONRateCashflow_.setInitialDateWf(1.0);
                    auto t = valuationDate.dayDiff(compoundedONRateCashflow_.getEndDate());
                    auto endDateWf = 1.0 / curve.getDiscountFactorAt(t);
                    compoundedONRateCashflow_.setEndDateWf(endDateWf);
                    for (size_t i = 0; i < curve.getLength(); ++i) {
                        zeroDerivatives.at(i) = notional_ * curve.wfDerivativeAt(i);
                    }
                    compoundedONRateCashflow_.setAmountDerivatives(zeroDerivatives);
                    return std::make_shared<CompoundedOvernightRateMultiCurrencyCashflow2>(compoundedONRateCashflow_);
                }
                else
                {
                    auto t1 = valuationDate.dayDiff(compoundedONRateCashflow_.getStartDate());
                    compoundedONRateCashflow_.setInitialDateWf(1.0 / curve.getDiscountFactorAt(t1));
                    vector<double> startDateDerivatives(curve.getLength(), 0.0);
                    for (size_t i = 0; i < curve.getLength(); ++i) {
                        startDateDerivatives.at(i) = curve.wfDerivativeAt(i);
                    }

                    auto t2 = valuationDate.dayDiff(compoundedONRateCashflow_.getEndDate());
                    compoundedONRateCashflow_.setEndDateWf(1.0 / curve.getDiscountFactorAt(t2));
                    vector<double> endDateDerivatives(curve.getLength(), 0.0);
                    for (size_t i = 0; i < curve.getLength(); ++i) {
                        endDateDerivatives.at(i) = curve.wfDerivativeAt(i);
                    }
                    for (size_t i = 0; i < curve.getLength(); ++i) {
                        zeroDerivatives.at(i) = notional_ * (endDateDerivatives.at(i) / curve.getDiscountFactorAt(t1) -
                                                             startDateDerivatives.at(i) / curve.getDiscountFactorAt(t2)) *
                                                std::pow(curve.getDiscountFactorAt(t1), 2.0);
                    }
                    compoundedONRateCashflow_.setAmountDerivatives(zeroDerivatives);

                    return std::make_shared<CompoundedOvernightRateMultiCurrencyCashflow2>(compoundedONRateCashflow_);
                }
            }


            std::shared_ptr<IcpClpCashflow2> setRateIcpClpCashflow(
                    const QCDate &valuationDate,
                    double icpValuationDate,
                    Cashflow &icpClpCashflow,
                    ZeroCouponCurve &curve) {
                std::vector<double> zeroDerivatives;
                zeroDerivatives.resize(curve.getLength());
                for (size_t i = 0; i < curve.getLength(); ++i) {
                    zeroDerivatives.at(i) = 0.0;
                }
                auto icpClpCashflow_ = dynamic_cast<IcpClpCashflow2 &>(icpClpCashflow);

                if (valuationDate >= icpClpCashflow_.getEndDate()) {
                    icpClpCashflow_.setStartDateICPDerivatives(zeroDerivatives);
                    icpClpCashflow_.setEndDateICPDerivatives(zeroDerivatives);
                    return std::make_shared<IcpClpCashflow2>(icpClpCashflow_);
                } else if ((icpClpCashflow_.getStartDate() < valuationDate) &&
                           (valuationDate < icpClpCashflow_.getEndDate())) {
                    auto t = valuationDate.dayDiff(icpClpCashflow_.getEndDate());
                    icpClpCashflow_.setStartDateICPDerivatives(zeroDerivatives);

                    icpClpCashflow_.setEndDateICP(icpValuationDate / curve.getDiscountFactorAt(t));
                    vector<double> endDateDerivatives;
                    endDateDerivatives.resize(curve.getLength());
                    for (size_t i = 0; i < curve.getLength(); ++i) {
                        endDateDerivatives.at(i) = icpValuationDate * curve.wfDerivativeAt(i);
                    }
                    icpClpCashflow_.setEndDateICPDerivatives(endDateDerivatives);
                    return std::make_shared<IcpClpCashflow2>(icpClpCashflow_);
                } else if (icpClpCashflow_.getStartDate() == valuationDate) {
                    auto t = valuationDate.dayDiff(icpClpCashflow_.getSettlementDate());

                    t = valuationDate.dayDiff(icpClpCashflow_.getEndDate());
                    icpClpCashflow_.setStartDateICP(icpValuationDate);
                    icpClpCashflow_.setStartDateICPDerivatives(zeroDerivatives);

                    icpClpCashflow_.setEndDateICP(icpValuationDate / curve.getDiscountFactorAt(t));
                    vector<double> endDateDerivatives;
                    endDateDerivatives.resize(curve.getLength());
                    for (size_t i = 0; i < curve.getLength(); ++i) {
                        endDateDerivatives.at(i) = icpValuationDate * curve.wfDerivativeAt(i);
                    }
                    icpClpCashflow_.setEndDateICPDerivatives(endDateDerivatives);
                    return std::make_shared<IcpClpCashflow2>(icpClpCashflow_);
                } else {
                    auto t1 = valuationDate.dayDiff(icpClpCashflow_.getSettlementDate());

                    t1 = valuationDate.dayDiff(icpClpCashflow_.getStartDate());
                    icpClpCashflow_.setStartDateICP(icpValuationDate / curve.getDiscountFactorAt(t1));
                    vector<double> startDateDerivatives;
                    startDateDerivatives.resize(curve.getLength());
                    for (size_t i = 0; i < curve.getLength(); ++i) {
                        startDateDerivatives.at(i) = icpValuationDate * curve.wfDerivativeAt(i);
                    }
                    icpClpCashflow_.setStartDateICPDerivatives(startDateDerivatives);

                    auto t2 = valuationDate.dayDiff(icpClpCashflow_.getEndDate());
                    icpClpCashflow_.setEndDateICP(icpValuationDate / curve.getDiscountFactorAt(t2));
                    vector<double> endDateDerivatives;
                    endDateDerivatives.resize(curve.getLength());
                    for (size_t i = 0; i < curve.getLength(); ++i) {
                        endDateDerivatives.at(i) = icpValuationDate * curve.wfDerivativeAt(i);
                    }
                    icpClpCashflow_.setEndDateICPDerivatives(endDateDerivatives);

                    return std::make_shared<IcpClpCashflow2>(icpClpCashflow_);
                }
            }


            std::shared_ptr<IcpClpCashflow2> setRateIcpClpCashflow2(
                    const QCDate &valuationDate,
                    double icpValuationDate,
                    Cashflow &icpClpCashflow2,
                    ZeroCouponCurve &curve) {
                std::vector<double> zeroDerivatives;
                zeroDerivatives.resize(curve.getLength());
                for (size_t i = 0; i < curve.getLength(); ++i) {
                    zeroDerivatives.at(i) = 0.0;
                }
                auto icpClpCashflow_ = dynamic_cast<IcpClpCashflow2 &>(icpClpCashflow2);

                if (valuationDate >= icpClpCashflow_.getEndDate()) {
                    icpClpCashflow_.setStartDateICPDerivatives(zeroDerivatives);
                    icpClpCashflow_.setEndDateICPDerivatives(zeroDerivatives);
                    return std::make_shared<IcpClpCashflow2>(icpClpCashflow_);
                } else if ((icpClpCashflow_.getStartDate() < valuationDate) &&
                           (valuationDate < icpClpCashflow_.getEndDate())) {
                    auto t = valuationDate.dayDiff(icpClpCashflow_.getEndDate());
                    icpClpCashflow_.setStartDateICPDerivatives(zeroDerivatives);

                    icpClpCashflow_.setEndDateICP(icpValuationDate / curve.getDiscountFactorAt(t));
                    vector<double> endDateDerivatives;
                    endDateDerivatives.resize(curve.getLength());
                    for (size_t i = 0; i < curve.getLength(); ++i) {
                        endDateDerivatives.at(i) = icpValuationDate * curve.wfDerivativeAt(i);
                    }
                    icpClpCashflow_.setEndDateICPDerivatives(endDateDerivatives);
                    return std::make_shared<IcpClpCashflow2>(icpClpCashflow_);
                } else if (icpClpCashflow_.getStartDate() == valuationDate) {
                    auto t = valuationDate.dayDiff(icpClpCashflow_.getSettlementDate());

                    t = valuationDate.dayDiff(icpClpCashflow_.getEndDate());
                    icpClpCashflow_.setStartDateICP(icpValuationDate);
                    icpClpCashflow_.setStartDateICPDerivatives(zeroDerivatives);

                    icpClpCashflow_.setEndDateICP(icpValuationDate / curve.getDiscountFactorAt(t));
                    vector<double> endDateDerivatives;
                    endDateDerivatives.resize(curve.getLength());
                    for (size_t i = 0; i < curve.getLength(); ++i) {
                        endDateDerivatives.at(i) = icpValuationDate * curve.wfDerivativeAt(i);
                    }
                    icpClpCashflow_.setEndDateICPDerivatives(endDateDerivatives);
                    return std::make_shared<IcpClpCashflow2>(icpClpCashflow_);
                } else {
                    auto t1 = valuationDate.dayDiff(icpClpCashflow_.getSettlementDate());

                    t1 = valuationDate.dayDiff(icpClpCashflow_.getStartDate());
                    icpClpCashflow_.setStartDateICP(icpValuationDate / curve.getDiscountFactorAt(t1));
                    vector<double> startDateDerivatives;
                    startDateDerivatives.resize(curve.getLength());
                    for (size_t i = 0; i < curve.getLength(); ++i) {
                        startDateDerivatives.at(i) = icpValuationDate * curve.wfDerivativeAt(i);
                    }
                    icpClpCashflow_.setStartDateICPDerivatives(startDateDerivatives);

                    auto t2 = valuationDate.dayDiff(icpClpCashflow_.getEndDate());
                    icpClpCashflow_.setEndDateICP(icpValuationDate / curve.getDiscountFactorAt(t2));
                    vector<double> endDateDerivatives;
                    endDateDerivatives.resize(curve.getLength());
                    for (size_t i = 0; i < curve.getLength(); ++i) {
                        endDateDerivatives.at(i) = icpValuationDate * curve.wfDerivativeAt(i);
                    }
                    icpClpCashflow_.setEndDateICPDerivatives(endDateDerivatives);

                    return std::make_shared<IcpClpCashflow2>(icpClpCashflow_);
                }
            }

            std::shared_ptr<OvernightIndexCashflow> setRateOvernightIndexCashflow(
                    const QCDate &valuationDate,
                    double indexValuationDate,
                    Cashflow &overnightIndexCashflow,
                    ZeroCouponCurve &curve) {
                std::vector<double> zeroDerivatives;
                zeroDerivatives.resize(curve.getLength());
                for (size_t i = 0; i < curve.getLength(); ++i) {
                    zeroDerivatives.at(i) = 0.0;
                }
                auto overnightIndexCashflow_ = dynamic_cast<OvernightIndexCashflow &>(overnightIndexCashflow);

                if (valuationDate >= overnightIndexCashflow_.getEndDate()) {
                    overnightIndexCashflow_.setStartDateIndexDerivatives(zeroDerivatives);
                    overnightIndexCashflow_.setEndDateIndexDerivatives(zeroDerivatives);
                    return std::make_shared<OvernightIndexCashflow>(overnightIndexCashflow_);
                } else if ((overnightIndexCashflow_.getStartDate() < valuationDate) &&
                           (valuationDate < overnightIndexCashflow_.getEndDate())) {
                    auto t = valuationDate.dayDiff(overnightIndexCashflow_.getIndexEndDate()); //AQUI
                    overnightIndexCashflow_.setStartDateIndexDerivatives(zeroDerivatives);

                    overnightIndexCashflow_.setEndDateIndex(indexValuationDate / curve.getDiscountFactorAt(t));
                    vector<double> endDateDerivatives;
                    endDateDerivatives.resize(curve.getLength());
                    for (size_t i = 0; i < curve.getLength(); ++i) {
                        endDateDerivatives.at(i) = indexValuationDate * curve.wfDerivativeAt(i);
                    }
                    overnightIndexCashflow_.setEndDateIndexDerivatives(endDateDerivatives);
                    return std::make_shared<OvernightIndexCashflow>(overnightIndexCashflow_);
                } else if (overnightIndexCashflow_.getStartDate() == valuationDate) {
                    auto t = valuationDate.dayDiff(overnightIndexCashflow_.getSettlementDate());

                    t = valuationDate.dayDiff(overnightIndexCashflow_.getIndexEndDate()); // AQUI
                    overnightIndexCashflow_.setStartDateIndex(indexValuationDate);
                    overnightIndexCashflow_.setStartDateIndexDerivatives(zeroDerivatives);

                    overnightIndexCashflow_.setEndDateIndex(indexValuationDate / curve.getDiscountFactorAt(t));
                    vector<double> endDateDerivatives;
                    endDateDerivatives.resize(curve.getLength());
                    for (size_t i = 0; i < curve.getLength(); ++i) {
                        endDateDerivatives.at(i) = indexValuationDate * curve.wfDerivativeAt(i);
                    }
                    overnightIndexCashflow_.setEndDateIndexDerivatives(endDateDerivatives);
                    return std::make_shared<OvernightIndexCashflow>(overnightIndexCashflow_);
                } else {
                    auto t1 = valuationDate.dayDiff(overnightIndexCashflow_.getSettlementDate());

                    t1 = valuationDate.dayDiff(overnightIndexCashflow_.getIndexStartDate()); // AQUI
                    overnightIndexCashflow_.setStartDateIndex(indexValuationDate / curve.getDiscountFactorAt(t1));
                    vector<double> startDateDerivatives;
                    startDateDerivatives.resize(curve.getLength());
                    for (size_t i = 0; i < curve.getLength(); ++i) {
                        startDateDerivatives.at(i) = indexValuationDate * curve.wfDerivativeAt(i);
                    }
                    overnightIndexCashflow_.setStartDateIndexDerivatives(startDateDerivatives);

                    auto t2 = valuationDate.dayDiff(overnightIndexCashflow_.getIndexEndDate()); // AQUI
                    overnightIndexCashflow_.setEndDateIndex(indexValuationDate / curve.getDiscountFactorAt(t2));
                    vector<double> endDateDerivatives;
                    endDateDerivatives.resize(curve.getLength());
                    for (size_t i = 0; i < curve.getLength(); ++i) {
                        endDateDerivatives.at(i) = indexValuationDate * curve.wfDerivativeAt(i);
                    }
                    overnightIndexCashflow_.setEndDateIndexDerivatives(endDateDerivatives);

                    return std::make_shared<OvernightIndexCashflow>(overnightIndexCashflow_);
                }
            }

            std::shared_ptr<OvernightIndexMultiCurrencyCashflow> setRateOvernightIndexMccyCashflow(
                    const QCDate &valuationDate,
                    double indexValuationDate,
                    Cashflow &overnightIndexCashflow,
                    ZeroCouponCurve &curve) {
                std::vector<double> zeroDerivatives;
                zeroDerivatives.resize(curve.getLength());
                for (size_t i = 0; i < curve.getLength(); ++i) {
                    zeroDerivatives.at(i) = 0.0;
                }
                auto overnightIndexCashflow_ = dynamic_cast<OvernightIndexMultiCurrencyCashflow &>(overnightIndexCashflow);

                if (valuationDate >= overnightIndexCashflow_.getEndDate()) {
                    overnightIndexCashflow_.setStartDateIndexDerivatives(zeroDerivatives);
                    overnightIndexCashflow_.setEndDateIndexDerivatives(zeroDerivatives);
                    return std::make_shared<OvernightIndexMultiCurrencyCashflow>(overnightIndexCashflow_);
                } else if ((overnightIndexCashflow_.getStartDate() < valuationDate) &&
                           (valuationDate < overnightIndexCashflow_.getEndDate())) {
                    auto t = valuationDate.dayDiff(overnightIndexCashflow_.getIndexEndDate()); //AQUI
                    overnightIndexCashflow_.setStartDateIndexDerivatives(zeroDerivatives);

                    overnightIndexCashflow_.setEndDateIndex(indexValuationDate / curve.getDiscountFactorAt(t));
                    vector<double> endDateDerivatives;
                    endDateDerivatives.resize(curve.getLength());
                    for (size_t i = 0; i < curve.getLength(); ++i) {
                        endDateDerivatives.at(i) = indexValuationDate * curve.wfDerivativeAt(i);
                    }
                    overnightIndexCashflow_.setEndDateIndexDerivatives(endDateDerivatives);
                    return std::make_shared<OvernightIndexMultiCurrencyCashflow>(overnightIndexCashflow_);
                } else if (overnightIndexCashflow_.getStartDate() == valuationDate) {
                    auto t = valuationDate.dayDiff(overnightIndexCashflow_.getSettlementDate());

                    t = valuationDate.dayDiff(overnightIndexCashflow_.getIndexEndDate()); // AQUI
                    overnightIndexCashflow_.setStartDateIndex(indexValuationDate);
                    overnightIndexCashflow_.setStartDateIndexDerivatives(zeroDerivatives);

                    overnightIndexCashflow_.setEndDateIndex(indexValuationDate / curve.getDiscountFactorAt(t));
                    vector<double> endDateDerivatives;
                    endDateDerivatives.resize(curve.getLength());
                    for (size_t i = 0; i < curve.getLength(); ++i) {
                        endDateDerivatives.at(i) = indexValuationDate * curve.wfDerivativeAt(i);
                    }
                    overnightIndexCashflow_.setEndDateIndexDerivatives(endDateDerivatives);
                    return std::make_shared<OvernightIndexMultiCurrencyCashflow>(overnightIndexCashflow_);
                } else {
                    auto t1 = valuationDate.dayDiff(overnightIndexCashflow_.getSettlementDate());

                    t1 = valuationDate.dayDiff(overnightIndexCashflow_.getIndexStartDate()); // AQUI
                    overnightIndexCashflow_.setStartDateIndex(indexValuationDate / curve.getDiscountFactorAt(t1));
                    vector<double> startDateDerivatives;
                    startDateDerivatives.resize(curve.getLength());
                    for (size_t i = 0; i < curve.getLength(); ++i) {
                        startDateDerivatives.at(i) = indexValuationDate * curve.wfDerivativeAt(i);
                    }
                    overnightIndexCashflow_.setStartDateIndexDerivatives(startDateDerivatives);

                    auto t2 = valuationDate.dayDiff(overnightIndexCashflow_.getIndexEndDate()); // AQUI
                    overnightIndexCashflow_.setEndDateIndex(indexValuationDate / curve.getDiscountFactorAt(t2));
                    vector<double> endDateDerivatives;
                    endDateDerivatives.resize(curve.getLength());
                    for (size_t i = 0; i < curve.getLength(); ++i) {
                        endDateDerivatives.at(i) = indexValuationDate * curve.wfDerivativeAt(i);
                    }
                    overnightIndexCashflow_.setEndDateIndexDerivatives(endDateDerivatives);

                    return std::make_shared<OvernightIndexMultiCurrencyCashflow>(overnightIndexCashflow_);
                }
            }

            void setRatesIcpClpLeg(
                    const QCDate &valuationDate,
                    double icpValuationDate,
                    Leg &icpClpLeg,
                    ZeroCouponCurve &curve) {
                for (size_t i = 0; i < icpClpLeg.size(); ++i) {
                    auto cashflow = setRateIcpClpCashflow(valuationDate, icpValuationDate,
                                                          *(icpClpLeg.getCashflowAt(i)), curve);
                    icpClpLeg.setCashflowAt(cashflow, i);
                }
            }

            void setRatesIcpClpLeg2(
                    const QCDate &valuationDate,
                    double icpValuationDate,
                    Leg &icpClpLeg2,
                    ZeroCouponCurve &curve) {
                for (size_t i = 0; i < icpClpLeg2.size(); ++i) {
                    auto cashflow = setRateIcpClpCashflow2(
                            valuationDate,
                            icpValuationDate,
                            *(icpClpLeg2.getCashflowAt(i)),
                            curve);
                    icpClpLeg2.setCashflowAt(cashflow, i);
                }
            }

            void setRatesOvernightIndexLeg(
                const QCDate &valuationDate,
                double indexValuationDate,
                Leg &overnightIndexLeg,
                ZeroCouponCurve &curve) {
                for (size_t i = 0; i < overnightIndexLeg.size(); ++i) {
                    auto cashflow = setRateOvernightIndexCashflow(
                            valuationDate,
                            indexValuationDate,
                            *(overnightIndexLeg.getCashflowAt(i)),
                            curve);
                    overnightIndexLeg.setCashflowAt(cashflow, i);
                }
            }


            void setRatesOvernightIndexMccyLeg(
                    const QCDate &valuationDate,
                    double indexValuationDate,
                    Leg &overnightIndexLeg,
                    ZeroCouponCurve &curve) {
                for (size_t i = 0; i < overnightIndexLeg.size(); ++i) {
                    auto cashflow = setRateOvernightIndexMccyCashflow(
                            valuationDate,
                            indexValuationDate,
                            *(overnightIndexLeg.getCashflowAt(i)),
                            curve);
                    overnightIndexLeg.setCashflowAt(cashflow, i);
                }
            }


            std::shared_ptr<IcpClfCashflow> setRateIcpClfCashflow(
                    const QCDate &valuationDate,
                    double icpValuationDate,
                    double ufValuationDate,
                    std::shared_ptr<IcpClfCashflow> icpClfCashflow,
                    ZeroCouponCurve &icpCurve,
                    ZeroCouponCurve &ufCLPCurve,
                    ZeroCouponCurve &ufCLFCurve) {
                std::vector<double> zeroIcpCurveDerivatives;
                zeroIcpCurveDerivatives.resize(icpCurve.getLength());
                for (size_t i = 0; i < icpCurve.getLength(); ++i) {
                    zeroIcpCurveDerivatives.at(i) = 0.0;
                }

                std::vector<double> zeroUFCLPCurveDerivatives;
                zeroUFCLPCurveDerivatives.resize(ufCLPCurve.getLength());
                for (size_t i = 0; i < ufCLPCurve.getLength(); ++i) {
                    zeroUFCLPCurveDerivatives.at(i) = 0.0;
                }

                std::vector<double> zeroUFCLFCurveDerivatives;
                zeroUFCLFCurveDerivatives.resize(ufCLFCurve.getLength());
                for (size_t i = 0; i < ufCLFCurve.getLength(); ++i) {
                    zeroUFCLFCurveDerivatives.at(i) = 0.0;
                }

                // Cashflow vencido. No se proyecta nada.
                if (valuationDate >= icpClfCashflow->getEndDate()) {
                    icpClfCashflow->setStartDateICPDerivatives(zeroIcpCurveDerivatives);
                    icpClfCashflow->setEndDateICPDerivatives(zeroIcpCurveDerivatives);

                    icpClfCashflow->setStartDateUFCLPDerivatives(zeroUFCLPCurveDerivatives);
                    icpClfCashflow->setEndDateUFCLPDerivatives(zeroUFCLPCurveDerivatives);

                    icpClfCashflow->setStartDateUFCLFDerivatives(zeroUFCLFCurveDerivatives);
                    icpClfCashflow->setEndDateUFCLFDerivatives(zeroUFCLFCurveDerivatives);
                    return icpClfCashflow;
                }

                    // Cashflow vigente, sólo se proyectan los índices a vencimiento
                else if ((icpClfCashflow->getStartDate() <= valuationDate) &&
                         (valuationDate < icpClfCashflow->getEndDate())) {
                    icpClfCashflow->setStartDateICPDerivatives(zeroIcpCurveDerivatives);
                    icpClfCashflow->setStartDateUFCLPDerivatives(zeroUFCLPCurveDerivatives);
                    icpClfCashflow->setStartDateUFCLFDerivatives(zeroUFCLFCurveDerivatives);

                    auto t = valuationDate.dayDiff(icpClfCashflow->getEndDate());

                    // Proyecta ICP final
                    icpClfCashflow->setEndDateICP(icpValuationDate / icpCurve.getDiscountFactorAt(t));

                    // Derivadas de ICP final
                    vector<double> tempDerivatives;
                    tempDerivatives.resize(icpCurve.getLength());
                    for (size_t i = 0; i < icpCurve.getLength(); ++i) {
                        tempDerivatives.at(i) = icpValuationDate * icpCurve.wfDerivativeAt(i);
                    }
                    icpClfCashflow->setEndDateICPDerivatives(tempDerivatives);

                    // Proyecta UF final
                    auto dfUFCLP = ufCLPCurve.getDiscountFactorAt(t);
                    auto dfUFCLF = ufCLFCurve.getDiscountFactorAt(t);
                    icpClfCashflow->setEndDateUf(ufValuationDate * dfUFCLF / dfUFCLP);

                    // Derivadas de UF final respecto a curva CLP
                    tempDerivatives.resize(ufCLPCurve.getLength());
                    for (size_t i = 0; i < ufCLPCurve.getLength(); ++i) {
                        tempDerivatives.at(i) = ufValuationDate * dfUFCLF * ufCLPCurve.wfDerivativeAt(i);
                    }
                    icpClfCashflow->setEndDateUFCLPDerivatives(tempDerivatives);

                    // Derivadas de UF final respecto a curva CLF
                    tempDerivatives.resize(ufCLFCurve.getLength());
                    for (size_t i = 0; i < ufCLFCurve.getLength(); ++i) {
                        tempDerivatives.at(i) = -ufValuationDate /
                                                dfUFCLP * ufCLFCurve.wfDerivativeAt(i) *
                                                pow(dfUFCLF, 2.0);
                    }
                    icpClfCashflow->setEndDateUFCLFDerivatives(tempDerivatives);

                    return icpClfCashflow;
                }
                    // Cashflow en el futuro, se proyectan los índices iniciales y finales
                else {
                    auto t1 = valuationDate.dayDiff(icpClfCashflow->getStartDate());
                    auto t2 = valuationDate.dayDiff(icpClfCashflow->getEndDate());

                    // Protecta ICP inicial
                    icpClfCashflow->setStartDateICP(icpValuationDate / icpCurve.getDiscountFactorAt(t1));

                    // Derivadas de ICP inicial
                    vector<double> tempDerivatives;
                    tempDerivatives.resize(icpCurve.getLength());
                    for (size_t i = 0; i < icpCurve.getLength(); ++i) {
                        tempDerivatives.at(i) = icpValuationDate * icpCurve.wfDerivativeAt(i);
                    }
                    icpClfCashflow->setStartDateICPDerivatives(tempDerivatives);

                    // Proyecta UF inicial
                    auto dfUFCLP = ufCLPCurve.getDiscountFactorAt(t1);
                    auto dfUFCLF = ufCLFCurve.getDiscountFactorAt(t1);
                    icpClfCashflow->setStartDateUf(ufValuationDate * dfUFCLF / dfUFCLP);

                    // Derivadas de UF inicial respecto a curva CLP
                    tempDerivatives.resize(ufCLPCurve.getLength());
                    for (size_t i = 0; i < ufCLPCurve.getLength(); ++i) {
                        tempDerivatives.at(i) = ufValuationDate * dfUFCLF * ufCLPCurve.wfDerivativeAt(i);
                    }
                    icpClfCashflow->setStartDateUFCLPDerivatives(tempDerivatives);

                    // Derivadas de UF inicial respecto a curva CLF
                    tempDerivatives.resize(ufCLFCurve.getLength());
                    for (size_t i = 0; i < ufCLFCurve.getLength(); ++i) {
                        tempDerivatives.at(i) = -ufValuationDate / dfUFCLP * ufCLFCurve.wfDerivativeAt(i) *
                                                pow(dfUFCLF, 2.0);
                    }
                    icpClfCashflow->setStartDateUFCLFDerivatives(tempDerivatives);


                    // Proyecta ICP final
                    icpClfCashflow->setEndDateICP(icpValuationDate / icpCurve.getDiscountFactorAt(t2));

                    // Derivadas de ICP final
                    tempDerivatives.resize(icpCurve.getLength());
                    for (size_t i = 0; i < icpCurve.getLength(); ++i) {
                        tempDerivatives.at(i) = icpValuationDate * icpCurve.wfDerivativeAt(i);
                    }
                    icpClfCashflow->setEndDateICPDerivatives(tempDerivatives);

                    // Proyecta UF final
                    dfUFCLP = ufCLPCurve.getDiscountFactorAt(t2);
                    dfUFCLF = ufCLFCurve.getDiscountFactorAt(t2);
                    icpClfCashflow->setEndDateUf(ufValuationDate * dfUFCLF / dfUFCLP);

                    // Derivadas de UF final respecto a curva CLP
                    tempDerivatives.resize(ufCLPCurve.getLength());
                    for (size_t i = 0; i < ufCLPCurve.getLength(); ++i) {
                        tempDerivatives.at(i) = ufValuationDate * dfUFCLF * ufCLPCurve.wfDerivativeAt(i);
                    }
                    icpClfCashflow->setEndDateUFCLPDerivatives(tempDerivatives);

                    // Derivadas de UF final respecto a curva CLF
                    tempDerivatives.resize(ufCLFCurve.getLength());
                    for (size_t i = 0; i < ufCLFCurve.getLength(); ++i) {
                        tempDerivatives.at(i) = -ufValuationDate / dfUFCLP * ufCLFCurve.wfDerivativeAt(i) *
                                                pow(dfUFCLF, 2.0);
                    }
                    icpClfCashflow->setEndDateUFCLFDerivatives(tempDerivatives);

                    return icpClfCashflow;
                }
            }


            void setRatesIcpClfLeg(
                    const QCDate &valuationDate,
                    double icpValuationDate,
                    double ufValuationDate,
                    Leg &IcpClfLeg,
                    ZeroCouponCurve &icpCurve,
                    ZeroCouponCurve &ufCLPCurve,
                    ZeroCouponCurve &ufCLFCurve) {
                for (size_t i = 0; i < IcpClfLeg.size(); ++i) {
                    auto cashflow = setRateIcpClfCashflow(
                            valuationDate,
                            icpValuationDate,
                            ufValuationDate,
                            std::dynamic_pointer_cast<IcpClfCashflow>(IcpClfLeg.getCashflowAt(i)),
                            icpCurve,
                            ufCLPCurve,
                            ufCLFCurve);
                    IcpClfLeg.setCashflowAt(cashflow, i);
                }
            }


            ~ForwardRates() {
            }


        private:
            std::vector<double> _startDateICPDerivatives;
            std::vector<double> _endDateICPDerivatives;
            std::vector<vector<double>> _derivatives2;
        };
    }


#endif //FORWARDRATES_H