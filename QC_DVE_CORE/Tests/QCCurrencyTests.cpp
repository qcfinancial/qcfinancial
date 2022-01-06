//
// Created by ADiazV on 21-12-2021.
//

#include "catch/catch-2.hpp"
#include "asset_classes/QCCurrency.h"

#include <iostream>

TEST_CASE("QCCurrency Default constructor") {
    auto ccy = QCCurrency();
    REQUIRE(ccy.getIsoCode() == "USD");
    REQUIRE(ccy.getName() == "U. S. Dollar");
    REQUIRE(ccy.getDecimalPlaces() == 2);
    REQUIRE(ccy.getIsoNumber() == 840);
    REQUIRE(ccy.amount(1.2345) == 1.23);
    REQUIRE(ccy.amount(1.23567) == 1.24);
}


TEST_CASE("QCCurrency constructor") {
    std::string nombre = "US Dollar";
    std::string code = "USD";
    auto isoNumber = 840;
    auto decimalPlaces = 3;
    auto ccy = QCCurrency(nombre, code, isoNumber, decimalPlaces);
    REQUIRE(ccy.getIsoCode() == code);
    REQUIRE(ccy.getName() == nombre);
    REQUIRE(ccy.getDecimalPlaces() == decimalPlaces);
    REQUIRE(ccy.getIsoNumber() == isoNumber);
    REQUIRE(ccy.amount(1.2345) == 1.235);
    REQUIRE(ccy.amount(1.23567) == 1.236);
}
