//
// Tests for PresentValueFX: settlement-currency PV via CIP-projected forward FX, per-curve
// derivatives, and FX delta. See openspec/changes/present-value-fx/ for the design this exercises.
//

#include <cmath>
#include <memory>
#include "catch/catch-2.hpp"
#include "asset_classes/FXRate.h"
#include "asset_classes/FXRateIndex.h"
#include "asset_classes/InterestRateCurve.h"
#include "asset_classes/ZeroCouponCurve.h"
#include "cashflows/FixedRateMultiCurrencyCashflow.h"
#include "cashflows/IborMultiCurrencyCashflow.h"
#include "cashflows/OvernightIndexMultiCurrencyCashflow.h"
#include "cashflows/CompoundedOvernightRateMultiCurrencyCashflow2.h"
#include "cashflows/SimpleMultiCurrencyCashflow.h"
#include "cashflows/SimpleCashflow.h"
#include "curves/QCCurve.h"
#include "curves/QCLinearInterpolator.h"
#include "present_value/ForwardFXRates.h"
#include "present_value/PresentValueFX.h"
#include "present_value/FXRateEstimator.h"
#include "Leg.h"
#include "TestHelpers.h"

namespace {
    std::shared_ptr<QCode::Financial::InterestRateCurve> buildCurveWithNodes(
            const std::vector<long>& plazos, const std::vector<double>& rates) {
        std::vector<long> plazosCopy(plazos);
        std::vector<double> rateValues(rates);
        auto curva = std::make_shared<QCCurve<long>>(QCCurve<long>(plazosCopy, rateValues));
        auto interp = std::make_shared<QCLinearInterpolator>(QCLinearInterpolator(curva));
        auto rate = TestHelpers::getLinAct360();
        return std::make_shared<QCode::Financial::ZeroCouponCurve>(interp, rate);
    }

    std::shared_ptr<QCode::Financial::InterestRateCurve> buildCurve(const std::vector<double>& rates) {
        return buildCurveWithNodes({1, 30, 90, 180, 365, 730}, rates);
    }

    std::shared_ptr<QCode::Financial::FXRateIndex> buildUsdClpIndex() {
        auto usd = std::make_shared<QCUSD>(QCUSD());
        auto clp = std::make_shared<QCCLP>(QCCLP());
        auto usdclp = QCode::Financial::FXRate(usd, clp); // USD strong, CLP weak
        auto oneDay = QCode::Financial::Tenor("1D");
        auto calendar = QCBusinessCalendar(QCDate(1, 1, 2021), 20);
        return std::make_shared<QCode::Financial::FXRateIndex>(
                std::make_shared<QCode::Financial::FXRate>(usdclp), "USDCLPOBS", oneDay, oneDay, calendar);
    }

    // Zero-rate FixedRateMultiCurrencyCashflow (interest = 0) so amount() == nominal exactly,
    // which keeps the pv() cancellation-identity checks free of interest-accrual noise.
    QCode::Financial::FixedRateMultiCurrencyCashflow buildFixedMccyCashflow(
            std::shared_ptr<QCCurrency> notionalCcy,
            std::shared_ptr<QCCurrency> settlementCcy,
            std::shared_ptr<QCode::Financial::FXRateIndex> fxIndex,
            const QCDate& endDate,
            const QCDate& fxFixingDate) {
        auto startDate = QCDate(2, 1, 2024);
        double nominal = 1000000.0;
        return {startDate, endDate, endDate, nominal, nominal, true,
                TestHelpers::getLinAct360(), notionalCcy, fxFixingDate, settlementCcy, fxIndex};
    }

    QCode::Financial::IborMultiCurrencyCashflow buildIborMccyCashflow(
            std::shared_ptr<QCCurrency> notionalCcy,
            std::shared_ptr<QCCurrency> settlementCcy,
            std::shared_ptr<QCode::Financial::FXRateIndex> fxIndex,
            const QCDate& endDate,
            const QCDate& fxFixingDate) {
        auto startDate = QCDate(2, 1, 2024);
        double nominal = 1000000.0;
        QCode::Financial::IborMultiCurrencyCashflow cf{
                TestHelpers::getSofr(), startDate, endDate, startDate, endDate,
                nominal, nominal, true, notionalCcy, 0.0, 1.0, fxFixingDate, settlementCcy, fxIndex};
        // TestHelpers::getSofr() is a shared static index mutated in place by other test files
        // (e.g. the deprecated CompoundedOvernightRateCashflow calls _index->setRateValue()), so
        // its rate value at this point depends on test execution order. Force it back to zero
        // here so amount() == nominal exactly, independent of what ran before this test.
        cf.setInterestRateValue(0.0);
        return cf;
    }

    // Zero-rate OvernightIndexMultiCurrencyCashflow: default start/end index values are both 1.0
    // and spread is 0, so the equivalent rate is 0 and settlementCurrencyAmount() == amortization
    // (== nominal), matching the interest-free construction used for Fixed/Ibor above.
    QCode::Financial::OvernightIndexMultiCurrencyCashflow buildOvernightMccyCashflow(
            std::shared_ptr<QCCurrency> notionalCcy,
            std::shared_ptr<QCCurrency> settlementCcy,
            std::shared_ptr<QCode::Financial::FXRateIndex> fxIndex,
            const QCDate& endDate,
            const QCDate& fxFixingDate) {
        auto startDate = QCDate(2, 1, 2024);
        double nominal = 1000000.0;
        return {startDate, endDate, startDate, endDate, endDate, notionalCcy, nominal, nominal, true,
                0.0, 1.0, TestHelpers::getLinAct360(), "SOFRINDEX", 8,
                QCode::Financial::DatesForEquivalentRate::qcAccrual,
                fxFixingDate, settlementCcy, fxIndex};
    }

