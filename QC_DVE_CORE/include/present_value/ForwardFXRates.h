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

        };

    } // QCode::Financial

#endif //QC_DVE_CORE_FORWARDFXRATES_H
