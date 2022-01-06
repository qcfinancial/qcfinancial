//
// Created by ADiazV on 22-12-2021.
//

#include "catch/catch-2.hpp"
#include "time/QCDate.h"
#include "asset_classes/QCInterestRate.h"
#include "asset_classes/QCAct360.h"
#include "asset_classes/QCAct365.h"
#include "asset_classes/QCLinearWf.h"
#include "asset_classes/QCCompoundWf.h"

TEST_CASE("QCInterestRate: LinAct360") {
    auto rateValue = .1;
    auto act360 = std::make_shared<QCAct360>(QCAct360());
    auto lin = std::make_shared<QCLinearWf>(QCLinearWf());
    auto linAct360 = QCInterestRate(rateValue, act360, lin);
    REQUIRE(linAct360.getValue() == rateValue);

    rateValue = .15;
    linAct360.setValue(rateValue);
    REQUIRE(linAct360.getValue() == rateValue);

    auto date1 = QCDate(12, 1, 1969);
    auto date2 = QCDate(12, 1, 1970);
    auto expectedResult = 1 + rateValue * date1.dayDiff(date2) / 360.0;
    REQUIRE(linAct360.wf(date1, date2) == expectedResult);
    REQUIRE(round(linAct360.getRateFromWf(expectedResult, date1, date2) * 1000000) / 1000000 == rateValue);
    expectedResult = date1.dayDiff(date2) / 360.0;
    REQUIRE(linAct360.dwf(date1, date2) == expectedResult);

    auto days = 197;
    expectedResult = 1 + rateValue * days / 360.0;
    REQUIRE(linAct360.wf(days) == expectedResult);
    REQUIRE(round(linAct360.getRateFromWf(expectedResult, days) * 1000000) / 1000000 == rateValue);
    expectedResult = days / 360.0;
    REQUIRE(linAct360.dwf(days) == expectedResult);
}


TEST_CASE("QCInterestRate: ComAct365") {
    auto rateValue = .1;
    auto act365 = std::make_shared<QCAct365>(QCAct365());
    auto com = std::make_shared<QCCompoundWf>(QCCompoundWf());
    auto comAct365 = QCInterestRate(rateValue, act365, com);
    REQUIRE(comAct365.getValue() == rateValue);

    rateValue = .15;
    comAct365.setValue(rateValue);
    REQUIRE(comAct365.getValue() == rateValue);

    auto date1 = QCDate(12, 1, 1969);
    auto date2 = QCDate(12, 1, 1970);
    auto yf = date1.dayDiff(date2) / 365.0;
    auto expectedResult = pow(1 + rateValue, yf);
    REQUIRE(comAct365.wf(date1, date2) == expectedResult);
    REQUIRE(round(comAct365.getRateFromWf(expectedResult, date1, date2) * 1000000) / 1000000 == rateValue);
    expectedResult = yf * pow(1 + rateValue, yf - 1);
    REQUIRE(comAct365.dwf(date1, date2) == expectedResult);

    auto days = 197;
    yf = days / 365.0;
    expectedResult = pow(1 + rateValue, yf);
    REQUIRE(comAct365.wf(days) == expectedResult);
    REQUIRE(round(comAct365.getRateFromWf(expectedResult, days) * 1000000) / 1000000 == rateValue);
    expectedResult = yf * pow(1 + rateValue, yf - 1);
    REQUIRE(comAct365.dwf(days) == expectedResult);
}