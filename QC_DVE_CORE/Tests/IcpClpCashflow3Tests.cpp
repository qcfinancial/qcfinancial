//
// Created by Soporte on 23-04-2023.
//

#include "catch/catch-2.hpp"
#include "cashflows/IcpClpCashflow3.h"
#include "time/QCDate.h"

TEST_CASE("Settlement Dates") {
    auto cashflow = QCode::Financial::IcpClpCashflow3(
            QCDate(12, 1, 1969),
            QCDate(12, 1, 1970),
            QCDate(12, 1, 1970),
            QCDate(14, 1, 1970),
            1000.0,
            1000.0,
            true,
            .01,
            1.0,
            true,
            10000.0,
            12000.0
            );

    //REQUIRE(cashflow.getAmortSettlementDate() == QCDate(12, 1, 1969));
    //REQUIRE(cashflow.getInterestSettlementDate() == QCDate(14, 1, 1969));

}