    // Zero-rate CompoundedOvernightRateMultiCurrencyCashflow2: default initial/end wealth factors
    // are both 1.0, so the compounded rate is 0 and settlementCurrencyAmount() == amortization
    // (== nominal).
    QCode::Financial::CompoundedOvernightRateMultiCurrencyCashflow2 buildCompoundedOvernightMccyCashflow(
            std::shared_ptr<QCCurrency> notionalCcy,
            std::shared_ptr<QCCurrency> settlementCcy,
            std::shared_ptr<QCode::Financial::FXRateIndex> fxIndex,
            const QCDate& endDate,
            const QCDate& fxFixingDate) {
        auto startDate = QCDate(2, 1, 2024);
        double nominal = 1000000.0;
        return {TestHelpers::getSofr(), startDate, endDate, endDate, {}, nominal, nominal, true,
                notionalCcy, 0.0, 1.0, TestHelpers::getLinAct360(), 8, 0, 0,
                fxFixingDate, settlementCcy, fxIndex};
    }

    // SimpleMultiCurrencyCashflow has no interest concept: amount() is the nominal directly.
    QCode::Financial::SimpleMultiCurrencyCashflow buildSimpleMccyCashflow(
            std::shared_ptr<QCCurrency> notionalCcy,
            std::shared_ptr<QCCurrency> settlementCcy,
            std::shared_ptr<QCode::Financial::FXRateIndex> fxIndex,
            const QCDate& endDate,
            const QCDate& fxFixingDate) {
        double nominal = 1000000.0;
        return {endDate, nominal, notionalCcy, fxFixingDate, settlementCcy, fxIndex};
    }

    // Recovers the old combined settlement-curve derivative: only meaningful when the caller used
    // the same curve object for both CIP projection and discounting, in which case the two vectors
    // share a node set and this sum is the true single-curve PV derivative.
    std::vector<double> sumSettlementDerivatives(const QCode::Financial::PresentValueFX& pvfx) {
        auto cip = pvfx.getCipSettlementCurveDerivatives();
        auto disc = pvfx.getDiscountCurveDerivatives();
        REQUIRE(cip.size() == disc.size());
        std::vector<double> total(cip.size());
        for (size_t i = 0; i < cip.size(); ++i) {
            total.at(i) = cip.at(i) + disc.at(i);
        }
        return total;
    }
}

TEST_CASE("PresentValueFX: FixedRateMultiCurrencyCashflow, strong-side notional") {
    auto fxIndex = buildUsdClpIndex();
    auto usd = fxIndex->getFxRate()->getStrongCcy();
    auto clp = fxIndex->getFxRate()->getWeakCcy();
    auto notionalCurve = buildCurve({.01, .015, .02, .025, .03, .035});
    auto settlementCurve = buildCurve({.02, .025, .03, .035, .04, .045});

    auto endDate = QCDate(2, 1, 2025);
    auto fxFixingDate = QCDate(2, 7, 2024);
    auto valuationDate = QCDate(2, 1, 2024); // well before fxFixingDate: still floating
    double spot = 900.0;

    auto cf = buildFixedMccyCashflow(usd, clp, fxIndex, endDate, fxFixingDate);

    auto fwd = QCode::Financial::ForwardFXRates();
    auto projected = fwd.setFXRateCIP(valuationDate, spot, cf, notionalCurve, settlementCurve);

    auto pvfx = QCode::Financial::PresentValueFX();
    auto result = pvfx.pv(valuationDate, projected, settlementCurve);

    auto t = valuationDate.dayDiff(endDate);
    auto dfNotional = notionalCurve->getDiscountFactorAt(t);
    auto expected = 1000000.0 * spot * dfNotional;
    REQUIRE(result == Approx(expected).epsilon(1e-9));

    // Strong-side: summed settlement contributions are a real algebraic cancellation, not a shortcut.
    // Each piece is generally non-zero on its own (checked below); only the sum cancels.
    auto settlementDerivatives = sumSettlementDerivatives(pvfx);
    for (auto d : settlementDerivatives) {
        REQUIRE(d == Approx(0.0).margin(1e-6));
    }
    REQUIRE(std::abs(pvfx.getCipSettlementCurveDerivatives().at(4)) > 1e-6);
    REQUIRE(std::abs(pvfx.getDiscountCurveDerivatives().at(4)) > 1e-6);

    // Notional curve derivative should be non-zero at the vertex bracketing t (index 4: 365 days).
    auto notionalDerivatives = pvfx.getNotionalCurveDerivatives();
    REQUIRE(std::abs(notionalDerivatives.at(4)) > 1e-6);

    // FX delta: finite-difference check against a fresh projection at a bumped spot.
    double bumpedSpot = spot * (1.0 + 1e-6);
    auto cf2 = buildFixedMccyCashflow(usd, clp, fxIndex, endDate, fxFixingDate);
    auto projected2 = fwd.setFXRateCIP(valuationDate, bumpedSpot, cf2, notionalCurve, settlementCurve);
    auto pvfx2 = QCode::Financial::PresentValueFX();
    auto result2 = pvfx2.pv(valuationDate, projected2, settlementCurve);
    auto finiteDiffDelta = (result2 - result) / (bumpedSpot - spot);

    REQUIRE(pvfx.getFxDelta() == Approx(result / spot).epsilon(1e-9));
    REQUIRE(pvfx.getFxDelta() == Approx(finiteDiffDelta).epsilon(1e-3));
}

