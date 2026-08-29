#ifndef QC_DVE_CORE_PRESENTVALUEFX_H
#define QC_DVE_CORE_PRESENTVALUEFX_H

#include <vector>

#include "Leg.h"
#include "time/QCDate.h"
#include "cashflows/Cashflow.h"
#include "asset_classes/InterestRateCurve.h"

namespace QCode::Financial {

    // Present-values a FixedRateMultiCurrencyCashflow, IborMultiCurrencyCashflow,
    // OvernightIndexMultiCurrencyCashflow, CompoundedOvernightRateMultiCurrencyCashflow2, or
    // SimpleMultiCurrencyCashflow (or a Leg of them) in its settlement currency, discounting the
    // cashflow's cached, unrounded settlementCurrencyAmount() with the discount curve. Curve-vertex derivatives to the notional
    // curve, the CIP-projection curve, and the discount curve, plus the FX delta, are read off the
    // cashflow (cached there by ForwardFXRates::setFXRateCIP/settlementCurrencyAmount) and combined
    // with the discount curve's own discount-factor derivative via the product rule.
    // getCipSettlementCurveDerivatives() and getDiscountCurveDerivatives() are independent — each is
    // sized to its own curve's length and neither assumes the two curves coincide. Callers who used
    // the same curve for both CIP projection and discounting sum the two vectors themselves. Does
    // not itself re-derive FX sensitivity, and does not mutate the cashflow's
    // present_value/discount_factor record() fields.
    class PresentValueFX {
    public:
        PresentValueFX() = default;

        double pv(
                const QCDate &valuationDate,
                const std::shared_ptr<Cashflow> &cashflow,
                const std::shared_ptr<InterestRateCurve> &discountCurve);

        double pv(
                const QCDate &valuationDate,
                Leg &leg,
                const std::shared_ptr<InterestRateCurve> &discountCurve);

        [[nodiscard]] std::vector<double> getNotionalCurveDerivatives() const;

        [[nodiscard]] std::vector<double> getCipSettlementCurveDerivatives() const;

        [[nodiscard]] std::vector<double> getDiscountCurveDerivatives() const;

        [[nodiscard]] double getFxDelta() const;

        ~PresentValueFX() = default;

    private:
        std::vector<double> _notionalCurveDerivatives;
        std::vector<double> _cipSettlementCurveDerivatives;
        std::vector<double> _discountCurveDerivatives;
        double _fxDelta{0.0};
    };

} // QCode::Financial

#endif //QC_DVE_CORE_PRESENTVALUEFX_H
