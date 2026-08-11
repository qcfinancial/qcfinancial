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
#include "cashflows/SimpleCashflow.h"
#include "curves/QCCurve.h"
#include "curves/QCLinearInterpolator.h"
#include "present_value/ForwardFXRates.h"
#include "present_value/PresentValueFX.h"
#include "Leg.h"
#include "TestHelpers.h"

namespace {
    std::shared_ptr<QCode::Financial::InterestRateCurve> buildCurve(const std::vector<double>& rates) {
        std::vector<long> plazos{1, 30, 90, 180, 365, 730};
        std::vector<double> rateValues(rates);
        auto curva = std::make_shared<QCCurve<long>>(QCCurve<long>(plazos, rateValues));
        auto interp = std::make_shared<QCLinearInterpolator>(QCLinearInterpolator(curva));
        auto rate = TestHelpers::getLinAct360();
        return std::make_shared<QCode::Financial::ZeroCouponCurve>(interp, rate);
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

    // Strong-side: settlement curve's PV derivative is a real algebraic cancellation, not a shortcut.
    auto settlementDerivatives = pvfx.getSettlementCurveDerivatives();
    for (auto d : settlementDerivatives) {
        REQUIRE(d == Approx(0.0).margin(1e-6));
    }

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

    // Weak-side: nothing cancels, settlement curve's derivative is structurally non-zero.
    auto settlementDerivatives = pvfx.getSettlementCurveDerivatives();
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

    // Ordinary discounting of an already-fixed amount still has real settlement-curve risk.
    REQUIRE(std::abs(pvfx.getSettlementCurveDerivatives().at(4)) > 1e-6);
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

        REQUIRE(std::abs(pvfx.getSettlementCurveDerivatives().at(4)) > 1e-6);
        REQUIRE(pvfx.getFxDelta() == Approx(-result / spot).epsilon(1e-9));
    }
}