TEST_CASE("PresentValueFX: FixedRateMultiCurrencyCashflow, weak-side notional") {
    auto fxIndex = buildUsdClpIndex();
    auto usd = fxIndex->getFxRate()->getStrongCcy();
    auto clp = fxIndex->getFxRate()->getWeakCcy();
    auto notionalCurve = buildCurve({.01, .015, .02, .025, .03, .035});
    auto settlementCurve = buildCurve({.02, .025, .03, .035, .04, .045});

    auto endDate = QCDate(2, 1, 2025);
    auto fxFixingDate = QCDate(2, 7, 2024);
    auto valuationDate = QCDate(2, 1, 2024);
    double spot = 900.0;

    // Notional currency is now CLP, the weak side of the pair; settlement is USD.
    auto cf = buildFixedMccyCashflow(clp, usd, fxIndex, endDate, fxFixingDate);

    auto fwd = QCode::Financial::ForwardFXRates();
    auto projected = fwd.setFXRateCIP(valuationDate, spot, cf, notionalCurve, settlementCurve);

    auto pvfx = QCode::Financial::PresentValueFX();
    auto result = pvfx.pv(valuationDate, projected, settlementCurve);
    REQUIRE(result > 0.0);

    // Weak-side: nothing cancels, the summed settlement contribution is structurally non-zero.
    auto settlementDerivatives = sumSettlementDerivatives(pvfx);
    REQUIRE(std::abs(settlementDerivatives.at(4)) > 1e-6);

    double bumpedSpot = spot * (1.0 + 1e-6);
    auto cf2 = buildFixedMccyCashflow(clp, usd, fxIndex, endDate, fxFixingDate);
    auto projected2 = fwd.setFXRateCIP(valuationDate, bumpedSpot, cf2, notionalCurve, settlementCurve);
    auto pvfx2 = QCode::Financial::PresentValueFX();
    auto result2 = pvfx2.pv(valuationDate, projected2, settlementCurve);
    auto finiteDiffDelta = (result2 - result) / (bumpedSpot - spot);

    REQUIRE(pvfx.getFxDelta() == Approx(-result / spot).epsilon(1e-9));
    REQUIRE(pvfx.getFxDelta() == Approx(finiteDiffDelta).epsilon(1e-3));
}

TEST_CASE("PresentValueFX: already-fixed cashflow") {
    auto fxIndex = buildUsdClpIndex();
    auto usd = fxIndex->getFxRate()->getStrongCcy();
    auto clp = fxIndex->getFxRate()->getWeakCcy();
    auto notionalCurve = buildCurve({.01, .015, .02, .025, .03, .035});
    auto settlementCurve = buildCurve({.02, .025, .03, .035, .04, .045});

    auto endDate = QCDate(2, 1, 2025);
    auto fxFixingDate = QCDate(1, 7, 2024);
    auto valuationDate = QCDate(3, 7, 2024); // after fxFixingDate, before endDate: already fixed
    double spot = 900.0;

    auto cf = buildFixedMccyCashflow(usd, clp, fxIndex, endDate, fxFixingDate);
    cf.setFxRateIndexValue(850.0); // the "real" historical fixing

    auto fwd = QCode::Financial::ForwardFXRates();
    auto projected = fwd.setFXRateCIP(valuationDate, spot, cf, notionalCurve, settlementCurve);
    auto &projectedFixed = dynamic_cast<QCode::Financial::FixedRateMultiCurrencyCashflow &>(*projected);
    REQUIRE(projectedFixed.getFxRateIndexValue() == 850.0); // untouched by the projection step

    auto pvfx = QCode::Financial::PresentValueFX();
    auto result = pvfx.pv(valuationDate, projected, settlementCurve);
    REQUIRE(result == Approx(1000000.0 * 850.0 *
            settlementCurve->getDiscountFactorAt(valuationDate.dayDiff(endDate))).epsilon(1e-9));

    for (auto d : pvfx.getNotionalCurveDerivatives()) {
        REQUIRE(d == Approx(0.0).margin(1e-12));
    }
    REQUIRE(pvfx.getFxDelta() == Approx(0.0).margin(1e-12));

    // The FX rate is already fixed: no more CIP sensitivity to the projection curve...
    for (auto d : pvfx.getCipSettlementCurveDerivatives()) {
        REQUIRE(d == Approx(0.0).margin(1e-12));
    }
    // ...but ordinary discounting of the (now-fixed) amount still has real discount-curve risk.
    REQUIRE(std::abs(pvfx.getDiscountCurveDerivatives().at(4)) > 1e-6);
}

