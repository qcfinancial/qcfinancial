//
// Created by ADiazV on 23-12-2021.
//

#include "catch/catch-2.hpp"
#include "asset_classes/Tenor.h"

TEST_CASE("Tenor") {
    auto tenor = QCode::Financial::Tenor("1d");
    REQUIRE(tenor.getDays() == 1);
    REQUIRE(tenor.getMonths() == 0);
    REQUIRE(tenor.getYears() == 0);
    REQUIRE(tenor.getString() == "1D");

    tenor.setTenor("j3Madv");
    REQUIRE(tenor.getDays() == 0);
    REQUIRE(tenor.getMonths() == 3);
    REQUIRE(tenor.getYears() == 0);
    REQUIRE(tenor.getString() == "3M");

    tenor.setTenor("5y");
    REQUIRE(tenor.getDays() == 0);
    REQUIRE(tenor.getMonths() == 0);
    REQUIRE(tenor.getYears() == 5);
    REQUIRE(tenor.getString() == "5Y");

    tenor.setTenor("trash1d-.dc2m%rdt13y()&");
    REQUIRE(tenor.getDays() == 1);
    REQUIRE(tenor.getMonths() == 2);
    REQUIRE(tenor.getYears() == 13);
    REQUIRE(tenor.getString() == "13Y2M1D");
}

