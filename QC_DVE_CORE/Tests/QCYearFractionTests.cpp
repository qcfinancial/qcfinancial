//
// Created by ADiazV on 22-12-2021.
//

#include "catch/catch-2.hpp"
#include "time/QCDate.h"
#include "asset_classes/QCAct360.h"
#include "asset_classes/QCAct365.h"
#include "asset_classes/QC30360.h"
#include "asset_classes/QCAct30.h"
#include "asset_classes/QC3030.h"
#include "asset_classes/QCActAct.h"

auto act360 = QCAct360();
auto act365 = QCAct365();
auto t30360 = QC30360();
auto act30 = QCAct30();
auto t3030 = QC3030();
auto actact = QCActAct();

TEST_CASE("QCYearFraction") {
    auto date1 = QCDate(15, 2, 1980);
    auto date2 = QCDate(15, 2, 1981);

    REQUIRE(act360.countDays(date1, date2) == 366);
    REQUIRE(act365.countDays(date1, date2) == 366);
    REQUIRE(t30360.countDays(date1, date2) == 360);
    REQUIRE(act30.countDays(date1, date2) == 366);
    REQUIRE(t3030.countDays(date1, date2) == 360);
    REQUIRE(actact.countDays(date1, date2) == 366);

    REQUIRE(act360.yf(date1, date2) == 366.0 / 360);
    REQUIRE(act365.yf(date1, date2) == 366.0 / 365);
    REQUIRE(t30360.yf(date1, date2) == 360.0 / 360);
    REQUIRE(act30.yf(date1, date2) == 366.0 / 30);
    REQUIRE(t3030.yf(date1, date2) == 360.0 / 30);
    REQUIRE(actact.yf(date1, date2) == 366.0 / 366);
}