TEST_CASE("PresentValueFX: Leg mixing a fixed and a floating cashflow") {
    auto fxIndex = buildUsdClpIndex();
    auto usd = fxIndex->getFxRate()->getStrongCcy();
    auto clp = fxIndex->getFxRate()->getWeakCcy();
    auto notionalCurve = buildCurve({.01, .015, .02, .025, .03, .035});
    auto settlementCurve = buildCurve({.02, .025, .03, .035, .04, .045});

    auto endDate = QCDate(2, 1, 2025);
    auto valuationDate = QCDate(3, 7, 2024);
    double spot = 900.0;

    auto fixedLeg = buildFixedMccyCashflow(usd, clp, fxIndex, endDate, QCDate(1, 7, 2024)); // already fixed
    fixedLeg.setFxRateIndexValue(850.0);
    auto floatingLeg = buildFixedMccyCashflow(usd, clp, fxIndex, endDate, QCDate(2, 1, 2025)); // still floating

    QCode::Financial::Leg leg;
    leg.appendCashflow(std::make_shared<QCode::Financial::FixedRateMultiCurrencyCashflow>(fixedLeg));
    leg.appendCashflow(std::make_shared<QCode::Financial::FixedRateMultiCurrencyCashflow>(floatingLeg));

    auto fwd = QCode::Financial::ForwardFXRates();
    fwd.setFXRateForLegCIP(valuationDate, spot, leg, notionalCurve, settlementCurve);

    auto pvfx = QCode::Financial::PresentValueFX();
    auto legPv = pvfx.pv(valuationDate, leg, settlementCurve);
    auto legDelta = pvfx.getFxDelta();

    auto perCashflowPvfx = QCode::Financial::PresentValueFX();
    auto pv0 = perCashflowPvfx.pv(valuationDate, leg.getCashflowAt(0), settlementCurve);
    auto delta0 = perCashflowPvfx.getFxDelta();
    auto pv1 = perCashflowPvfx.pv(valuationDate, leg.getCashflowAt(1), settlementCurve);
    auto delta1 = perCashflowPvfx.getFxDelta();

    REQUIRE(legPv == Approx(pv0 + pv1).epsilon(1e-9));
    REQUIRE(legDelta == Approx(delta0 + delta1).epsilon(1e-9));
    REQUIRE(delta0 == Approx(0.0).margin(1e-12)); // the fixed cashflow contributes no FX delta

    // The point of this test: leg delta is a per-cashflow sum, not legPv / spot.
    REQUIRE(std::abs(legDelta - legPv / spot) > 1.0);
}

TEST_CASE("PresentValueFX: unsupported cashflow type throws") {
    auto usd = std::make_shared<QCUSD>(QCUSD());
    auto notionalCurve = buildCurve({.01, .015, .02, .025, .03, .035});
    auto settlementCurve = buildCurve({.02, .025, .03, .035, .04, .045});
    auto valuationDate = QCDate(2, 1, 2024);

    QCode::Financial::SimpleCashflow simple(QCDate(2, 1, 2025), 1000.0, usd);

    auto fwd = QCode::Financial::ForwardFXRates();
    REQUIRE_THROWS_AS(
            fwd.setFXRateCIP(valuationDate, 900.0, simple, notionalCurve, settlementCurve),
            std::invalid_argument);

    auto pvfx = QCode::Financial::PresentValueFX();
    REQUIRE_THROWS_AS(
            pvfx.pv(valuationDate, std::make_shared<QCode::Financial::SimpleCashflow>(simple), settlementCurve),
            std::invalid_argument);
}

TEST_CASE("PresentValueFX: IborMultiCurrencyCashflow, strong and weak notional") {
    auto fxIndex = buildUsdClpIndex();
    auto usd = fxIndex->getFxRate()->getStrongCcy();
    auto clp = fxIndex->getFxRate()->getWeakCcy();
    auto notionalCurve = buildCurve({.01, .015, .02, .025, .03, .035});
    auto settlementCurve = buildCurve({.02, .025, .03, .035, .04, .045});

    auto endDate = QCDate(2, 1, 2025);
    auto fxFixingDate = QCDate(2, 7, 2024);
    auto valuationDate = QCDate(2, 1, 2024);
    double spot = 900.0;

    auto fwd = QCode::Financial::ForwardFXRates();

    SECTION("strong-side notional: cancellation identity holds") {
        auto cf = buildIborMccyCashflow(usd, clp, fxIndex, endDate, fxFixingDate);
        auto projected = fwd.setFXRateCIP(valuationDate, spot, cf, notionalCurve, settlementCurve);

        auto pvfx = QCode::Financial::PresentValueFX();
        auto result = pvfx.pv(valuationDate, projected, settlementCurve);

        auto t = valuationDate.dayDiff(endDate);
        auto expected = 1000000.0 * spot * notionalCurve->getDiscountFactorAt(t);
        REQUIRE(result == Approx(expected).epsilon(1e-9));
        REQUIRE(pvfx.getFxDelta() == Approx(result / spot).epsilon(1e-9));
    }

    SECTION("weak-side notional: settlement curve carries real risk") {
        auto cf = buildIborMccyCashflow(clp, usd, fxIndex, endDate, fxFixingDate);
        auto projected = fwd.setFXRateCIP(valuationDate, spot, cf, notionalCurve, settlementCurve);

        auto pvfx = QCode::Financial::PresentValueFX();
        auto result = pvfx.pv(valuationDate, projected, settlementCurve);

        REQUIRE(std::abs(sumSettlementDerivatives(pvfx).at(4)) > 1e-6);
        REQUIRE(pvfx.getFxDelta() == Approx(-result / spot).epsilon(1e-9));
    }
}

