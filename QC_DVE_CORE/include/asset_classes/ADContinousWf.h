#ifndef ADCONTINOUSWF_H
#define ADCONTINOUSWF_H

#include <cmath>
#include "ADWealthFactor.h"

namespace QCode::ADFinancial {
    class ADContinousWf : public ADWealthFactor {
    public:
        ADContinousWf() = default;

        ad::real wf(ad::real rate, ad::real yf) override {
            return exp(rate * yf);
        };


        ad::real rate(ad::real wf, ad::real yf) override {
            return log(wf) / yf;
        };


        std::string description() override {
            return "CON";
        };


        ~ADContinousWf() override = default;
    };
}

#endif //ADCONTINOUSWF_H

