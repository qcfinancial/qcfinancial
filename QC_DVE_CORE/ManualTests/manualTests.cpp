//
// Created by Soporte on 24/05/2020.
//
#include <iostream>

#include "LegFactory.h"
#include "cashflows/Cashflow.h"
#include "cashflows/FixedRateCashflow2.h"
#include "cashflows/IcpClpCashflow2.h"
#include "asset_classes/Tenor.h"

namespace qf = QCode::Financial;

int main()
{
    auto rec =qf::RecPay::Receive;
    auto fechaInicio = QCDate(13, 1, 1969);
    auto fechaFinal = QCDate(13, 1, 1970);
    auto busAdjRule = QCDate::QCBusDayAdjRules::qcModFollow;
    auto periodicity = qf::Tenor("6M");
    auto stubPeriod = QCInterestRateLeg::QCStubPeriod::qcShortFront;
    auto cal = QCBusinessCalendar(fechaInicio, 5);
    auto settLag = 0;
    auto notionalAmort = qf::CustomNotionalAmort();
    notionalAmort.setSize(2);
    notionalAmort.setNotionalAmortAt(0, 1000000.0, 500000.0);
    notionalAmort.setNotionalAmortAt(1, 500000.0, 500000.0);
    auto doesAmortize = true;
    auto spread = 0.0;
    auto gearing = 0.0;

    auto leg = qf::LegFactory::buildCustomAmortIcpClp2Leg(
            rec,
            fechaInicio,
            fechaFinal,
            busAdjRule,
            periodicity,
            stubPeriod,
            cal,
            settLag,
            notionalAmort,
            doesAmortize,
            spread,
            gearing,
            true
            );

    std::cout << "Logrado." << "\n";

    return 0;
}