TEST_CASE("PresentValueFX: distinct CIP-projection and discount curves") {
    auto fxIndex = buildUsdClpIndex();
    auto usd = fxIndex->getFxRate()->getStrongCcy();
    auto clp = fxIndex->getFxRate()->getWeakCcy();
    auto notionalCurve = buildCurve({.01, .015, .02, .025, .03, .035});
    // CIP-projection curve: same 6-node shape as other tests.
    auto cipCurve = buildCurve({.02, .025, .03, .035, .04, .045});
    // Discount curve: a genuinely different curve object, with a different node count entirely.
    auto discountCurve = buildCurveWithNodes({1, 180, 730}, {.018, .028, .038});

    auto endDate = QCDate(2, 1, 2025);
    auto fxFixingDate = QCDate(2, 7, 2024);
    auto valuationDate = QCDate(2, 1, 2024);
    double spot = 900.0;

    auto cf = buildFixedMccyCashflow(usd, clp, fxIndex, endDate, fxFixingDate);

    auto fwd = QCode::Financial::ForwardFXRates();
    auto projected = fwd.setFXRateCIP(valuationDate, spot, cf, notionalCurve, cipCurve);

    auto pvfx = QCode::Financial::PresentValueFX();
    auto result = pvfx.pv(valuationDate, projected, discountCurve);

    auto t = valuationDate.dayDiff(endDate);
    auto forward = spot * notionalCurve->getDiscountFactorAt(t) / cipCurve->getDiscountFactorAt(t);
    auto expected = 1000000.0 * forward * discountCurve->getDiscountFactorAt(t);
    REQUIRE(result == Approx(expected).epsilon(1e-9));

    // Each vector is sized to (and only to) its own curve — no shared node-index assumption.
    REQUIRE(pvfx.getCipSettlementCurveDerivatives().size() == cipCurve->getLength());
    REQUIRE(pvfx.getDiscountCurveDerivatives().size() == discountCurve->getLength());
    REQUIRE(pvfx.getNotionalCurveDerivatives().size() == notionalCurve->getLength());

    // Both pieces are independently non-zero; they no longer cancel since the curves differ.
    REQUIRE(std::abs(pvfx.getCipSettlementCurveDerivatives().at(4)) > 1e-6);
    REQUIRE(std::abs(pvfx.getDiscountCurveDerivatives().at(1)) > 1e-6);
}

TEST_CASE("PresentValueFX: OvernightIndexMultiCurrencyCashflow, strong and weak notional") {
    auto fxIndex = buildUsdClpIndex();
    auto usd = fxIndex->getFxRate()->getStrongCcy();
    auto clp = fxIndex->getFxRate()->getWeakCcy();
    auto notionalCurve = buildCurve({.01, .015, .02, .025, .03, .035});
    auto settlementCurve = buildCurve({.02, .025, .03, .035, .04, .045});

    auto endDate = QCDate(2, 1, 2025);
    auto fxFixingDate = QCDate(2, 7, 2024);
    auto valuationDate = QCDate(2, 1, 2024);
    double spot = 900.0;

    auto fwd = QCode::Financial::ForwardFXRates();

    SECTION("strong-side notional: cancellation identity holds") {
        auto cf = buildOvernightMccyCashflow(usd, clp, fxIndex, endDate, fxFixingDate);
        auto projected = fwd.setFXRateCIP(valuationDate, spot, cf, notionalCurve, settlementCurve);

        auto pvfx = QCode::Financial::PresentValueFX();
        auto result = pvfx.pv(valuationDate, projected, settlementCurve);

        auto t = valuationDate.dayDiff(endDate);
        auto expected = 1000000.0 * spot * notionalCurve->getDiscountFactorAt(t);
        REQUIRE(result == Approx(expected).epsilon(1e-9));
        REQUIRE(pvfx.getFxDelta() == Approx(result / spot).epsilon(1e-9));

        auto settlementDerivatives = sumSettlementDerivatives(pvfx);
        for (auto d : settlementDerivatives) {
            REQUIRE(d == Approx(0.0).margin(1e-6));
        }
    }

    SECTION("weak-side notional: settlement curve carries real risk") {
        auto cf = buildOvernightMccyCashflow(clp, usd, fxIndex, endDate, fxFixingDate);
        auto projected = fwd.setFXRateCIP(valuationDate, spot, cf, notionalCurve, settlementCurve);

        auto pvfx = QCode::Financial::PresentValueFX();
        auto result = pvfx.pv(valuationDate, projected, settlementCurve);

        REQUIRE(std::abs(sumSettlementDerivatives(pvfx).at(4)) > 1e-6);
        REQUIRE(pvfx.getFxDelta() == Approx(-result / spot).epsilon(1e-9));
    }
}

TEST_CASE("PresentValueFX: OvernightIndexMultiCurrencyCashflow, already-fixed cashflow") {
    auto fxIndex = buildUsdClpIndex();
    auto usd = fxIndex->getFxRate()->getStrongCcy();
    auto clp = fxIndex->getFxRate()->getWeakCcy();
    auto notionalCurve = buildCurve({.01, .015, .02, .025, .03, .035});
    auto settlementCurve = buildCurve({.02, .025, .03, .035, .04, .045});

    auto endDate = QCDate(2, 1, 2025);
    auto fxFixingDate = QCDate(1, 7, 2024);
    auto valuationDate = QCDate(3, 7, 2024);
    double spot = 900.0;

    auto cf = buildOvernightMccyCashflow(usd, clp, fxIndex, endDate, fxFixingDate);
    cf.setFxRateIndexValue(850.0);

    auto fwd = QCode::Financial::ForwardFXRates();
    auto projected = fwd.setFXRateCIP(valuationDate, spot, cf, notionalCurve, settlementCurve);
    auto &projectedFixed = dynamic_cast<QCode::Financial::OvernightIndexMultiCurrencyCashflow &>(*projected);
    REQUIRE(projectedFixed.getFXRateIndexValue() == 850.0);

    auto pvfx = QCode::Financial::PresentValueFX();
    auto result = pvfx.pv(valuationDate, projected, settlementCurve);
    REQUIRE(result == Approx(1000000.0 * 850.0 *
            settlementCurve->getDiscountFactorAt(valuationDate.dayDiff(endDate))).epsilon(1e-9));

    for (auto d : pvfx.getNotionalCurveDerivatives()) {
        REQUIRE(d == Approx(0.0).margin(1e-12));
    }
    REQUIRE(pvfx.getFxDelta() == Approx(0.0).margin(1e-12));
    for (auto d : pvfx.getCipSettlementCurveDerivatives()) {
        REQUIRE(d == Approx(0.0).margin(1e-12));
    }
    REQUIRE(std::abs(pvfx.getDiscountCurveDerivatives().at(4)) > 1e-6);
}

