//
// Created by Soporte on 23-04-2023.
//

#ifndef QC_DVE_CORE_ICPCLPCASHFLOW3_H
#define QC_DVE_CORE_ICPCLPCASHFLOW3_H

#include "IcpClpCashflow2.h"

namespace QCode {

    namespace Financial {

        class IcpClpCashflow3 : public IcpClpCashflow2 {
        public:
            IcpClpCashflow3(
                    const QCDate& startDate,
                    const QCDate& endDate,
                    const QCDate& amortSettlementDate,
                    const QCDate& interestSettlementDate,
                    double nominal,
                    double amortization,
                    bool doesAmortize,
                    double spread,
                    double gearing,
                    bool isAct360,
                    double startDateICP = QCode::Financial::DEFAULT_ICP,
                    double endDateICP = QCode::Financial::DEFAULT_ICP);

            const QCDate& getAmortSettlementDate() const;

            const QCDate& getInterestSettlementDate() const;

        protected:
            QCDate _interestSettlementDate;

        };

    } // Financial
} // QCode

#endif //QC_DVE_CORE_ICPCLPCASHFLOW3_H
