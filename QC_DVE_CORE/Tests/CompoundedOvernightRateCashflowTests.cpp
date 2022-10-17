//
// Created by ADiazV on 21-12-2021.
//

#include "catch/catch-2.hpp"
#include "TestHelpers.h"

#include "cashflows/CompoundedOvernightRateCashflow.h"

TEST_CASE("CompoundedOvernightRateCashflow: constructor") {
    auto sofr = TestHelpers::getSofr();
    auto startDate = QCDate(27, 12, 2021);
    auto endDate = QCDate(31, 12, 2021);
    auto settlementDate = QCDate(31, 12, 2021);
    std::vector<QCDate> fixingDates {
        QCDate(27, 12, 2021),
        QCDate(28, 12, 2021),
        QCDate(29, 12, 2021),
        QCDate(30, 12, 2021),
    };
    double notional = 10000000;
    double amortization = 1000000;
    bool doesAmortize = true;
    auto ccy = TestHelpers::getUSD();
    double spread = 0.01;
    double gearing = 1.0;
    bool isAct360 = true;
    unsigned int eqRateDecimalPlaces = 8;
    unsigned int lookback = 0;
    unsigned int lockout = 0;

    auto cashflow = QCode::Financial::CompoundedOvernightRateCashflow(
            sofr,
            startDate,
            endDate,
            settlementDate,
            fixingDates,
            notional,
            amortization,
            doesAmortize,
            ccy,
            spread,
            gearing,
            isAct360,
            eqRateDecimalPlaces,
            lookback,
            lockout);

    REQUIRE(cashflow.getStartDate() == startDate);
    REQUIRE(cashflow.getEndDate() == endDate);
    REQUIRE(cashflow.getSettlementDate() == settlementDate);
    auto fixDates = cashflow.getFixingDates();
    REQUIRE(fixDates.size() == 4);
    REQUIRE(fixDates[0] == QCDate(27, 12, 2021));
    REQUIRE(fixDates[1] == QCDate(28, 12, 2021));
    REQUIRE(fixDates[2] == QCDate(29, 12, 2021));
    REQUIRE(fixDates[3] == QCDate(30, 12, 2021));
    REQUIRE(cashflow.getNominal() == notional);
    REQUIRE(cashflow.getAmortization() == amortization);
    REQUIRE(cashflow.doesAmortize());
    REQUIRE(cashflow.ccy()->getIsoCode() == "USD");
    REQUIRE(cashflow.getSpread() == spread);
    REQUIRE(cashflow.getGearing() == gearing);
    REQUIRE(cashflow.getEqRateDecimalPlaces() == eqRateDecimalPlaces);
    REQUIRE(cashflow.getLookBack() == lookback);
    REQUIRE(cashflow.getLockOut() == lockout);
    REQUIRE(cashflow.getTypeOfRate() == "0.000000 Act360 Lin");
    REQUIRE(cashflow.getType() == "COMPOUNDED_OVERNIGHT_RATE");
}