TEST_CASE("PresentValueFX: OvernightIndexMultiCurrencyCashflow, distinct CIP-projection and discount curves") {
    auto fxIndex = buildUsdClpIndex();
    auto usd = fxIndex->getFxRate()->getStrongCcy();
    auto clp = fxIndex->getFxRate()->getWeakCcy();
    auto notionalCurve = buildCurve({.01, .015, .02, .025, .03, .035});
    auto cipCurve = buildCurve({.02, .025, .03, .035, .04, .045});
    auto discountCurve = buildCurveWithNodes({1, 180, 730}, {.018, .028, .038});

    auto endDate = QCDate(2, 1, 2025);
    auto fxFixingDate = QCDate(2, 7, 2024);
    auto valuationDate = QCDate(2, 1, 2024);
    double spot = 900.0;

    auto cf = buildOvernightMccyCashflow(usd, clp, fxIndex, endDate, fxFixingDate);

    auto fwd = QCode::Financial::ForwardFXRates();
    auto projected = fwd.setFXRateCIP(valuationDate, spot, cf, notionalCurve, cipCurve);

    auto pvfx = QCode::Financial::PresentValueFX();
    auto result = pvfx.pv(valuationDate, projected, discountCurve);

    auto t = valuationDate.dayDiff(endDate);
    auto forward = spot * notionalCurve->getDiscountFactorAt(t) / cipCurve->getDiscountFactorAt(t);
    auto expected = 1000000.0 * forward * discountCurve->getDiscountFactorAt(t);
    REQUIRE(result == Approx(expected).epsilon(1e-9));

    REQUIRE(pvfx.getCipSettlementCurveDerivatives().size() == cipCurve->getLength());
    REQUIRE(pvfx.getDiscountCurveDerivatives().size() == discountCurve->getLength());
}

TEST_CASE("PresentValueFX: CompoundedOvernightRateMultiCurrencyCashflow2, strong and weak notional") {
    auto fxIndex = buildUsdClpIndex();
    auto usd = fxIndex->getFxRate()->getStrongCcy();
    auto clp = fxIndex->getFxRate()->getWeakCcy();
    auto notionalCurve = buildCurve({.01, .015, .02, .025, .03, .035});
    auto settlementCurve = buildCurve({.02, .025, .03, .035, .04, .045});

    auto endDate = QCDate(2, 1, 2025);
    auto fxFixingDate = QCDate(2, 7, 2024);
    auto valuationDate = QCDate(2, 1, 2024);
    double spot = 900.0;

    auto fwd = QCode::Financial::ForwardFXRates();

    SECTION("strong-side notional: cancellation identity holds") {
        auto cf = buildCompoundedOvernightMccyCashflow(usd, clp, fxIndex, endDate, fxFixingDate);
        auto projected = fwd.setFXRateCIP(valuationDate, spot, cf, notionalCurve, settlementCurve);

        auto pvfx = QCode::Financial::PresentValueFX();
        auto result = pvfx.pv(valuationDate, projected, settlementCurve);

        auto t = valuationDate.dayDiff(endDate);
        auto expected = 1000000.0 * spot * notionalCurve->getDiscountFactorAt(t);
        REQUIRE(result == Approx(expected).epsilon(1e-9));
        REQUIRE(pvfx.getFxDelta() == Approx(result / spot).epsilon(1e-9));

        auto settlementDerivatives = sumSettlementDerivatives(pvfx);
        for (auto d : settlementDerivatives) {
            REQUIRE(d == Approx(0.0).margin(1e-6));
        }
    }

    SECTION("weak-side notional: settlement curve carries real risk") {
        auto cf = buildCompoundedOvernightMccyCashflow(clp, usd, fxIndex, endDate, fxFixingDate);
        auto projected = fwd.setFXRateCIP(valuationDate, spot, cf, notionalCurve, settlementCurve);

        auto pvfx = QCode::Financial::PresentValueFX();
        auto result = pvfx.pv(valuationDate, projected, settlementCurve);

        REQUIRE(std::abs(sumSettlementDerivatives(pvfx).at(4)) > 1e-6);
        REQUIRE(pvfx.getFxDelta() == Approx(-result / spot).epsilon(1e-9));
    }
}

TEST_CASE("PresentValueFX: CompoundedOvernightRateMultiCurrencyCashflow2, already-fixed cashflow") {
    auto fxIndex = buildUsdClpIndex();
    auto usd = fxIndex->getFxRate()->getStrongCcy();
    auto clp = fxIndex->getFxRate()->getWeakCcy();
    auto notionalCurve = buildCurve({.01, .015, .02, .025, .03, .035});
    auto settlementCurve = buildCurve({.02, .025, .03, .035, .04, .045});

    auto endDate = QCDate(2, 1, 2025);
    auto fxFixingDate = QCDate(1, 7, 2024);
    auto valuationDate = QCDate(3, 7, 2024);
    double spot = 900.0;

    auto cf = buildCompoundedOvernightMccyCashflow(usd, clp, fxIndex, endDate, fxFixingDate);
    cf.setFxRateIndexValue(850.0);

    auto fwd = QCode::Financial::ForwardFXRates();
    auto projected = fwd.setFXRateCIP(valuationDate, spot, cf, notionalCurve, settlementCurve);
    auto &projectedFixed =
            dynamic_cast<QCode::Financial::CompoundedOvernightRateMultiCurrencyCashflow2 &>(*projected);
    REQUIRE(projectedFixed.getFXRateIndexValue() == 850.0);

    auto pvfx = QCode::Financial::PresentValueFX();
    auto result = pvfx.pv(valuationDate, projected, settlementCurve);
    REQUIRE(result == Approx(1000000.0 * 850.0 *
            settlementCurve->getDiscountFactorAt(valuationDate.dayDiff(endDate))).epsilon(1e-9));

    for (auto d : pvfx.getNotionalCurveDerivatives()) {
        REQUIRE(d == Approx(0.0).margin(1e-12));
    }
    REQUIRE(pvfx.getFxDelta() == Approx(0.0).margin(1e-12));
    for (auto d : pvfx.getCipSettlementCurveDerivatives()) {
        REQUIRE(d == Approx(0.0).margin(1e-12));
    }
    REQUIRE(std::abs(pvfx.getDiscountCurveDerivatives().at(4)) > 1e-6);
}

