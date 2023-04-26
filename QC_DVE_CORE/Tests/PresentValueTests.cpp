//
// Created by ADiazV on 02-01-2022.
//

#include <iomanip>
#include <memory>
#include "catch/catch-2.hpp"
#include "cashflows/Cashflow.h"
#include "asset_classes/InterestRateCurve.h"
#include "present_value/PresentValue.h"
#include "present_value/ForwardRates.h"
#include "TestHelpers.h"

TEST_CASE("PresentValue: CompoundedOvernightRateFlow") {
    auto cashflow = TestHelpers::getCashflowWithAmort();
    auto flatZcc = TestHelpers::getTestZcc();
    std::shared_ptr<QCode::Financial::InterestRateCurve> zcc = TestHelpers::getTestZccPtr();
    auto fwd = QCode::Financial::ForwardRates();
    auto fixingDates = cashflow.getFixingDates();
    double startRate {0.0001};
    auto i {0};
    auto increment {0.0001};
    std::map<QCDate, double> timeSeries;
    for (auto f : fixingDates) {
        timeSeries[f] = startRate + increment * i;
        i++;
    }

    auto pv {QCode::Financial::PresentValue()};

    // Valuation date == start date
    auto valuationDate = QCDate(27, 12, 2021);
    auto expectedResult = cashflow.getNominal();
    auto fixedCashflow = fwd.setRateCompoundedOvernightCashflow(
            valuationDate,
            cashflow.accruedFixing(valuationDate, timeSeries),
            cashflow,
            flatZcc
            );

    // Check zero coupon curve sensitivities and present value
    std::cout << std::fixed;
    std::cout << std::setprecision(6);
    auto cashflowSens = fixedCashflow->getAmountDerivatives();
    auto pips = .0001;
    auto df = zcc->getDiscountFactorAt(valuationDate.dayDiff(cashflow.getEndDate()));

    auto result = pv.pv(valuationDate, fixedCashflow, zcc);
    REQUIRE(result == expectedResult);

    auto discSens = pv.getDerivatives();
    REQUIRE(cashflowSens[0] * df * pips == -discSens[0] * pips);
    REQUIRE(cashflowSens[1] * df * pips == -discSens[1] * pips);
    REQUIRE(cashflowSens[2] * df * pips == -discSens[2] * pips);
    REQUIRE(cashflowSens[3] * df * pips == -discSens[3] * pips);
    REQUIRE(cashflowSens[4] * df * pips == -discSens[4] * pips);
    REQUIRE(cashflowSens[5] * df * pips == -discSens[5] * pips);

    // Valuation date between start date and end date
    valuationDate = QCDate(31, 12, 2021);
    auto accFixing {cashflow.accruedFixing(valuationDate, timeSeries)};
    auto days {cashflow.getStartDate().dayDiff(valuationDate)};
    expectedResult = cashflow.getNominal() * (1 + accFixing * days / 360.0);
    fixedCashflow = fwd.setRateCompoundedOvernightCashflow(
            valuationDate,
            cashflow.accruedFixing(valuationDate, timeSeries),
            cashflow,
            flatZcc);
    result = pv.pv(valuationDate, fixedCashflow, zcc);
    REQUIRE(result == expectedResult);
}

TEST_CASE("PresentValue: IcpClpCashflow3") {
    auto cashflow = QCode::Financial::IcpClpCashflow3(
            QCDate(12, 1, 1969),
            QCDate(12, 1, 1970),
            QCDate(12, 1, 1970),
            QCDate(14, 1, 1970),
            1000.0,
            1000.0,
            true,
            .0,
            1.0,
            true,
            10000.0,
            12000.0
    );

    auto fwd = QCode::Financial::ForwardRates();
    auto pv {QCode::Financial::PresentValue()};
    auto flatZcc = TestHelpers::getTestZcc();
    std::shared_ptr<QCode::Financial::InterestRateCurve> zcc = TestHelpers::getTestZccPtr();
    auto valuationDate = QCDate(12, 1, 1969);
    auto fixedCashflow = fwd.setRateIcpClpCashflow3(
            valuationDate,
            10000.0,
            cashflow,
            flatZcc);

    auto result = pv.pv(valuationDate, fixedCashflow, zcc);
    auto expectedResult = 1000.0;
    REQUIRE(result == expectedResult);
}