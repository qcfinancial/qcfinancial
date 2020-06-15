//
// Created by Soporte on 24/05/2020.
//
#include <iostream>

#include "LegFactory.h"
#include "cashflows/FixedRateCashflow2.h"

int main()
{
    auto leg = QCode::Financial::LegFactory::makeLoan(1000.0, 5, .02, "2020-04-30");

    auto cashflow = std::dynamic_pointer_cast<QCode::Financial::FixedRateCashflow2>(leg.getCashflowAt(0));

    std::cout << cashflow->interest() << std::endl;
    
    return 0;
}

