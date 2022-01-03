//
// Created by ADiazV on 02-01-2022.
//

#include "catch/catch-2.hpp"
#include "Leg.h"
#include "LegFactory.h"
#include "cashflows/Cashflow.h"
#include "time/QCBusinessCalendar.h"
#include "TestHelpers.h"
#include "asset_classes/QCCurrency.h"

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
