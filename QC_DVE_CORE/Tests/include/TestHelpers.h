//
// Created by ADiazV on 24-12-2021.
//

#ifndef QC_DVE_CORE_TESTHELPERS_H
#define QC_DVE_CORE_TESTHELPERS_H

#include "asset_classes/InterestRateIndex.h"
#include "asset_classes/QCInterestRate.h"
#include "asset_classes/QCAct360.h"
#include "asset_classes/QCLinearWf.h"
#include "asset_classes/ZeroCouponCurve.h"
#include "cashflows/CompoundedOvernightRateCashflow.h"
#include "curves/QCCurve.h"
#include "curves/QCLinearInterpolator.h"
#include "Leg.h"

namespace TestHelpers {
    std::shared_ptr<QCUSD> getUSD();

    std::shared_ptr<QCAct360> getAct360();

    std::shared_ptr<QCLinearWf> getLin();

    QCInterestRate getLinAct360();

    std::shared_ptr<QCode::Financial::InterestRateIndex> getSofr();

    QCode::Financial::ZeroCouponCurve getTestZcc();

    std::shared_ptr<QCode::Financial::InterestRateCurve> getTestZccPtr();

    QCode::Financial::CompoundedOvernightRateCashflow getCashflow(double spread);

    QCode::Financial::CompoundedOvernightRateCashflow getCashflowWithAmort();

    QCode::Financial::Leg getConrLeg(
            const QCDate &startDate,
            const QCDate &endDate,
            const QCode::Financial::Tenor &periodicity,
            double spread,
            const QCBusinessCalendar &calendar);
}

#endif //QC_DVE_CORE_TESTHELPERS_H
