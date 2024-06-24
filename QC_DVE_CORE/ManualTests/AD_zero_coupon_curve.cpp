//
// Created by Soporte on 24/05/2020.
//
#include <iostream>

#include "asset_classes/QCAct365.h"
#include "asset_classes/ADContinousWf.h"
#include "asset_classes/ADInterestRate.h"
#include "curves/ADScatter.h"
#include "curves/ADLinearInterpolator.h"


namespace qf = QCode::ADFinancial;

int main()
{
    // Test ADInterestRate
    std::unique_ptr<QCYearFraction> yearFraction = std::make_unique<QCAct365>();
    std::unique_ptr<qf::ADWealthFactor> wealthFactor = std::make_unique<qf::ADContinousWf>();

    auto interestRate = qf::ADInterestRate(std::move(yearFraction), std::move(wealthFactor));

    ad::real rate = .1;
    long days = 730;
    std::function<ad::real(qf::ADInterestRate&, ad::real, long)> wfDays = &qf::ADInterestRate::wfFromDays;
    auto wf = wfDays(interestRate, rate, days);
    auto dwf_drate = ad::derivative(wfDays, wrt(rate), at(interestRate, rate, days));

    std::cout << "Continous Act/365\n" << "Rate: " << rate << "\nWealth Factor: " << wf << "\nDerivative: " << dwf_drate ;
    std::cout << "\n" << interestRate.description();

    // Test ADScatter
    std::vector<double> plazos {1.0, 31.0, 92.0, 365.0};
    std::vector<ad::real> tasas {.01, .02, .03, .04};

    auto scatter = qf::ADScatter(plazos, tasas);
    std::cout << "\n\n" << "Rate at position 0: " << scatter.getValuesAt(0).second << "\n";

    // Test ADLinearInterpolator
    auto linInterpolator = qf::ADLinearInterpolator();
    double x0 = 182.0;
    auto y0 = linInterpolator.interpolateAt(x0, plazos, tasas);
    std::cout << "\nTasa interpolada: " << y0 << "\n";

    std::function<ad::real(
            qf::ADLinearInterpolator&,
            double,
            std::vector<double>,
            std::vector<ad::real>
            )> linInterpol = &qf::ADLinearInterpolator::interpolateAt;

    y0 = linInterpol(linInterpolator, x0, plazos, tasas);
    auto dy0_dx0 = ad::gradient(
            linInterpol,
            wrt(tasas),
            at(linInterpolator, x0, plazos, tasas)
            );
    std::cout << "\nTasa interpolada: " << y0 << "\n";
    std::cout << "Gradiente: " << dy0_dx0 << "\n";
}
