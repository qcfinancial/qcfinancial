//
// Created by Alvaro Patricio Diaz Valenzuela on 2024-07-23.
//

#ifndef QC_DVE_CORE_FXRATEESTIMATOR_H
#define QC_DVE_CORE_FXRATEESTIMATOR_H

#include <TypeAliases.h>
#include <time/QCDate.h>

namespace QCode::Financial {
    class FXRateEstimator {
    public:
        explicit FXRateEstimator(const TimeSeries& ts, double unkownValue);

        [[nodiscard]] double getValueForDate(const QCDate &fecha) const;

    protected:
        const TimeSeries & _ts;
        double _unknownValue;

    };
} // QCode::Financial


#endif //QC_DVE_CORE_FXRATEESTIMATOR_H
