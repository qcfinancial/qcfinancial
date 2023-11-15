//
// Created by Soporte on 23-04-2023.
//

#include "cashflows/IcpClpCashflow3.h"

namespace QCode {
    Financial::IcpClpCashflow3::IcpClpCashflow3(
            const QCDate &startDate,
            const QCDate &endDate,
            const QCDate &amortSettlementDate,
            const QCDate &interestSettlementDate,
            double nominal,
            double amortization,
            bool doesAmortize,
            double spread,
            double gearing,
            bool isAct360,
            double startDateICP,
            double endDateICP) : IcpClpCashflow2(
                    startDate,
                    endDate,
                    amortSettlementDate,
                    nominal,
                    amortization,
                    doesAmortize,
                    spread,
                    gearing,
                    isAct360,
                    startDateICP,
                    endDateICP),
                    _interestSettlementDate(interestSettlementDate) {

    }

    const QCDate &Financial::IcpClpCashflow3::getAmortSettlementDate() const {
        return _settlementDate;
    }

    const QCDate &Financial::IcpClpCashflow3::getInterestSettlementDate() const {
        return _interestSettlementDate;
    }
} // QCode