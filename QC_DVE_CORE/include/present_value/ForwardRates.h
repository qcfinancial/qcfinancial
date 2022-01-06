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


            std::shared_ptr <IborCashflow2> setRateIborCashflow(
                    const QCDate& valuationDate,
                    Cashflow& iborCashflow,
                    ZeroCouponCurve& curve)
            {
                auto iborCashflow_ = dynamic_cast<IborCashflow2&>(iborCashflow);
                std::vector<double> derivatives;
                derivatives.resize(curve.getLength());
                for (size_t i = 0; i < curve.getLength(); ++i)
                {
                    derivatives.at(i) = 0.0;
                }
                iborCashflow_.setForwardRateWfDerivatives(derivatives);
                auto fixingDate = iborCashflow_.getFixingDates()[0];
                if (valuationDate > fixingDate)
                {
                    return std::make_shared<IborCashflow2>(iborCashflow_);
                }
                QCDate fecha1 = iborCashflow_.getInterestRateIndex()->getStartDate(fixingDate);
                QCDate fecha2 = iborCashflow_.getInterestRateIndex()->getEndDate(fixingDate);
                long t1 = valuationDate.dayDiff(fecha1);
                long t2 = valuationDate.dayDiff(fecha2);
                auto iborRate = iborCashflow_.getInterestRateIndex()->getRate();
                auto tasaForward = curve.getForwardRateWithRate(iborRate, t1, t2);
                for (size_t i = 0; i < curve.getLength(); ++i)
                {
                    derivatives.at(i) = curve.fwdWfDerivativeAt(i);
                }
                iborCashflow_.setRateValue(tasaForward);
                iborCashflow_.setForwardRateWfDerivatives(derivatives);
                auto plazo = valuationDate.dayDiff(iborCashflow_.getSettlementDate());
                return std::make_shared<IborCashflow2>(iborCashflow_);
            }


            void setRatesIborLeg(
                    const QCDate& valuationDate,
                    Leg& iborLeg,
                    ZeroCouponCurve& curve)
            {
                _derivatives2.resize(iborLeg.size(), vector<double>(curve.getLength(), 0.0));
                for (size_t i = 0; i < iborLeg.size(); ++i)
                {
                    auto cashflow = setRateIborCashflow(valuationDate, *(iborLeg.getCashflowAt(i)), curve);
                    iborLeg.setCashflowAt(cashflow, i);
                }
            }


            std::shared_ptr <IcpClpCashflow2> setRateIcpClpCashflow(
                    const QCDate& valuationDate,
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
                    return std::make_shared<IcpClpCashflow2>(icpClpCashflow_);
                }
                else if ((icpClpCashflow_.getStartDate() < valuationDate) &&
                         (valuationDate < icpClpCashflow_.getEndDate()))
                {
                    auto t = valuationDate.dayDiff(icpClpCashflow_.getEndDate());
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
                    // auto t1 = valuationDate.dayDiff(icpClpCashflow_.getSettlementDate());

                    auto t1 = valuationDate.dayDiff(icpClpCashflow_.getStartDate());
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


            void setRatesIcpClpLeg(
                    const QCDate& valuationDate,
                    double icpValuationDate,
                    Leg& icpClpLeg,
                    ZeroCouponCurve& curve)
            {
                for (size_t i = 0; i < icpClpLeg.size(); ++i)
                {
                    auto cashflow = setRateIcpClpCashflow(valuationDate, icpValuationDate,
                                                          *(icpClpLeg.getCashflowAt(i)), curve);
                    icpClpLeg.setCashflowAt(cashflow, i);
                }
            }


            std::shared_ptr <IcpClfCashflow> setRateIcpClfCashflow(
                    const QCDate& valuationDate,
                    double icpValuationDate,
                    double ufValuationDate,
                    std::shared_ptr <IcpClfCashflow> icpClfCashflow,
                    ZeroCouponCurve& icpCurve,
                    ZeroCouponCurve& ufCLPCurve,
                    ZeroCouponCurve& ufCLFCurve)
            {
                std::vector<double> zeroIcpCurveDerivatives;
                zeroIcpCurveDerivatives.resize(icpCurve.getLength());
                for (size_t i = 0; i < icpCurve.getLength(); ++i)
                {
                    zeroIcpCurveDerivatives.at(i) = 0.0;
                }

                std::vector<double> zeroUFCLPCurveDerivatives;
                zeroUFCLPCurveDerivatives.resize(ufCLPCurve.getLength());
                for (size_t i = 0; i < ufCLPCurve.getLength(); ++i)
                {
                    zeroUFCLPCurveDerivatives.at(i) = 0.0;
                }

                std::vector<double> zeroUFCLFCurveDerivatives;
                zeroUFCLFCurveDerivatives.resize(ufCLFCurve.getLength());
                for (size_t i = 0; i < ufCLFCurve.getLength(); ++i)
                {
                    zeroUFCLFCurveDerivatives.at(i) = 0.0;
                }

                // Cashflow vencido. No se proyecta nada.
                if (valuationDate >= icpClfCashflow->getEndDate())
                {
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
                         (valuationDate < icpClfCashflow->getEndDate()))
                {
                    icpClfCashflow->setStartDateICPDerivatives(zeroIcpCurveDerivatives);
                    icpClfCashflow->setStartDateUFCLPDerivatives(zeroUFCLPCurveDerivatives);
                    icpClfCashflow->setStartDateUFCLFDerivatives(zeroUFCLFCurveDerivatives);

                    auto t = valuationDate.dayDiff(icpClfCashflow->getEndDate());

                    // Proyecta ICP final
                    icpClfCashflow->setEndDateICP(icpValuationDate / icpCurve.getDiscountFactorAt(t));

                    // Derivadas de ICP final
                    vector<double> tempDerivatives;
                    tempDerivatives.resize(icpCurve.getLength());
                    for (size_t i = 0; i < icpCurve.getLength(); ++i)
                    {
                        tempDerivatives.at(i) = icpValuationDate * icpCurve.wfDerivativeAt(i);
                    }
                    icpClfCashflow->setEndDateICPDerivatives(tempDerivatives);

                    // Proyecta UF final
                    auto dfUFCLP = ufCLPCurve.getDiscountFactorAt(t);
                    auto dfUFCLF = ufCLFCurve.getDiscountFactorAt(t);
                    icpClfCashflow->setEndDateUf(ufValuationDate * dfUFCLF / dfUFCLP);

                    // Derivadas de UF final respecto a curva CLP
                    tempDerivatives.resize(ufCLPCurve.getLength());
                    for (size_t i = 0; i < ufCLPCurve.getLength(); ++i)
                    {
                        tempDerivatives.at(i) = ufValuationDate * dfUFCLF * ufCLPCurve.wfDerivativeAt(i);
                    }
                    icpClfCashflow->setEndDateUFCLPDerivatives(tempDerivatives);

                    // Derivadas de UF final respecto a curva CLF
                    tempDerivatives.resize(ufCLFCurve.getLength());
                    for (size_t i = 0; i < ufCLFCurve.getLength(); ++i)
                    {
                        tempDerivatives.at(i) = -ufValuationDate /
                                                dfUFCLP * ufCLFCurve.wfDerivativeAt(i) *
                                                pow(dfUFCLF, 2.0);
                    }
                    icpClfCashflow->setEndDateUFCLFDerivatives(tempDerivatives);

                    return icpClfCashflow;
                }
                    // Cashflow en el futuro, se proyectan los índices iniciales y finales
                else
                {
                    auto t1 = valuationDate.dayDiff(icpClfCashflow->getStartDate());
                    auto t2 = valuationDate.dayDiff(icpClfCashflow->getEndDate());

                    // Protecta ICP inicial
                    icpClfCashflow->setStartDateICP(icpValuationDate / icpCurve.getDiscountFactorAt(t1));

                    // Derivadas de ICP inicial
                    vector<double> tempDerivatives;
                    tempDerivatives.resize(icpCurve.getLength());
                    for (size_t i = 0; i < icpCurve.getLength(); ++i)
                    {
                        tempDerivatives.at(i) = icpValuationDate * icpCurve.wfDerivativeAt(i);
                    }
                    icpClfCashflow->setStartDateICPDerivatives(tempDerivatives);

                    // Proyecta UF inicial
                    auto dfUFCLP = ufCLPCurve.getDiscountFactorAt(t1);
                    auto dfUFCLF = ufCLFCurve.getDiscountFactorAt(t1);
                    icpClfCashflow->setStartDateUf(ufValuationDate * dfUFCLF / dfUFCLP);

                    // Derivadas de UF inicial respecto a curva CLP
                    tempDerivatives.resize(ufCLPCurve.getLength());
                    for (size_t i = 0; i < ufCLPCurve.getLength(); ++i)
                    {
                        tempDerivatives.at(i) = ufValuationDate * dfUFCLF * ufCLPCurve.wfDerivativeAt(i);
                    }
                    icpClfCashflow->setStartDateUFCLPDerivatives(tempDerivatives);

                    // Derivadas de UF inicial respecto a curva CLF
                    tempDerivatives.resize(ufCLFCurve.getLength());
                    for (size_t i = 0; i < ufCLFCurve.getLength(); ++i)
                    {
                        tempDerivatives.at(i) = -ufValuationDate / dfUFCLP * ufCLFCurve.wfDerivativeAt(i) *
                                                pow(dfUFCLF, 2.0);
                    }
                    icpClfCashflow->setStartDateUFCLFDerivatives(tempDerivatives);


                    // Proyecta ICP final
                    icpClfCashflow->setEndDateICP(icpValuationDate / icpCurve.getDiscountFactorAt(t2));

                    // Derivadas de ICP final
                    tempDerivatives.resize(icpCurve.getLength());
                    for (size_t i = 0; i < icpCurve.getLength(); ++i)
                    {
                        tempDerivatives.at(i) = icpValuationDate * icpCurve.wfDerivativeAt(i);
                    }
                    icpClfCashflow->setEndDateICPDerivatives(tempDerivatives);

                    // Proyecta UF final
                    dfUFCLP = ufCLPCurve.getDiscountFactorAt(t2);
                    dfUFCLF = ufCLFCurve.getDiscountFactorAt(t2);
                    icpClfCashflow->setEndDateUf(ufValuationDate * dfUFCLF / dfUFCLP);

                    // Derivadas de UF final respecto a curva CLP
                    tempDerivatives.resize(ufCLPCurve.getLength());
                    for (size_t i = 0; i < ufCLPCurve.getLength(); ++i)
                    {
                        tempDerivatives.at(i) = ufValuationDate * dfUFCLF * ufCLPCurve.wfDerivativeAt(i);
                    }
                    icpClfCashflow->setEndDateUFCLPDerivatives(tempDerivatives);

                    // Derivadas de UF final respecto a curva CLF
                    tempDerivatives.resize(ufCLFCurve.getLength());
                    for (size_t i = 0; i < ufCLFCurve.getLength(); ++i)
                    {
                        tempDerivatives.at(i) = -ufValuationDate / dfUFCLP * ufCLFCurve.wfDerivativeAt(i) *
                                                pow(dfUFCLF, 2.0);
                    }
                    icpClfCashflow->setEndDateUFCLFDerivatives(tempDerivatives);

                    return icpClfCashflow;
                }
            }


            void setRatesIcpClfLeg(
                    const QCDate& valuationDate,
                    double icpValuationDate,
                    double ufValuationDate,
                    Leg& IcpClfLeg,
                    ZeroCouponCurve& icpCurve,
                    ZeroCouponCurve& ufCLPCurve,
                    ZeroCouponCurve& ufCLFCurve)
            {
                for (size_t i = 0; i < IcpClfLeg.size(); ++i)
                {
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


            ~ForwardRates()
            {
            }


        private:
            std::vector<double> _startDateICPDerivatives;
            std::vector<double> _endDateICPDerivatives;
            std::vector <vector<double>> _derivatives2;
        };
    }
}

#endif //FORWARDRATES_H