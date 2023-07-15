//
// Created by Alvaro Diaz on 24-06-23.
//

#ifndef QCFINANCIALPYBIND11HELPERS_H
#define QCFINANCIALPYBIND11HELPERS_H

#include <pybind11/pybind11.h>

#include <cashflows/Cashflow.h>
#include <cashflows/SimpleMultiCurrencyCashflow.h>
#include <cashflows/LinearInterestRateCashflow.h>

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

class PyLinearInterestRateCashflow : qf::LinearInterestRateCashflow {
public:
    /* Inherit the constructors */
    using qf::LinearInterestRateCashflow::LinearInterestRateCashflow;

    bool isExpired(const QCDate& fecha) override {
        PYBIND11_OVERRIDE(
                bool,
                qf::LinearInterestRateCashflow,
                isExpired,
                fecha);
    }

    double amount() override {
        PYBIND11_OVERRIDE_PURE(
                double,
                qf::LinearInterestRateCashflow,
                amount);
    }

    shared_ptr<QCCurrency> ccy() override {
        PYBIND11_OVERRIDE_PURE(
                shared_ptr<QCCurrency>,
                qf::LinearInterestRateCashflow,
                ccy);
    }


    QCDate date() override {
        PYBIND11_OVERRIDE_PURE(
                QCDate,
                qf::LinearInterestRateCashflow,
                date);

    }


    std::string getType() const override {
        PYBIND11_OVERRIDE_PURE(
                std::string ,
                qf::LinearInterestRateCashflow,
                getType);
    }


    shared_ptr<QCCurrency> getInitialCcy() const override {
        PYBIND11_OVERRIDE_PURE(
                std::shared_ptr<QCCurrency>,
                qf::LinearInterestRateCashflow,
                getInitialCcy);
    }


    const QCDate &getStartDate() const override {
        PYBIND11_OVERRIDE_PURE(
                const QCDate,
                qf::LinearInterestRateCashflow,
                getStartDate);
    }


    const QCDate &getEndDate() const override {
        PYBIND11_OVERRIDE_PURE(
                QCDate,
                qf::LinearInterestRateCashflow,
                getEndDate);
    }


    const QCDate &getSettlementDate() const override {
        PYBIND11_OVERRIDE_PURE(
                const QCDate,
                qf::LinearInterestRateCashflow,
                getSettlementDate);
    }


    const qf::DateList& getFixingDates() const override {
        PYBIND11_OVERRIDE_PURE(
                const qf::DateList ,
                qf::LinearInterestRateCashflow,
                getFixingDates);
    }


    double getNominal() const override {
        PYBIND11_OVERRIDE_PURE(
                double,
                qf::LinearInterestRateCashflow,
                getNominal);
    }


    double nominal(const QCDate& fecha) const override {
        PYBIND11_OVERRIDE_PURE(
                double,
                qf::LinearInterestRateCashflow,
                nominal,
                fecha);
    }


    double getAmortization() const override {
        PYBIND11_OVERRIDE_PURE(
                double,
                qf::LinearInterestRateCashflow,
                getAmortization);
    }


    double amortization() const override {
        PYBIND11_OVERRIDE_PURE(
                double,
                qf::LinearInterestRateCashflow,
                amortization);
    }


    bool doesAmortize() const override {
        PYBIND11_OVERRIDE_PURE(
                bool,
                qf::LinearInterestRateCashflow,
                doesAmortize);
    }


    double interest() override {
        PYBIND11_OVERRIDE_PURE(
                double,
                qf::LinearInterestRateCashflow,
                interest);
    }


    double interest(const qf::TimeSeries &fixings) override {
        PYBIND11_OVERRIDE_PURE(
                double,
                qf::LinearInterestRateCashflow,
                interest,
                fixings);
    }


    double fixing() override {
        PYBIND11_OVERRIDE_PURE(
                double,
                qf::LinearInterestRateCashflow,
                fixing);
    }


    double fixing(const qf::TimeSeries &fixings) override {
        PYBIND11_OVERRIDE_PURE(
                double,
                qf::LinearInterestRateCashflow,
                fixing,
                fixings);
    }


    double accruedInterest(const QCDate &fecha) override {
        PYBIND11_OVERRIDE_PURE(
                double,
                qf::LinearInterestRateCashflow,
                accruedInterest,
                fecha);
    }


    double accruedInterest(const QCDate &fecha, const qf::TimeSeries &fixings) override {
        PYBIND11_OVERRIDE_PURE(
                double,
                qf::LinearInterestRateCashflow,
                accruedInterest,
                fecha,
                fixings);
    }


    double accruedFixing(const QCDate &fecha) override {
        PYBIND11_OVERRIDE_PURE(
                double,
                qf::LinearInterestRateCashflow,
                accruedFixing,
                fecha);
    }


