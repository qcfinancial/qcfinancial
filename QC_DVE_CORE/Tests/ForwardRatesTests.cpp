//
// Created by ADiazV on 28-12-2021.
//

#include "catch/catch-2.hpp"
#include "TestHelpers.h"
#include "present_value/ForwardRates.h"
#include "asset_classes/ZeroCouponCurve.h"


TEST_CASE("ForwardRates: setRateCompoundedOvernightCashflow") {
    auto cashflow = TestHelpers::getCashflow();
    auto fixingDates = cashflow.getFixingDates();
    std::map<QCDate, double> timeSeries = {
            {fixingDates[0], .001},
            {fixingDates[1], .002},
            {fixingDates[2], .003},
            {fixingDates[3], .004},
            {fixingDates[4], .005},
    };

    auto fwd = QCode::Financial::ForwardRates();
    auto zcc = TestHelpers::getTestZcc();

    // Case where valuation date is < start date
    auto valuationDate = QCDate(24, 12, 2021);
    auto fixedCashflow = fwd.setRateCompoundedOvernightCashflow(
            valuationDate,
            cashflow.accruedFixing(valuationDate, timeSeries),
            cashflow,
            zcc);

    auto t1 = valuationDate.dayDiff(fixedCashflow->getStartDate());
    auto df1 = zcc.getDiscountFactorAt(t1);

    auto t2 = valuationDate.dayDiff(fixedCashflow->getEndDate());
    auto df2 = zcc.getDiscountFactorAt(t2);

    // Case where valuation date is == start date
    valuationDate = QCDate(27, 12, 2021);
    fixedCashflow = fwd.setRateCompoundedOvernightCashflow(
            valuationDate,
            cashflow.accruedFixing(valuationDate, timeSeries),
            cashflow,
            zcc);

    t2 = valuationDate.dayDiff(fixedCashflow->getEndDate());
    df2 = zcc.getDiscountFactorAt(t2);

    auto expectedResult = (1.0 / df2 - 1.0) * fixedCashflow->getNominal();
    REQUIRE(fixedCashflow->amount() == expectedResult);

    // Case where start date < valuation date < end date
    valuationDate = QCDate(31, 12, 2021);
    fixedCashflow = fwd.setRateCompoundedOvernightCashflow(
            valuationDate,
            cashflow.accruedFixing(valuationDate, timeSeries),
            cashflow,
            zcc);

    auto accFix = fixedCashflow->accruedFixing(valuationDate, timeSeries);
    auto days = fixedCashflow->getStartDate().dayDiff(valuationDate);
    auto accWf = 1 + accFix * days / 360.0;

    t2 = valuationDate.dayDiff(fixedCashflow->getEndDate());
    df2 = zcc.getDiscountFactorAt(t2);

    expectedResult = (accWf / df2 - 1.0) * fixedCashflow->getNominal();
    REQUIRE(fixedCashflow->amount() == expectedResult);

    // Case where valuation date >= end date
    valuationDate = QCDate(7, 1, 2022);
    REQUIRE_THROWS(fwd.setRateCompoundedOvernightCashflow(
            valuationDate,
            cashflow.accruedFixing(valuationDate, timeSeries),
            cashflow,
            zcc));

    timeSeries[QCDate(3, 1, 2022)] = .006;
    timeSeries[QCDate(4, 1, 2022)] = .007;
    timeSeries[QCDate(5, 1, 2022)] = .008;
    timeSeries[QCDate(6, 1, 2022)] = .009;
    accFix = cashflow.accruedFixing(valuationDate, timeSeries);
    fixedCashflow = fwd.setRateCompoundedOvernightCashflow(
            valuationDate,
            accFix,
            cashflow,
            zcc);
    days = fixedCashflow->getStartDate().dayDiff(fixedCashflow->getEndDate());
    accWf = 1 + accFix * days / 360.0;
    expectedResult = (accWf - 1.0) * fixedCashflow->getNominal();
    REQUIRE(fixedCashflow->amount() == expectedResult);

    valuationDate = QCDate(7, 1, 2022);
    fixedCashflow = fwd.setRateCompoundedOvernightCashflow(
            valuationDate,
            accFix,
            cashflow,
            zcc);
    REQUIRE(fixedCashflow->amount() == expectedResult);
}
