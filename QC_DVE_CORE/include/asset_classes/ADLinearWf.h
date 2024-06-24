#ifndef ADLINEARWF_H
#define ADLINEARWF_H

#include "ADWealthFactor.h"

namespace QCode::ADFinancial {
    class ADLinearWf : public ADWealthFactor {
    public:
        ADLinearWf() = default;

        ad::real wf(ad::real rate, ad::real yf) override {
            return 1 + rate * yf;
        };


        ad::real rate(ad::real wf, ad::real yf) override {
            return (wf - 1.0) / yf;
        };


        std::string description() override {
            return "LIN";
        };


        ~ADLinearWf() override = default;
    };
}

#endif //ADLINEARWF_H