TEST_CASE(
        "PresentValueFX: CompoundedOvernightRateMultiCurrencyCashflow2, distinct CIP-projection and discount curves") {
    auto fxIndex = buildUsdClpIndex();
    auto usd = fxIndex->getFxRate()->getStrongCcy();
    auto clp = fxIndex->getFxRate()->getWeakCcy();
    auto notionalCurve = buildCurve({.01, .015, .02, .025, .03, .035});
    auto cipCurve = buildCurve({.02, .025, .03, .035, .04, .045});
    auto discountCurve = buildCurveWithNodes({1, 180, 730}, {.018, .028, .038});

    auto endDate = QCDate(2, 1, 2025);
    auto fxFixingDate = QCDate(2, 7, 2024);
    auto valuationDate = QCDate(2, 1, 2024);
    double spot = 900.0;

    auto cf = buildCompoundedOvernightMccyCashflow(usd, clp, fxIndex, endDate, fxFixingDate);

    auto fwd = QCode::Financial::ForwardFXRates();
    auto projected = fwd.setFXRateCIP(valuationDate, spot, cf, notionalCurve, cipCurve);

    auto pvfx = QCode::Financial::PresentValueFX();
    auto result = pvfx.pv(valuationDate, projected, discountCurve);

    auto t = valuationDate.dayDiff(endDate);
    auto forward = spot * notionalCurve->getDiscountFactorAt(t) / cipCurve->getDiscountFactorAt(t);
    auto expected = 1000000.0 * forward * discountCurve->getDiscountFactorAt(t);
    REQUIRE(result == Approx(expected).epsilon(1e-9));

    REQUIRE(pvfx.getCipSettlementCurveDerivatives().size() == cipCurve->getLength());
    REQUIRE(pvfx.getDiscountCurveDerivatives().size() == discountCurve->getLength());
}

TEST_CASE("PresentValueFX: SimpleMultiCurrencyCashflow, strong and weak notional") {
    auto fxIndex = buildUsdClpIndex();
    auto usd = fxIndex->getFxRate()->getStrongCcy();
    auto clp = fxIndex->getFxRate()->getWeakCcy();
    auto notionalCurve = buildCurve({.01, .015, .02, .025, .03, .035});
    auto settlementCurve = buildCurve({.02, .025, .03, .035, .04, .045});

    auto endDate = QCDate(2, 1, 2025);
    auto fxFixingDate = QCDate(2, 7, 2024);
    auto valuationDate = QCDate(2, 1, 2024);
    double spot = 900.0;

    auto fwd = QCode::Financial::ForwardFXRates();

    SECTION("strong-side notional: cancellation identity holds") {
        auto cf = buildSimpleMccyCashflow(usd, clp, fxIndex, endDate, fxFixingDate);
        auto projected = fwd.setFXRateCIP(valuationDate, spot, cf, notionalCurve, settlementCurve);

        auto pvfx = QCode::Financial::PresentValueFX();
        auto result = pvfx.pv(valuationDate, projected, settlementCurve);

        auto t = valuationDate.dayDiff(endDate);
        auto expected = 1000000.0 * spot * notionalCurve->getDiscountFactorAt(t);
        REQUIRE(result == Approx(expected).epsilon(1e-9));
        REQUIRE(pvfx.getFxDelta() == Approx(result / spot).epsilon(1e-9));

        auto settlementDerivatives = sumSettlementDerivatives(pvfx);
        for (auto d : settlementDerivatives) {
            REQUIRE(d == Approx(0.0).margin(1e-6));
        }
    }

    SECTION("weak-side notional: settlement curve carries real risk") {
        auto cf = buildSimpleMccyCashflow(clp, usd, fxIndex, endDate, fxFixingDate);
        auto projected = fwd.setFXRateCIP(valuationDate, spot, cf, notionalCurve, settlementCurve);

        auto pvfx = QCode::Financial::PresentValueFX();
        auto result = pvfx.pv(valuationDate, projected, settlementCurve);

        REQUIRE(std::abs(sumSettlementDerivatives(pvfx).at(4)) > 1e-6);
        REQUIRE(pvfx.getFxDelta() == Approx(-result / spot).epsilon(1e-9));
    }
}

