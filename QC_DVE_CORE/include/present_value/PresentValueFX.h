#ifndef QC_DVE_CORE_PRESENTVALUEFX_H
#define QC_DVE_CORE_PRESENTVALUEFX_H

#include <vector>

#include "Leg.h"
#include "time/QCDate.h"
#include "cashflows/Cashflow.h"
#include "asset_classes/InterestRateCurve.h"

namespace QCode::Financial {

    // Present-values a FixedRateMultiCurrencyCashflow/IborMultiCurrencyCashflow (or a Leg of them)
    // in its settlement currency, discounting the cashflow's cached, unrounded
    // settlementCurrencyAmount() with the settlement curve. Curve-vertex derivatives to both the
    // notional and settlement curves, and the FX delta, are read off the cashflow (cached there by
    // ForwardFXRates::setFXRateCIP/settlementCurrencyAmount) and combined with the settlement
    // curve's own discount-factor derivative via the product rule. Does not itself re-derive FX
    // sensitivity, and does not mutate the cashflow's present_value/discount_factor record() fields.
    class PresentValueFX {
    public:
        PresentValueFX() = default;

        double pv(
                const QCDate &valuationDate,
                const std::shared_ptr<Cashflow> &cashflow,
                const std::shared_ptr<InterestRateCurve> &settlementCurve);

        double pv(
                const QCDate &valuationDate,
                Leg &leg,
                const std::shared_ptr<InterestRateCurve> &settlementCurve);

        [[nodiscard]] std::vector<double> getNotionalCurveDerivatives() const;

        [[nodiscard]] std::vector<double> getSettlementCurveDerivatives() const;

        [[nodiscard]] double getFxDelta() const;

        ~PresentValueFX() = default;

    private:
        std::vector<double> _notionalCurveDerivatives;
        std::vector<double> _settlementCurveDerivatives;
        double _fxDelta{0.0};
    };

} // QCode::Financial

#endif //QC_DVE_CORE_PRESENTVALUEFX_H