TEST_CASE("CompoundedOvernightRateCashflow: fixing and interest") {
    auto sofr = TestHelpers::getSofr();
    auto startDate = QCDate(27, 12, 2021);
    auto endDate = QCDate(31, 12, 2021);
    auto settlementDate = QCDate(31, 12, 2021);
    std::vector<QCDate> fixingDates {
            QCDate(27, 12, 2021),
            QCDate(28, 12, 2021),
            QCDate(29, 12, 2021),
            QCDate(30, 12, 2021),
    };
    double notional = 10000000;
    double amortization = 1000000;
    bool doesAmortize = true;
    auto ccy = TestHelpers::getUSD();
    double spread = 0.01;
    double gearing = 1.0;
    bool isAct360 = true;
    unsigned int eqRateDecimalPlaces = 8;
    unsigned int lookback = 0;
    unsigned int lockout = 0;

    auto cashflow = QCode::Financial::CompoundedOvernightRateCashflow(
            sofr,
            startDate,
            endDate,
            settlementDate,
            fixingDates,
            notional,
            amortization,
            doesAmortize,
            ccy,
            spread,
            gearing,
            isAct360,
            eqRateDecimalPlaces,
            lookback,
            lockout);

    std::map<QCDate, double> timeSeries = {
            {fixingDates[0], .001},
            {fixingDates[1], .002},
            {fixingDates[2], .003},
            {fixingDates[3], .004},
    };

    REQUIRE_THROWS(cashflow.fixing());
    REQUIRE_THROWS(cashflow.interest());

    auto expectedFixing = 1.0;
    expectedFixing *= 1 + .001 * 1 / 360.; //27 a 28
    expectedFixing *= 1 + .002 * 1 / 360.; //28 a 29
    expectedFixing *= 1 + .003 * 1 / 360.; //29 a 30
    expectedFixing *= 1 + .004 * 1 / 360.; //30 a 31
    expectedFixing = (expectedFixing - 1.0) * 360.0 / 4;
    double factor = std::pow(10, eqRateDecimalPlaces);
    expectedFixing = round(expectedFixing * factor) / factor;
    REQUIRE(cashflow.fixing(timeSeries) == expectedFixing);

    auto expectedInterest = std::round(notional * (expectedFixing + spread) * 4 / 360.0 * 100) / 100.0;
    REQUIRE(std::round(cashflow.interest(timeSeries) * 100) / 100.0 == expectedInterest);

    auto amount = round(cashflow.amount() * 100.0) / 100.0;
    auto expectedAmount = expectedInterest + cashflow.getAmortization();
    REQUIRE(amount == round(expectedAmount * 100) / 100.0 );
}

TEST_CASE("CompoundedOvernightRateCashflow: accrued fixing and accrued interest") {
    auto sofr = TestHelpers::getSofr();
    auto startDate = QCDate(27, 12, 2021);
    auto endDate = QCDate(31, 12, 2021);
    auto settlementDate = QCDate(31, 12, 2021);
    std::vector<QCDate> fixingDates {
            QCDate(27, 12, 2021),
            QCDate(28, 12, 2021),
            QCDate(29, 12, 2021),
            QCDate(30, 12, 2021),
    };
    double notional = 10000000;
    double amortization = 1000000;
    bool doesAmortize = true;
    auto ccy = TestHelpers::getUSD();
    double spread = 0.01;
    double gearing = 1.0;
    bool isAct360 = true;
    unsigned int eqRateDecimalPlaces = 8;
    unsigned int lookback = 0;
    unsigned int lockout = 0;

    auto cashflow = QCode::Financial::CompoundedOvernightRateCashflow(
            sofr,
            startDate,
            endDate,
            settlementDate,
            fixingDates,
            notional,
            amortization,
            doesAmortize,
            ccy,
            spread,
            gearing,
            isAct360,
            eqRateDecimalPlaces,
            lookback,
            lockout);

    std::map<QCDate, double> timeSeries = {
            {fixingDates[0], .001},
            {fixingDates[1], .002},
            {fixingDates[2], .003},
            {fixingDates[3], .004},
    };

    auto accrualDate = QCDate(30, 12, 2021);

    REQUIRE_THROWS(cashflow.accruedFixing(accrualDate));
    REQUIRE_THROWS(cashflow.accruedInterest(accrualDate));

    auto expectedFixing = 1.0;
    expectedFixing *= 1 + .001 * 1 / 360.; //27 a 28
    expectedFixing *= 1 + .002 * 1 / 360.; //28 a 29
    expectedFixing *= 1 + .003 * 1 / 360.; //29 a 30
    expectedFixing = (expectedFixing - 1.0) * 360.0 / 3;
    double factor = std::pow(10, eqRateDecimalPlaces);
    expectedFixing = round(expectedFixing * factor) / factor;
    REQUIRE(cashflow.accruedFixing(accrualDate, timeSeries) == expectedFixing);

    auto expectedInterest = std::round(notional * (expectedFixing+ spread) * 3 / 360.0 * 100) / 100.0;
    REQUIRE(std::round(cashflow.accruedInterest(
            accrualDate, timeSeries) * 100) / 100.0 == expectedInterest);
}