    double accruedFixing(const QCDate &fecha, const qf::TimeSeries &fixings) override {
        PYBIND11_OVERRIDE_PURE(
                double,
                qf::LinearInterestRateCashflow,
                accruedFixing,
                fecha,
                fixings);
    }
};

// Show Functions
py::tuple show(const std::shared_ptr<qf::SimpleCashflow>& simpleCashflow) {
    if (simpleCashflow->getType() == "SimpleCashflow") {
        auto tuple = py::tuple(3);
        tuple[0] = simpleCashflow->date().description(false);
        tuple[1] = simpleCashflow->amount();
        tuple[2] = simpleCashflow->ccy()->getIsoCode();
        return tuple;
    } else {
        auto tuple = py::tuple(8);
        auto simpleMCCashflow = std::dynamic_pointer_cast<qf::SimpleMultiCurrencyCashflow>(simpleCashflow);
        auto cashflow = simpleMCCashflow->wrap();
        tuple[0] = std::get<0>(*cashflow).description(false);
        tuple[1]  = std::get<1>(*cashflow);
        tuple[2]  = std::get<2>(*cashflow)->getIsoCode();
        tuple[3]  = std::get<3>(*cashflow).description(false);
        tuple[4]  = std::get<4>(*cashflow)->getIsoCode();
        tuple[5]  = std::get<5>(*cashflow)->getCode();
        tuple[6]  = std::get<6>(*cashflow);
        tuple[7]  = std::get<7>(*cashflow);
        return tuple;
    }

}

py::tuple show(const std::shared_ptr<qf::FixedRateCashflow>& fixedRateCashflow) {
    if (fixedRateCashflow->getType() == "FixedRateCashflow") {
        auto tuple = py::tuple(11);
        auto cashflow = fixedRateCashflow->wrap();
        tuple[0] = std::get<0>(*cashflow).description(false);
        tuple[1] = std::get<1>(*cashflow).description(false);
        tuple[2] = std::get<2>(*cashflow).description(false);
        tuple[3] = std::get<3>(*cashflow);
        tuple[4] = std::get<4>(*cashflow);
        tuple[5] = std::get<5>(*cashflow);
        tuple[6] = std::get<6>(*cashflow);
        tuple[7] = fixedRateCashflow->amount();
        tuple[8] = std::get<7>(*cashflow)->getIsoCode();
        tuple[9] = std::get<8>(*cashflow).getValue();
        std::string wf = std::get<8>(*cashflow).getWealthFactor()->description();
        std::string yf = std::get<8>(*cashflow).getYearFraction()->description();
        tuple[10] = wf + yf;
        return tuple;
    } else {
        auto tuple = py::tuple(17);
        auto fixedRateMCCashflow = std::dynamic_pointer_cast<qf::FixedRateMultiCurrencyCashflow>(fixedRateCashflow);
        auto cashflow = fixedRateMCCashflow->wrap();
        tuple[0] = std::get<0>(*cashflow).description(false);
        tuple[1] = std::get<1>(*cashflow).description(false);
        tuple[2] = std::get<2>(*cashflow).description(false);
        tuple[3] = std::get<3>(*cashflow);
        tuple[4] = std::get<4>(*cashflow);
        tuple[5] = std::get<5>(*cashflow);
        tuple[6] = std::get<6>(*cashflow);
        tuple[7] = fixedRateCashflow->amount();
        tuple[8] = std::get<7>(*cashflow)->getIsoCode();
        tuple[9] = std::get<8>(*cashflow).getValue();
        std::string wf = std::get<8>(*cashflow).getWealthFactor()->description();
        std::string yf = std::get<8>(*cashflow).getYearFraction()->description();
        tuple[10] = wf + yf;
        tuple[11] = std::get<9>(*cashflow).description(false);
        tuple[12] = std::get<10>(*cashflow)->getIsoCode();
        tuple[13] = std::get<11>(*cashflow)->getCode();
        tuple[14] = std::get<12>(*cashflow);
        tuple[15] = std::get<13>(*cashflow);
        tuple[16] = std::get<14>(*cashflow);

        return tuple;
    };

}

py::tuple show(const std::shared_ptr<qf::FixedRateCashflow2>& fixedRateCashflow) {
    auto tuple = py::tuple(11);
    auto cashflow = fixedRateCashflow->wrap();
    tuple[0] = std::get<0>(*cashflow).description(false);
    tuple[1] = std::get<1>(*cashflow).description(false);
    tuple[2] = std::get<2>(*cashflow).description(false);
    tuple[3] = std::get<3>(*cashflow);
    tuple[4] = std::get<4>(*cashflow);
    tuple[5] = std::get<5>(*cashflow);
    tuple[6] = std::get<6>(*cashflow);
    tuple[7] = fixedRateCashflow->amount();
    tuple[8] = std::get<7>(*cashflow)->getIsoCode();
    tuple[9] = std::get<8>(*cashflow).getValue();
    std::string wf = std::get<8>(*cashflow).getWealthFactor()->description();
    std::string yf = std::get<8>(*cashflow).getYearFraction()->description();
    tuple[10] = wf + yf;
    return tuple;
}

