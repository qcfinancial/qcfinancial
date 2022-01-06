//
// Created by ADiazV on 22-12-2021.
//

#include "catch/catch-2.hpp"
#include "asset_classes/QCWealthFactor.h"
#include "asset_classes/QCLinearWf.h"
#include "asset_classes/QCCompoundWf.h"
#include "asset_classes/QCContinousWf.h"

TEST_CASE("QCLinearWf") {
    auto lin = QCLinearWf();
    auto rate = .1;
    auto yf = .5;
    auto expectedResult = 1 + rate * yf;
    REQUIRE(lin.wf(rate, yf) == expectedResult);
    REQUIRE(std::round(lin.rate(expectedResult, yf) * 1000000) / 1000000 == rate);

    auto wf = lin.wf(rate, yf);
    REQUIRE(lin.dwf() == yf);
    REQUIRE(lin.d2wf() == 0.0);
}

TEST_CASE("QCCompoundWf") {
    auto com = QCCompoundWf();
    auto rate = .1;
    auto yf = .5;
    auto expectedResult = pow(1 + rate, yf);
    REQUIRE(com.wf(rate, yf) == expectedResult);
    REQUIRE(std::round(com.rate(expectedResult, yf) * 1000000) / 1000000 == rate);

    auto wf = com.wf(rate, yf);
    REQUIRE(com.dwf() == yf * pow(1 + rate, yf - 1));
    REQUIRE(com.d2wf() == yf * (yf - 1) * pow(1 + rate, yf - 2));
}

TEST_CASE("QCContinousWf") {
    auto con = QCContinousWf();
    auto rate = .1;
    auto yf = .5;
    auto expectedResult = exp(rate * yf);
    REQUIRE(con.wf(rate, yf) == expectedResult);
    REQUIRE(std::round(con.rate(expectedResult, yf) * 1000000) / 1000000 == rate);

    auto wf = con.wf(rate, yf);
    REQUIRE(con.dwf() == yf * exp(rate * yf));
    REQUIRE(con.d2wf() == yf * yf * exp(rate * yf));
}
