#ifndef ADWEALTHFACTOR_H
#define ADWEALTHFACTOR_H

#include <string>
#include <../autodiff/autodiff/forward/dual.hpp>
#include <../autodiff/autodiff/forward/real/eigen.hpp>

namespace ad = autodiff;

// La clase no se implementa directamente como clase abstracta
// para evitar escribir la trampoline class de pybind11
namespace QCode::ADFinancial {
    class ADWealthFactor
    {
    public:
        ADWealthFactor() = default;


        virtual ad::real wf(ad::real rate, ad::real yf)
        {
            return 0.0;
        };


        virtual ad::real rate(ad::real wf, ad::real yf)
        {
            return 0.0;
        };


        virtual std::string description()
        {
            return "Not implemented";
        };


        virtual ~ADWealthFactor() = default;
    };
}

#endif //ADWEALTHFACTOR_H