py::tuple show(const std::shared_ptr<qf::IborCashflow>& iborCashflow) {
    if (iborCashflow->getType() == "IborCashflow") {
        auto tuple = py::tuple(15);
        auto cashflow = iborCashflow->wrap();
        tuple[0] = std::get<0>(*cashflow).description(false);
        tuple[1] = std::get<1>(*cashflow).description(false);
        tuple[2] = std::get<2>(*cashflow).description(false);
        tuple[3] = std::get<3>(*cashflow).description(false);
        tuple[4] = std::get<4>(*cashflow);
        tuple[5] = std::get<5>(*cashflow);
        tuple[6] = std::get<6>(*cashflow);
        tuple[7] = std::get<7>(*cashflow);
        tuple[8] = std::get<8>(*cashflow);
        tuple[9] = std::get<9>(*cashflow)->getIsoCode();
        tuple[10] = std::get<10>(*cashflow);
        tuple[11] = std::get<14>(*cashflow);
        tuple[12] = std::get<12>(*cashflow);
        tuple[13] = std::get<13>(*cashflow);
        std::string wf = std::get<11>(*cashflow).getWealthFactor()->description();
        std::string yf = std::get<11>(*cashflow).getYearFraction()->description();
        tuple[14] = wf + yf;
        return tuple;
    } else {
        auto iborMCCashflow = std::dynamic_pointer_cast<qf::IborMultiCurrencyCashflow>(iborCashflow);
        auto tuple = py::tuple(21);
        auto cashflow = iborMCCashflow->wrap();
        tuple[0] = std::get<0>(*cashflow).description(false); // startDate
        tuple[1] = std::get<1>(*cashflow).description(false); // endDate
        tuple[2] = std::get<2>(*cashflow).description(false); // fixingDate
        tuple[3] = std::get<3>(*cashflow).description(false); // settlementDate
        tuple[4] = std::get<4>(*cashflow); // nominal
        tuple[5] = std::get<5>(*cashflow); // amortization
        tuple[6] = std::get<6>(*cashflow); // interest
        tuple[7] = std::get<7>(*cashflow); // doesAmortize
        tuple[8] = std::get<8>(*cashflow); // settlementCurrencyCashflow
        tuple[9] = std::get<9>(*cashflow)->getIsoCode(); // nominalCurrency
        tuple[10] = std::get<10>(*cashflow); // interestRateIndexCode
        tuple[11] = std::get<12>(*cashflow); // spread
        tuple[12] = std::get<13>(*cashflow); // gearing
        tuple[13] = std::get<14>(*cashflow); // rateValue
        std::string wf = std::get<11>(*cashflow).getWealthFactor()->description();
        std::string yf = std::get<11>(*cashflow).getYearFraction()->description();
        tuple[14] = wf + yf; // typeRate
        tuple[15] = std::get<15>(*cashflow).description(false); // fxRateIndexDate
        tuple[16] = std::get<16>(*cashflow)->getIsoCode(); // settlementCurrency
        tuple[17] = std::get<17>(*cashflow)->getCode(); // fxRateIndexCode
        tuple[18] = std::get<18>(*cashflow); // fxRateIndexValue
        tuple[19] = std::get<19>(*cashflow); // settlementCcyAmortization
        tuple[20] = std::get<20>(*cashflow); // settlementCcyInterest
        return tuple;

    }
}

py::tuple show(const std::shared_ptr<qf::IborCashflow2>& iborCashflow2) {
    auto tuple = py::tuple(15);
    auto cashflow = iborCashflow2->wrap();
    tuple[0] = std::get<0>(*cashflow).description(false);
    tuple[1] = std::get<1>(*cashflow).description(false);
    tuple[2] = std::get<2>(*cashflow).description(false);
    tuple[3] = std::get<3>(*cashflow).description(false);
    tuple[4] = std::get<4>(*cashflow);
    tuple[5] = std::get<5>(*cashflow);
    tuple[6] = std::get<6>(*cashflow);
    tuple[7] = std::get<7>(*cashflow);
    tuple[8] = std::get<8>(*cashflow);
    tuple[9] = std::get<9>(*cashflow)->getIsoCode();
    tuple[10] = std::get<10>(*cashflow);
    tuple[11] = std::get<14>(*cashflow);
    tuple[12] = std::get<12>(*cashflow);
    tuple[13] = std::get<13>(*cashflow);
    std::string wf = std::get<11>(*cashflow).getWealthFactor()->description();
    std::string yf = std::get<11>(*cashflow).getYearFraction()->description();
    tuple[14] = wf + yf;
    return tuple;
}

