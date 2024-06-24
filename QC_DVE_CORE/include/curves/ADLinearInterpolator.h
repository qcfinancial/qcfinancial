#ifndef ADLINEARINTERPOLATOR_H
#define ADLINEARINTERPOLATOR_H

#include "curves/ADScatter.h"
#include "curves/ADInterpolator.h"
#include <memory>


namespace QCode::ADFinancial {
class ADLinearInterpolator : ADInterpolator {
public:
    ADLinearInterpolator() = default;


    ad::real interpolateAt(double value, std::vector<double> x, std::vector<ad::real> y) override
    {
        long i = findIndex(value, x);

        if (i == x.size() - 1)
        {
            return y.at(i);
        }
        else
        {
            auto x1 = x.at(i);
            auto x2 = x.at(i + 1);
            auto y1 = y.at(i);
            auto y2 = y.at(i + 1);

            if (value < x1)
            {
                return y1;
            }
            if (value > x2)
            {
                return y2;
            }

            return (y2 - y1) / (x2 - x1) * (value - x1) + y1;

        };
    };


    ~ADLinearInterpolator() override = default;
};
}

#endif //ADLINEARINTERPOLATOR_H


