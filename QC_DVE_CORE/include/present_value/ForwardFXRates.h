//
// Created by Alvaro Patricio Diaz Valenzuela on 2024-07-23.
//

#ifndef QC_DVE_CORE_FORWARDFXRATES_H
#define QC_DVE_CORE_FORWARDFXRATES_H

#include "Leg.h"
#include "time/QCDate.h"
#include "cashflows/Cashflow.h"
#include "cashflows/IborMultiCurrencyCashflow.h"
#include "cashflows/FixedRateMultiCurrencyCashflow.h"
#include "cashflows/OvernightIndexMultiCurrencyCashflow.h"
#include "cashflows/CompoundedOvernightRateMultiCurrencyCashflow2.h"
#include "present_value/FXRateEstimator.h"
#include "asset_classes/InterestRateCurve.h"


    namespace QCode::Financial {

        class ForwardFXRates {
        public:
            ForwardFXRates() = default;

            std::shared_ptr<Cashflow> setFXRate(
                    const QCDate &valuationDate,
                    Cashflow &mccyCashflow,
                    const FXRateEstimator& fxRateEstimator
                    );

            void setFXRateForLeg(
                    const QCDate &valuationDate,
                    Leg &mccyLeg,
                    const FXRateEstimator& fxRateEstimator
                    );

            // Projects a forward FX rate via covered interest parity (CIP) from a notional-currency
            // curve and a settlement-currency curve, and caches the forward's curve-vertex and
            // spot derivatives on the returned cashflow. Supports FixedRateMultiCurrencyCashflow and
            // IborMultiCurrencyCashflow only.
            std::shared_ptr<Cashflow> setFXRateCIP(
                    const QCDate &valuationDate,
                    double spotFxValue,
                    Cashflow &mccyCashflow,
                    const std::shared_ptr<InterestRateCurve> &notionalCurve,
                    const std::shared_ptr<InterestRateCurve> &settlementCurve
                    );

            void setFXRateForLegCIP(
                    const QCDate &valuationDate,
                    double spotFxValue,
                    Leg &mccyLeg,
                    const std::shared_ptr<InterestRateCurve> &notionalCurve,
                    const std::shared_ptr<InterestRateCurve> &settlementCurve
                    );

        private:
            // Shared CIP-projection math for FixedRateMultiCurrencyCashflow/IborMultiCurrencyCashflow:
            // both expose the same setFxRateIndexValue/setFxRateNotionalCurveDerivatives/
            // setFxRateSettlementCurveDerivatives/setFxRateSpotDerivative surface, so one template
            // avoids duplicating the CIP formula per type.
            template<typename T>
            void _projectFXRateCIP(
                    const QCDate &valuationDate,
                    double spotFxValue,
                    T &cashflow,
                    const QCDate &fxFixingDate,
                    const std::shared_ptr<InterestRateCurve> &notionalCurve,
                    const std::shared_ptr<InterestRateCurve> &settlementCurve
                    );

        };

    } // QCode::Financial

#endif //QC_DVE_CORE_FORWARDFXRATES_H
