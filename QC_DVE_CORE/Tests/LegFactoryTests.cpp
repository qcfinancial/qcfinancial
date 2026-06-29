//
// Created by ADiazV on 02-01-2022.
//

#include "catch/catch-2.hpp"
#include "Leg.h"
#include "LegFactory.h"
#include "cashflows/Cashflow.h"
#include "cashflows/FixedRateCashflow.h"
#include "time/QCBusinessCalendar.h"
#include "TestHelpers.h"
#include "asset_classes/QCCurrency.h"
#include "asset_classes/QCAct360.h"
#include "asset_classes/QCLinearWf.h"

TEST_CASE("LegFactory: build bullet CompoundedOvernightRateLeg #1") {
    auto startDate = QCDate(3, 1, 2022);
    auto endDate = QCDate(3, 1, 2023);
    auto periodicity = QCode::Financial::Tenor("3M");
    double spread = 0.0;
    auto calendar = QCBusinessCalendar(QCDate(3, 1, 2022), 1);

    auto leg = TestHelpers::getConrLeg(startDate, endDate, periodicity, spread, calendar);

    REQUIRE(leg.size() == 4);

    auto cashflow = leg.getCashflowAt(0);
    auto cashflow_ = std::dynamic_pointer_cast<
            QCode::Financial::CompoundedOvernightRateCashflow>(cashflow);
    REQUIRE(cashflow_->getStartDate() == QCDate(3, 1, 2022));

    auto tempDate = startDate.addMonths((int)periodicity.getTotalMonths());
    tempDate = calendar.nextBusinessDay(tempDate);
    REQUIRE(cashflow_->getEndDate() == tempDate);

    cashflow = leg.getCashflowAt(3);
    cashflow_ = std::dynamic_pointer_cast<
            QCode::Financial::CompoundedOvernightRateCashflow>(cashflow);
    REQUIRE(cashflow_->getEndDate() == calendar.nextBusinessDay(endDate));
}

// Regression tests for fix of _buildBasicDates2 front-stub anchor overshoot.
// Bug: moveToDayOfMonth(qcForward) on a date whose day > endDate.day() crossed into the
// next month, making pseudoStartDate wrong and shifting all subsequent period ends.
// Fix: pseudoStartDate is now computed as adj(_endDate - numWholePeriods*tenor) so the
// last regular period always ends exactly at adj(_endDate).

TEST_CASE("_buildBasicDates2: SHORTFRONT anchor overshoot fix") {
    // Confirmed production bug: startDate=Jan31, endDate=Sep30, 3M, FOLLOW.
    // OLD: adj(Jan31+2M)=Mar31, moveToDayOfMonth(30)=Apr30 (overshoot), leg ends Oct30.
    // NEW: pseudoStart=adj(Sep30-6M)=adj(Mar30)=Mar31, last end=Sep30.
    auto calendar = QCBusinessCalendar(QCDate(1, 1, 2025), 1);
    auto rate = TestHelpers::getLinAct360();
    auto ccy = std::make_shared<QCCurrency>(QCUSD());

    auto leg = QCode::Financial::LegFactory::buildBulletFixedRateLeg(
        QCode::Financial::RecPay::Receive,
        QCDate(31, 1, 2025),
        QCDate(30, 9, 2025),
        QCDate::QCBusDayAdjRules::qcFollow,
        QCode::Financial::Tenor("3M"),
        QCInterestRateLeg::QCStubPeriod::qcShortFront,
        calendar, 0, 1000000.0, false, rate, ccy
    );

    REQUIRE(leg.size() == 3);
    auto cf0 = std::dynamic_pointer_cast<QCode::Financial::FixedRateCashflow>(leg.getCashflowAt(0));
    auto cf1 = std::dynamic_pointer_cast<QCode::Financial::FixedRateCashflow>(leg.getCashflowAt(1));
    auto cf2 = std::dynamic_pointer_cast<QCode::Financial::FixedRateCashflow>(leg.getCashflowAt(2));
    REQUIRE(cf0->getStartDate() == QCDate(31, 1, 2025));
    REQUIRE(cf0->getEndDate()   == QCDate(31, 3, 2025));  // adj(Sep30 - 6M) = adj(Mar30) = Mar31
    REQUIRE(cf1->getEndDate()   == QCDate(30, 6, 2025));  // adj(Sep30 - 3M) = Jun30
    REQUIRE(cf2->getEndDate()   == QCDate(30, 9, 2025));  // adj(endDate) = Sep30
}

