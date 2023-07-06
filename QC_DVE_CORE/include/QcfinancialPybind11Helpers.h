//
// Created by Alvaro Diaz on 24-06-23.
//

#ifndef QCFINANCIALPYBIND11HELPERS_H
#define QCFINANCIALPYBIND11HELPERS_H

#include <pybind11/pybind11.h>

#include <cashflows/Cashflow.h>

namespace qf = QCode::Financial;

// Trampoline classes
class PyCashflow : public qf::Cashflow {
public:
    /* Inherit the constructors */
    using qf::Cashflow::Cashflow;

    /* Trampoline (need one for each virtual function) */
    double amount() override {
        PYBIND11_OVERRIDE_PURE(
                double,           /* Return type */
                qf::Cashflow,     /* Parent class */
                amount);          /* Name of function in C++ (must match Python name) */
    }

    std::shared_ptr<QCCurrency> ccy() override {
        PYBIND11_OVERRIDE_PURE(
                std::shared_ptr<QCCurrency>,
                qf::Cashflow,
                ccy);
    }

    QCDate date() override {
        PYBIND11_OVERRIDE_PURE(
                QCDate,
                qf::Cashflow,
                date);
    }
};

// Functions
py::tuple show(const std::shared_ptr<qf::SimpleCashflow>& simpleCashflow) {
    auto tuple = py::tuple(3);
    tuple[0] = simpleCashflow->date().description(false);
    tuple[1] = simpleCashflow->amount();
    tuple[2] = simpleCashflow->ccy()->getIsoCode();
    return tuple;
}

#endif //QCFINANCIALPYBIND11HELPERS_H