TEST_CASE("PresentValueFX: SimpleMultiCurrencyCashflow, already-fixed cashflow") {
    auto fxIndex = buildUsdClpIndex();
    auto usd = fxIndex->getFxRate()->getStrongCcy();
    auto clp = fxIndex->getFxRate()->getWeakCcy();
    auto notionalCurve = buildCurve({.01, .015, .02, .025, .03, .035});
    auto settlementCurve = buildCurve({.02, .025, .03, .035, .04, .045});

    auto endDate = QCDate(2, 1, 2025);
    auto fxFixingDate = QCDate(1, 7, 2024);
    auto valuationDate = QCDate(3, 7, 2024);
    double spot = 900.0;

    auto cf = buildSimpleMccyCashflow(usd, clp, fxIndex, endDate, fxFixingDate);
    cf.setFxRateIndexValue(850.0);

    auto fwd = QCode::Financial::ForwardFXRates();
    auto projected = fwd.setFXRateCIP(valuationDate, spot, cf, notionalCurve, settlementCurve);

    auto pvfx = QCode::Financial::PresentValueFX();
    auto result = pvfx.pv(valuationDate, projected, settlementCurve);
    REQUIRE(result == Approx(1000000.0 * 850.0 *
            settlementCurve->getDiscountFactorAt(valuationDate.dayDiff(endDate))).epsilon(1e-9));

    for (auto d : pvfx.getNotionalCurveDerivatives()) {
        REQUIRE(d == Approx(0.0).margin(1e-12));
    }
    REQUIRE(pvfx.getFxDelta() == Approx(0.0).margin(1e-12));
    for (auto d : pvfx.getCipSettlementCurveDerivatives()) {
        REQUIRE(d == Approx(0.0).margin(1e-12));
    }
    REQUIRE(std::abs(pvfx.getDiscountCurveDerivatives().at(4)) > 1e-6);
}

TEST_CASE("PresentValueFX: SimpleMultiCurrencyCashflow, distinct CIP-projection and discount curves") {
    auto fxIndex = buildUsdClpIndex();
    auto usd = fxIndex->getFxRate()->getStrongCcy();
    auto clp = fxIndex->getFxRate()->getWeakCcy();
    auto notionalCurve = buildCurve({.01, .015, .02, .025, .03, .035});
    auto cipCurve = buildCurve({.02, .025, .03, .035, .04, .045});
    auto discountCurve = buildCurveWithNodes({1, 180, 730}, {.018, .028, .038});

    auto endDate = QCDate(2, 1, 2025);
    auto fxFixingDate = QCDate(2, 7, 2024);
    auto valuationDate = QCDate(2, 1, 2024);
    double spot = 900.0;

    auto cf = buildSimpleMccyCashflow(usd, clp, fxIndex, endDate, fxFixingDate);

    auto fwd = QCode::Financial::ForwardFXRates();
    auto projected = fwd.setFXRateCIP(valuationDate, spot, cf, notionalCurve, cipCurve);

    auto pvfx = QCode::Financial::PresentValueFX();
    auto result = pvfx.pv(valuationDate, projected, discountCurve);

    auto t = valuationDate.dayDiff(endDate);
    auto forward = spot * notionalCurve->getDiscountFactorAt(t) / cipCurve->getDiscountFactorAt(t);
    auto expected = 1000000.0 * forward * discountCurve->getDiscountFactorAt(t);
    REQUIRE(result == Approx(expected).epsilon(1e-9));

    REQUIRE(pvfx.getCipSettlementCurveDerivatives().size() == cipCurve->getLength());
    REQUIRE(pvfx.getDiscountCurveDerivatives().size() == discountCurve->getLength());
}

TEST_CASE("ForwardFXRates: setFXRate applies historical fixing to SimpleMultiCurrencyCashflow") {
    auto fxIndex = buildUsdClpIndex();
    auto usd = fxIndex->getFxRate()->getStrongCcy();
    auto clp = fxIndex->getFxRate()->getWeakCcy();
    auto endDate = QCDate(2, 1, 2025);
    auto fxFixingDate = QCDate(2, 7, 2024);
    auto valuationDate = QCDate(3, 7, 2024);

    QCode::Financial::TimeSeries ts;
    ts[fxFixingDate] = 875.0;
    QCode::Financial::FXRateEstimator estimator(ts, -1.0);

    auto cf = buildSimpleMccyCashflow(usd, clp, fxIndex, endDate, fxFixingDate);

    auto fwd = QCode::Financial::ForwardFXRates();
    auto fixed = fwd.setFXRate(valuationDate, cf, estimator);
    auto &fixedSimple = dynamic_cast<QCode::Financial::SimpleMultiCurrencyCashflow &>(*fixed);

    REQUIRE(fixedSimple.settlementAmount() == Approx(1000000.0 * 875.0).epsilon(1e-9));
}

TEST_CASE("SimpleMultiCurrencyCashflow: settlementAmount() unchanged by settlementCurrencyAmount() refactor") {
    auto fxIndex = buildUsdClpIndex();
    auto usd = fxIndex->getFxRate()->getStrongCcy();
    auto clp = fxIndex->getFxRate()->getWeakCcy();
    auto endDate = QCDate(2, 1, 2025);
    auto fxFixingDate = QCDate(2, 7, 2024);
    double nominal = 1000000.0;
    double fxRateValue = 812.34;

    // Strong-side notional: pre-refactor formula was `_nominal * _fxRateIndexValue`.
    QCode::Financial::SimpleMultiCurrencyCashflow strong(
            endDate, nominal, usd, fxFixingDate, clp, fxIndex, fxRateValue);
    REQUIRE(strong.settlementAmount() == Approx(nominal * fxRateValue).epsilon(1e-12));
    REQUIRE(strong.settlementAmount() == Approx(strong.settlementCurrencyAmount()).epsilon(1e-12));

    // Weak-side notional: pre-refactor formula was `_nominal / _fxRateIndexValue`.
    QCode::Financial::SimpleMultiCurrencyCashflow weak(
            endDate, nominal, clp, fxFixingDate, usd, fxIndex, fxRateValue);
    REQUIRE(weak.settlementAmount() == Approx(nominal / fxRateValue).epsilon(1e-12));
    REQUIRE(weak.settlementAmount() == Approx(weak.settlementCurrencyAmount()).epsilon(1e-12));
}
