//
// Created by Alvaro Patricio Diaz Valenzuela on 2024-07-23.
//

#include "ForwardFXRates.h"


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
    } // Financial
// QCode