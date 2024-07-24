//
// Created by Alvaro Patricio Diaz Valenzuela on 2024-07-23.
//

#include "FXRateEstimator.h"

namespace QCode::Financial {
    FXRateEstimator::FXRateEstimator(
            const TimeSeries &ts,
            double unknownValue
            ) : _ts(ts),
            _unknownValue(unknownValue) {

    }

    double FXRateEstimator::getValueForDate(const QCDate &fecha) const {
        if (_ts.find(fecha) == _ts.end()) {
            return _unknownValue;
        } else {
            return _ts.at(fecha);
        }
    }
}

