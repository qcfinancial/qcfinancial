//
// Created by Soporte on 24/05/2020.
//
#include <iostream>

#include "asset_classes/ADLinearWf.h"
#include "asset_classes/ADContinousWf.h"

namespace qf = QCode::ADFinancial;

int main()
{
    ad::real yf = 2.0;
    ad::real rate = .1;

    // ---------------------------------------------------------------------------------------------------------------

    auto linearWf = qf::ADLinearWf();

    std::function<ad::real(qf::ADLinearWf&, ad::real, ad::real)> linWf = &qf::ADLinearWf::wf;
    ad::real wf = linWf(linearWf, rate, yf);
    auto dwf_drate = ad::derivative(linWf, wrt(rate), at(linearWf, rate, yf));
    std::cout << "Linear Wf\n" << "Value: " << wf << "\n" << "Derivative: " << dwf_drate << "\n";

    std::function<ad::real(qf::ADLinearWf&, ad::real, ad::real)> linRate = &qf::ADLinearWf::rate;
    rate = linRate(linearWf, wf, yf);
    auto drate_dwf = ad::derivative(linRate, wrt(wf), at(linearWf, wf, yf));
    std::cout << "\nLinear Rate\n" << "Value: " << rate << "\n" << "Derivative: " << drate_dwf << "\n";

    // ---------------------------------------------------------------------------------------------------------------

    auto continousWf = qf::ADContinousWf();
    std::function<ad::real(qf::ADContinousWf&, ad::real, ad::real)> conWf = &qf::ADContinousWf::wf;
    wf = conWf(continousWf, rate, yf);
    dwf_drate = ad::derivative(conWf, wrt(rate), at(continousWf, rate, yf));
    std::cout << "\nContinous Wf\n" << "Value: " << wf << "\n" << "Derivative: " << dwf_drate << "\n";

    std::function<ad::real(qf::ADContinousWf&, ad::real, ad::real)> conRate = &qf::ADContinousWf::rate;
    rate = conRate(continousWf, wf, yf);
    drate_dwf = ad::derivative(conRate, wrt(wf), at(continousWf, wf, yf));
    std::cout << "\nContinous Rate\n" << "Value: " << rate << "\n" << "Derivative: " << drate_dwf;

    return 0;
}
