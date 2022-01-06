//
// Created by ADiazV on 23-12-2021.
//

#include "catch/catch-2.hpp"
#include "asset_classes/InterestRateIndex.h"
#include "asset_classes/QCAct360.h"
#include "asset_classes/QCLinearWf.h"
#include "asset_classes/Tenor.h"
#include "time/QCBusinessCalendar.h"
#include "include/TestHelpers.h"


TEST_CASE("InterestRateIndex") {
    std::string code = "SOFRRATE";
    auto startLag = QCode::Financial::Tenor("0D");
    auto tenor = QCode::Financial::Tenor("1D");
    auto ny = QCBusinessCalendar(QCDate(1, 1, 2021), 10);
    auto usd = std::make_shared<QCUSD>(QCUSD());
    auto sofr = QCode::Financial::InterestRateIndex(
            code,
            TestHelpers::getLinAct360(),
            startLag,
            tenor,
            ny,
            ny,
            usd);
    REQUIRE(sofr.getCode() == code);

    sofr.setRateValue(.01);
    REQUIRE(sofr.getRate().getValue() == .01);

    auto fixingDate = QCDate(22, 12, 2021);
    REQUIRE(sofr.getStartDate(fixingDate) == fixingDate);
    REQUIRE(sofr.getEndDate(fixingDate) == QCDate(23, 12, 2021));

    code = "LIBORUSD3M";
    startLag = QCode::Financial::Tenor("2D");
    tenor = QCode::Financial::Tenor("3M");
    auto liborUSD3M = QCode::Financial::InterestRateIndex(
            code,
            TestHelpers::getLinAct360(),
            startLag,
            tenor,
            ny,
            ny,
            usd);
    REQUIRE(liborUSD3M.getCode() == code);

    fixingDate = QCDate(22, 12, 2021);
    auto expectedStartDate = QCDate(24, 12, 2021);
    REQUIRE(liborUSD3M.getStartDate(fixingDate) == expectedStartDate);
    auto expectedEndDate = QCDate(24, 3, 2022);
    REQUIRE(liborUSD3M.getEndDate(fixingDate) == expectedEndDate);
}
