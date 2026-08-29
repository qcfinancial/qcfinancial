//
// Created by Alvaro Patricio Diaz Valenzuela on 2024-07-23.
//

#include "present_value/ForwardFXRates.h"


    namespace QCode::Financial {
        std::shared_ptr<Cashflow> ForwardFXRates::setFXRate(
                const QCDate &valuationDate,
                Cashflow &mccyCashflow,
                const FXRateEstimator& fxRateEstimator
                ) {
            auto typeOfCashflow = mccyCashflow.getType();
            if (typeOfCashflow == "FixedRateMultiCurrencyCashflow") {
                auto fixedRateMccyCashflow_ = dynamic_cast<FixedRateMultiCurrencyCashflow &>(mccyCashflow);
                auto fxDate = fixedRateMccyCashflow_.getFXPublishDate();
                fixedRateMccyCashflow_.setFxRateIndexValue(fxRateEstimator.getValueForDate(fxDate));
                return std::make_shared<FixedRateMultiCurrencyCashflow>(fixedRateMccyCashflow_);
            }
            if (typeOfCashflow == "IborMultiCurrencyCashflow") {
                auto iborMccyCashflow_ = dynamic_cast<IborMultiCurrencyCashflow &>(mccyCashflow);
                auto fxDate = iborMccyCashflow_.getFXFixingDate();
                iborMccyCashflow_.setFxRateIndexValue(fxRateEstimator.getValueForDate(fxDate));
                return std::make_shared<IborMultiCurrencyCashflow>(iborMccyCashflow_);
            }
            if (typeOfCashflow == "OvernightIndexMultiCurrencyCashflow") {
                auto overnightIndexMccyCashflow_ = dynamic_cast<OvernightIndexMultiCurrencyCashflow &>(mccyCashflow);
                auto fxDate = overnightIndexMccyCashflow_.getFXRateIndexFixingDate();
                overnightIndexMccyCashflow_.setFxRateIndexValue(fxRateEstimator.getValueForDate(fxDate));
                return std::make_shared<OvernightIndexMultiCurrencyCashflow>(overnightIndexMccyCashflow_);
            }
            if (typeOfCashflow == "CompoundedOvernightRateMultiCurrencyCashflow2") {
                auto compoundedOvernightRateMccyCashflow2_ = dynamic_cast<CompoundedOvernightRateMultiCurrencyCashflow2 &>(mccyCashflow);
                auto fxDate = compoundedOvernightRateMccyCashflow2_.getFXRateIndexFixingDate();
                compoundedOvernightRateMccyCashflow2_.setFxRateIndexValue(fxRateEstimator.getValueForDate(fxDate));
                return std::make_shared<CompoundedOvernightRateMultiCurrencyCashflow2>(compoundedOvernightRateMccyCashflow2_);
            }
            if (typeOfCashflow == "SimpleMultiCurrencyCashflow") {
                auto simpleMccyCashflow_ = dynamic_cast<SimpleMultiCurrencyCashflow &>(mccyCashflow);
                auto fxDate = simpleMccyCashflow_.getFXRateIndexFixingDate();
                simpleMccyCashflow_.setFxRateIndexValue(fxRateEstimator.getValueForDate(fxDate));
                return std::make_shared<SimpleMultiCurrencyCashflow>(simpleMccyCashflow_);
            }
            throw std::invalid_argument("Type of cashflow " + typeOfCashflow + " not implemented.");
        }

        void ForwardFXRates::setFXRateForLeg(
                const QCDate &valuationDate,
                Leg &mccyLeg,
                const FXRateEstimator& fxRateEstimator) {
            for (size_t i = 0; i < mccyLeg.size(); ++i) {
                auto cashflow = setFXRate(valuationDate,* mccyLeg.getCashflowAt(i), fxRateEstimator);
                mccyLeg.setCashflowAt(cashflow, i);
            }
        }

        template<typename T>
        void ForwardFXRates::_projectFXRateCIP(
                const QCDate &valuationDate,
                double spotFxValue,
                T &cashflow,
                const QCDate &fxFixingDate,
                const std::shared_ptr<InterestRateCurve> &notionalCurve,
                const std::shared_ptr<InterestRateCurve> &settlementCurve) {
            std::vector<double> notionalCurveDerivatives(notionalCurve->getLength(), 0.0);
            std::vector<double> settlementCurveDerivatives(settlementCurve->getLength(), 0.0);
            double spotDerivative = 0.0;

            // Only project while still floating: not matured, and the FX rate has not fixed yet.
            // Matured/already-fixed cashflows keep their stored fxRateIndexValue untouched and get
            // all-zero derivatives (mirrors ForwardRates::setRateIborCashflow1's skip-if-fixed check).
            if (valuationDate < cashflow.endDate() && valuationDate <= fxFixingDate) {
                auto t = valuationDate.dayDiff(cashflow.endDate());
                auto dfNotional = notionalCurve->getDiscountFactorAt(t);
                auto dfSettlement = settlementCurve->getDiscountFactorAt(t);
                auto forward = spotFxValue * dfNotional / dfSettlement;

                for (size_t i = 0; i < notionalCurve->getLength(); ++i) {
                    notionalCurveDerivatives.at(i) = spotFxValue * notionalCurve->dfDerivativeAt(i) / dfSettlement;
                }
                for (size_t j = 0; j < settlementCurve->getLength(); ++j) {
                    settlementCurveDerivatives.at(j) = -spotFxValue * dfNotional /
                            (dfSettlement * dfSettlement) * settlementCurve->dfDerivativeAt(j);
                }
                spotDerivative = dfNotional / dfSettlement;

                cashflow.setFxRateIndexValue(forward);
            }

            cashflow.setFxRateNotionalCurveDerivatives(notionalCurveDerivatives);
            cashflow.setFxRateSettlementCurveDerivatives(settlementCurveDerivatives);
            cashflow.setFxRateSpotDerivative(spotDerivative);
        }

        std::shared_ptr<Cashflow> ForwardFXRates::setFXRateCIP(
                const QCDate &valuationDate,
                double spotFxValue,
                Cashflow &mccyCashflow,
                const std::shared_ptr<InterestRateCurve> &notionalCurve,
                const std::shared_ptr<InterestRateCurve> &settlementCurve) {
            auto typeOfCashflow = mccyCashflow.getType();
            if (typeOfCashflow == "FixedRateMultiCurrencyCashflow") {
                auto cf = dynamic_cast<FixedRateMultiCurrencyCashflow &>(mccyCashflow);
                _projectFXRateCIP(valuationDate, spotFxValue, cf, cf.getFXPublishDate(),
                                   notionalCurve, settlementCurve);
                return std::make_shared<FixedRateMultiCurrencyCashflow>(cf);
            }
            if (typeOfCashflow == "IborMultiCurrencyCashflow") {
                auto cf = dynamic_cast<IborMultiCurrencyCashflow &>(mccyCashflow);
                _projectFXRateCIP(valuationDate, spotFxValue, cf, cf.getFXFixingDate(),
                                   notionalCurve, settlementCurve);
                return std::make_shared<IborMultiCurrencyCashflow>(cf);
            }
            if (typeOfCashflow == "OvernightIndexMultiCurrencyCashflow") {
                auto cf = dynamic_cast<OvernightIndexMultiCurrencyCashflow &>(mccyCashflow);
                _projectFXRateCIP(valuationDate, spotFxValue, cf, cf.getFXRateIndexFixingDate(),
                                   notionalCurve, settlementCurve);
                return std::make_shared<OvernightIndexMultiCurrencyCashflow>(cf);
            }
            if (typeOfCashflow == "CompoundedOvernightRateMultiCurrencyCashflow2") {
                auto cf = dynamic_cast<CompoundedOvernightRateMultiCurrencyCashflow2 &>(mccyCashflow);
                _projectFXRateCIP(valuationDate, spotFxValue, cf, cf.getFXRateIndexFixingDate(),
                                   notionalCurve, settlementCurve);
                return std::make_shared<CompoundedOvernightRateMultiCurrencyCashflow2>(cf);
            }
            if (typeOfCashflow == "SimpleMultiCurrencyCashflow") {
                auto cf = dynamic_cast<SimpleMultiCurrencyCashflow &>(mccyCashflow);
                _projectFXRateCIP(valuationDate, spotFxValue, cf, cf.getFXRateIndexFixingDate(),
                                   notionalCurve, settlementCurve);
                return std::make_shared<SimpleMultiCurrencyCashflow>(cf);
            }
            throw std::invalid_argument("Type of cashflow " + typeOfCashflow + " not implemented.");
        }

        void ForwardFXRates::setFXRateForLegCIP(
                const QCDate &valuationDate,
                double spotFxValue,
                Leg &mccyLeg,
                const std::shared_ptr<InterestRateCurve> &notionalCurve,
                const std::shared_ptr<InterestRateCurve> &settlementCurve) {
            for (size_t i = 0; i < mccyLeg.size(); ++i) {
                auto cashflow = setFXRateCIP(valuationDate, spotFxValue, *mccyLeg.getCashflowAt(i),
                                              notionalCurve, settlementCurve);
                mccyLeg.setCashflowAt(cashflow, i);
            }
        }
    } // Financial
// QCode