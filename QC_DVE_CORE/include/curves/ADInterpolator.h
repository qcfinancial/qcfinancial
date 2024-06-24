#ifndef ADINTERPOLATOR_H
#define ADINTERPOLATOR_H

#include "curves/ADScatter.h"
#include <memory>


namespace QCode::ADFinancial {
    class ADInterpolator
    {
    public:
        ADInterpolator() = default;


        virtual ad::real interpolateAt(double value, std::vector<double> x, std::vector<ad::real> y) {
            return 0.0;
        };


        virtual ~ADInterpolator() = default;
    };

    long findIndex(double arg, std::vector<double> x) {
        long inf = 0;
        long sup = static_cast<long>(x.size()) - 1;
        long centro = (inf + sup) / 2;
        long indice = centro;
        if (x.size() < 2)
        {
            throw invalid_argument("At least two data points are needed.");
        }
        while (inf <= sup)
        {
            if (arg == x[centro])
            {
                indice = centro;
                inf = static_cast<long>(x.size());
            }
            else
            {
                if (arg < x[centro])
                    sup = centro - 1;
                else
                    inf = centro+1;
                indice = sup;
            }
            centro = (inf + sup) / 2;

        }
        if (indice < 0)
            indice = 0;

        return indice;
    };
}

#endif //ADINTERPOLATOR_H