py::tuple show(const std::shared_ptr<qf::IcpClpCashflow>& icpClpCashflow) {
    if (icpClpCashflow->getType() == "IcpClpCashflow") {
        auto tuple = py::tuple(15);
        auto cashflow = icpClpCashflow->wrap();
        tuple[0] = std::get<0>(*cashflow).description(false);
        tuple[1] = std::get<1>(*cashflow).description(false);
        tuple[2] = std::get<2>(*cashflow).description(false);
        tuple[3] = std::get<3>(*cashflow);
        tuple[4] = std::get<4>(*cashflow);
        tuple[5] = std::get<5>(*cashflow);
        tuple[6] = icpClpCashflow->amount();
        tuple[7] = std::get<6>(*cashflow)->getIsoCode();
        tuple[8] = std::get<7>(*cashflow);
        tuple[9] = std::get<8>(*cashflow);
        tuple[10] = std::get<9>(*cashflow);
        tuple[11] = std::get<10>(*cashflow);
        tuple[12] = std::get<11>(*cashflow);
        tuple[13] = std::get<12>(*cashflow);
        tuple[14] = icpClpCashflow->getTypeOfRate();
        return tuple;
    } else {
        auto icpClfCashflow = std::dynamic_pointer_cast<qf::IcpClfCashflow>(icpClpCashflow);
        auto tuple = py::tuple(17);
        auto cashflow = icpClfCashflow->wrap();
        tuple[0] = std::get<0>(*cashflow).description(false);
        tuple[1] = std::get<1>(*cashflow).description(false);
        tuple[2] = std::get<2>(*cashflow).description(false);
        tuple[3] = std::get<3>(*cashflow);
        tuple[4] = std::get<4>(*cashflow);
        tuple[5] = std::get<5>(*cashflow);
        tuple[6] = icpClpCashflow->amount();
        tuple[7] = std::get<6>(*cashflow)->getIsoCode();
        tuple[8] = std::get<7>(*cashflow);
        tuple[9] = std::get<8>(*cashflow);
        tuple[10] = std::get<9>(*cashflow);
        tuple[11] = std::get<10>(*cashflow);
        tuple[12] = std::get<11>(*cashflow);
        tuple[13] = std::get<12>(*cashflow);
        tuple[14] = std::get<13>(*cashflow);
        tuple[15] = std::get<14>(*cashflow);
        tuple[16] = icpClpCashflow->getTypeOfRate();
        return tuple;
    }
}

py::tuple show(const std::shared_ptr<qf::IcpClpCashflow2>& icpClpCashflow2) {
    auto tuple = py::tuple(15);
    auto cashflow = icpClpCashflow2->wrap();
    tuple[0] = std::get<0>(*cashflow).description(false);
    tuple[1] = std::get<1>(*cashflow).description(false);
    tuple[2] = std::get<2>(*cashflow).description(false);
    tuple[3] = std::get<3>(*cashflow);
    tuple[4] = std::get<4>(*cashflow);
    tuple[5] = std::get<5>(*cashflow);
    tuple[6] = icpClpCashflow2->amount();
    tuple[7] = std::get<6>(*cashflow)->getIsoCode();
    tuple[8] = std::get<7>(*cashflow);
    tuple[9] = std::get<8>(*cashflow);
    tuple[10] = std::get<9>(*cashflow);
    tuple[11] = std::get<10>(*cashflow);
    tuple[12] = std::get<11>(*cashflow);
    tuple[13] = std::get<12>(*cashflow);
    tuple[14] = icpClpCashflow2->getTypeOfRate();
    return tuple;
}

py::tuple show(const std::shared_ptr<qf::CompoundedOvernightRateCashflow>& compoundedOvernightRateCashflow) {
    auto tuple = py::tuple(14);
    auto cashflow = compoundedOvernightRateCashflow->wrap();
    tuple[0] = std::get<0>(*cashflow).description(false);
    tuple[1] = std::get<1>(*cashflow).description(false);
    tuple[2] = std::get<2>(*cashflow).description(false);
    tuple[3] = std::get<3>(*cashflow);
    tuple[4] = std::get<4>(*cashflow);
    tuple[5] = std::get<5>(*cashflow);
    tuple[6] = std::get<6>(*cashflow);
    tuple[7] = std::get<7>(*cashflow);
    tuple[8] = std::get<8>(*cashflow)->getIsoCode();
    tuple[9] = std::get<9>(*cashflow);
    tuple[10] = std::get<13>(*cashflow);
    tuple[11] = std::get<11>(*cashflow);
    tuple[12] = std::get<12>(*cashflow);
    tuple[13] = compoundedOvernightRateCashflow->getTypeOfRate();
    return tuple;
}


#endif //QCFINANCIALPYBIND11HELPERS_H