TEST_CASE("_buildBasicDates2: LONGFRONT anchor overshoot fix") {
    // startDate=Jan31, endDate=Nov28, 3M, FOLLOW.
    // OLD: moveToDayOfMonth(28,forward) on a 31-day-month source overshoots into next month;
    //      leg ends 2 months early. NEW: backward anchor from endDate, last end=adj(Nov28)=Nov28.
    // numWholePeriods=3: periods = [(Jan31,May28),(May28,Aug28),(Aug28,Nov28)].
    auto calendar = QCBusinessCalendar(QCDate(1, 1, 2025), 1);
    auto rate = TestHelpers::getLinAct360();
    auto ccy = std::make_shared<QCCurrency>(QCUSD());

    auto leg = QCode::Financial::LegFactory::buildBulletFixedRateLeg(
        QCode::Financial::RecPay::Receive,
        QCDate(31, 1, 2025),
        QCDate(28, 11, 2025),
        QCDate::QCBusDayAdjRules::qcFollow,
        QCode::Financial::Tenor("3M"),
        QCInterestRateLeg::QCStubPeriod::qcLongFront,
        calendar, 0, 1000000.0, false, rate, ccy
    );

    REQUIRE(leg.size() == 3);
    auto cf0 = std::dynamic_pointer_cast<QCode::Financial::FixedRateCashflow>(leg.getCashflowAt(0));
    auto cfLast = std::dynamic_pointer_cast<QCode::Financial::FixedRateCashflow>(
        leg.getCashflowAt(leg.size() - 1));
    REQUIRE(cf0->getStartDate()  == QCDate(31, 1, 2025));
    REQUIRE(cfLast->getEndDate() == QCDate(28, 11, 2025)); // adj(endDate) = Nov28
}

TEST_CASE("_buildBasicDates2: LONGFRONT2 anchor overshoot fix") {
    // Same dates as LONGFRONT test; LONGFRONT2 merges the first two temp periods into the stub.
    // Invariant: last period ends at adj(endDate) = Nov28.
    auto calendar = QCBusinessCalendar(QCDate(1, 1, 2025), 1);
    auto rate = TestHelpers::getLinAct360();
    auto ccy = std::make_shared<QCCurrency>(QCUSD());

    auto leg = QCode::Financial::LegFactory::buildBulletFixedRateLeg(
        QCode::Financial::RecPay::Receive,
        QCDate(31, 1, 2025),
        QCDate(28, 11, 2025),
        QCDate::QCBusDayAdjRules::qcFollow,
        QCode::Financial::Tenor("3M"),
        QCInterestRateLeg::QCStubPeriod::qcLongFront2,
        calendar, 0, 1000000.0, false, rate, ccy
    );

    REQUIRE(leg.size() == 3);
    auto cfLast = std::dynamic_pointer_cast<QCode::Financial::FixedRateCashflow>(
        leg.getCashflowAt(leg.size() - 1));
    REQUIRE(cfLast->getEndDate() == QCDate(28, 11, 2025));  // last period ends at adj(endDate)
}

TEST_CASE("_buildBasicDates2: LONGFRONT3 (_auxWithLongFrontNNotEasyCase) anchor overshoot fix") {
    // startDate=Nov30 2024, endDate=Nov28 2025, 3M, FOLLOW, LONGFRONT3.
    // numWholePeriods=3, displacement=2 → 2 output periods.
    // Invariant: last period ends at adj(endDate)=Nov28,2025.
    auto calendar = QCBusinessCalendar(QCDate(1, 1, 2024), 2);
    auto rate = TestHelpers::getLinAct360();
    auto ccy = std::make_shared<QCCurrency>(QCUSD());

    auto leg = QCode::Financial::LegFactory::buildBulletFixedRateLeg(
        QCode::Financial::RecPay::Receive,
        QCDate(30, 11, 2024),
        QCDate(28, 11, 2025),
        QCDate::QCBusDayAdjRules::qcFollow,
        QCode::Financial::Tenor("3M"),
        QCInterestRateLeg::QCStubPeriod::qcLongFront3,
        calendar, 0, 1000000.0, false, rate, ccy
    );

    REQUIRE(leg.size() == 2);
    auto cfLast = std::dynamic_pointer_cast<QCode::Financial::FixedRateCashflow>(
        leg.getCashflowAt(leg.size() - 1));
    REQUIRE(cfLast->getEndDate() == QCDate(28, 11, 2025));
}
