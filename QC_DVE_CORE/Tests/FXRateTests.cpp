//
// Created by ADiazV on 23-12-2021.
//

#include "catch/catch-2.hpp"
#include "asset_classes/FXRate.h"
#include "asset_classes/FXRateIndex.h"
#include "asset_classes/Tenor.h"
#include "time/QCDate.h"
#include "time/QCBusinessCalendar.h"


TEST_CASE("FXRate and FXRateIndex") {
    auto usd = std::make_shared<QCUSD>(QCUSD());
    auto clp = std::make_shared<QCCLP>(QCCLP());
    auto usdclp = QCode::Financial::FXRate(usd, clp);
    REQUIRE(usdclp.getCode() == "USDCLP");
    REQUIRE(usdclp.getStrongCcy()->getIsoCode() == "USD");
    REQUIRE(usdclp.getWeakCcy()->getIsoCode() == "CLP");
    REQUIRE(usdclp.strongCcyCode() == "USD");
    REQUIRE(usdclp.weakCcyCode() == "CLP");

    auto oneDay = QCode::Financial::Tenor("1D");
    auto scl = QCBusinessCalendar(QCDate(1, 1, 2021), 20);
    auto obs = QCode::Financial::FXRateIndex(
            std::make_shared<QCode::Financial::FXRate>(usdclp),
            "OBS",
            oneDay,
            oneDay,
            scl);
    auto badDate = QCDate(1, 1, 2021);
    REQUIRE_THROWS_AS(obs.fixingDate(badDate), std::invalid_argument);
    auto goodDate = QCDate(4, 1, 2021);
    REQUIRE(obs.fixingDate(goodDate) == QCDate(31, 12, 2020));
    REQUIRE(obs.valueDate(goodDate) == goodDate);
}

