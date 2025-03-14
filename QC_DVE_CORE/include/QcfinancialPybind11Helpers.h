//
// Created by Alvaro Diaz on 24-06-23.
//

#ifndef QCFINANCIALPYBIND11HELPERS_H
#define QCFINANCIALPYBIND11HELPERS_H

#include <pybind11/pybind11.h>

#include <cashflows/Cashflow.h>
#include <cashflows/SimpleMultiCurrencyCashflow.h>
#include <cashflows/LinearInterestRateCashflow.h>
#include <cashflows/OvernightIndexCashflow.h>
#include <curves/QCInterpolator.h>
#include <asset_classes//InterestRateCurve.h>
#include <asset_classes/QCInterestRate.h>
#include <cashflows/CompoundedOvernightRateCashflow2.h>
#include <cashflows/CompoundedOvernightRateMultiCurrencyCashflow2.h>

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

    bool isExpired(const QCDate& refDate) override {
        PYBIND11_OVERRIDE_PURE(
                bool,           /* Return type */
                qf::Cashflow,     /* Parent class */
                isExpired);          /* Name of function in C++ (must match Python name) */
    }

    std::string getType() const override {
        PYBIND11_OVERRIDE_PURE(
                std::string,      /* Return type */
                qf::Cashflow,     /* Parent class */
                getType);         /* Name of function in C++ (must match Python name) */
    }

    double getPresentValue() const override {
        PYBIND11_OVERRIDE_PURE(
                double,           /* Return type */
                qf::Cashflow,     /* Parent class */
                getPresentValue);          /* Name of function in C++ (must match Python name) */
    }

    double getDiscountFactor() const override {
        PYBIND11_OVERRIDE_PURE(
                double,           /* Return type */
                qf::Cashflow,     /* Parent class */
                getDiscountFactor);          /* Name of function in C++ (must match Python name) */
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

    QCDate endDate() override {
        PYBIND11_OVERRIDE_PURE(
                QCDate,
                qf::Cashflow,
                endDate);
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


    const std::vector<QCDate>& getFixingDates() const override {
        PYBIND11_OVERRIDE_PURE(
                const std::vector<QCDate> ,
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

class PyQCInterpolator : public QCInterpolator {
public:
    /* Inherit the constructors */
    using QCInterpolator::QCInterpolator;

    /* Trampoline (need one for each virtual function) */
    double interpolateAt(long value) override {
        PYBIND11_OVERRIDE_PURE(
                double,           /* Return type */
                QCInterpolator,   /* Parent class */
                interpolateAt);   /* Name of function in C++ (must match Python name) */
    }

    double derivativeAt(long value) override {
        PYBIND11_OVERRIDE_PURE(
                double,           /* Return type */
                QCInterpolator,   /* Parent class */
                derivativeAt);    /* Name of function in C++ (must match Python name) */
    }

    double secondDerivativeAt(long value) override {
        PYBIND11_OVERRIDE_PURE(
                double,                 /* Return type */
                QCInterpolator,         /* Parent class */
                secondDerivativeAt);    /* Name of function in C++ (must match Python name) */
    }
};

class PyInterestRateCurve : public qf::InterestRateCurve {
public:
    /* Inherit the constructors */
    using qf::InterestRateCurve::InterestRateCurve;

    /* Trampoline (need one for each virtual function) */
    double getRateAt(long d) override {
        PYBIND11_OVERRIDE_PURE(
                double,                     /* Return type */
                qf::InterestRateCurve,      /* Parent class */
                getRateAt,                  /* Name of function in C++ (must match Python name) */
                d);
    }

    QCInterestRate getQCInterestRateAt(long d) override {
        PYBIND11_OVERRIDE_PURE(
                QCInterestRate,             /* Return type */
                qf::InterestRateCurve,      /* Parent class */
                getQCInterestRateAt,        /* Name of function in C++ (must match Python name) */
                d);
    }

    double getDiscountFactorAt(long d) override {
        PYBIND11_OVERRIDE_PURE(
                double,                     /* Return type */
                qf::InterestRateCurve,      /* Parent class */
                getDiscountFactorAt,        /* Name of function in C++ (must match Python name) */
                d);
    }

    double getForwardRateWithRate(QCInterestRate& intRate, long d1, long d2) override {
        PYBIND11_OVERRIDE_PURE(
                double,                     /* Return type */
                qf::InterestRateCurve,      /* Parent class */
                getForwardRateWithRate,     /* Name of function in C++ (must match Python name) */
                intRate,
                d1,
                d2);
    }

    double getForwardRate(long d1, long d2) override {
        PYBIND11_OVERRIDE_PURE(
                double,                     /* Return type */
                qf::InterestRateCurve,      /* Parent class */
                getForwardRate,             /* Name of function in C++ (must match Python name) */
                d1,
                d2);
    }

    double getForwardWf(long d1, long d2) override {
        PYBIND11_OVERRIDE_PURE(
                double,                     /* Return type */
                qf::InterestRateCurve,      /* Parent class */
                getForwardWf,               /* Name of function in C++ (must match Python name) */
                d1,
                d2);
    }

    double dfDerivativeAt(unsigned int index) override {
        PYBIND11_OVERRIDE_PURE(
                double,                     /* Return type */
                qf::InterestRateCurve,      /* Parent class */
                dfDerivativeAt,             /* Name of function in C++ (must match Python name) */
                index);
    }

    double wfDerivativeAt(unsigned int index) override {
        PYBIND11_OVERRIDE_PURE(
                double,                     /* Return type */
                qf::InterestRateCurve,      /* Parent class */
                wfDerivativeAt,             /* Name of function in C++ (must match Python name) */
                index);
    }

    double fwdWfDerivativeAt(unsigned int index) override {
        PYBIND11_OVERRIDE_PURE(
                double,                     /* Return type */
                qf::InterestRateCurve,      /* Parent class */
                fwdWfDerivativeAt,          /* Name of function in C++ (must match Python name) */
                index);
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
        auto tuple = py::tuple(22);
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
        tuple[21] = tuple[19] + tuple[20];
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
        auto tuple = py::tuple(18);
        auto cashflow = icpClfCashflow->wrap();
        tuple[0] = std::get<0>(*cashflow).description(false);
        tuple[1] = std::get<1>(*cashflow).description(false);
        tuple[2] = std::get<2>(*cashflow).description(false);
        tuple[3] = std::get<3>(*cashflow);
        tuple[4] = std::get<4>(*cashflow);
        tuple[5] = std::get<5>(*cashflow);
        tuple[6] = std::get<4>(*cashflow) + std::get<12>(*cashflow); // icpClpCashflow->amount();
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
        tuple[17] = std::get<15>(*cashflow);
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

py::tuple show(const std::shared_ptr<qf::CompoundedOvernightRateCashflow2>& compoundedOvernightRateCashflow2) {
    if (compoundedOvernightRateCashflow2->getType() == "CompoundedOvernightRateCashflow2") {
        auto tuple = py::tuple(14);
        auto cashflow = compoundedOvernightRateCashflow2->wrap();
        tuple[0] = std::get<0>(*cashflow);
        tuple[1] = std::get<1>(*cashflow);
        tuple[2] = std::get<2>(*cashflow);
        tuple[3] = std::get<3>(*cashflow);
        tuple[4] = std::get<4>(*cashflow);
        tuple[5] = std::get<5>(*cashflow);
        tuple[6] = std::get<6>(*cashflow);
        tuple[7] = std::get<7>(*cashflow);
        tuple[8] = std::get<8>(*cashflow);
        tuple[9] = std::get<9>(*cashflow);
        tuple[10] = std::get<10>(*cashflow);
        tuple[11] = std::get<13>(*cashflow);
        tuple[12] = std::get<11>(*cashflow);
        tuple[13] = std::get<12>(*cashflow);
        return tuple;
    } else {
        auto tuple = py::tuple(21);
        auto mccyCashflow = std::dynamic_pointer_cast<qf::CompoundedOvernightRateMultiCurrencyCashflow2>(
                compoundedOvernightRateCashflow2);
        auto cashflow = mccyCashflow->mccyWrap();
        tuple[0] = std::get<0>(*cashflow);
        tuple[1] = std::get<1>(*cashflow);
        tuple[2] = std::get<2>(*cashflow);
        tuple[3] = std::get<3>(*cashflow);
        tuple[4] = std::get<4>(*cashflow);
        tuple[5] = std::get<5>(*cashflow);
        tuple[6] = std::get<6>(*cashflow);
        tuple[7] = std::get<7>(*cashflow);
        tuple[8] = std::get<8>(*cashflow);
        tuple[9] = std::get<9>(*cashflow);
        tuple[10] = std::get<10>(*cashflow);
        tuple[11] = std::get<11>(*cashflow);
        tuple[12] = std::get<12>(*cashflow);
        tuple[13] = std::get<13>(*cashflow);
        tuple[14] = std::get<14>(*cashflow);
        tuple[15] = std::get<15>(*cashflow);
        tuple[16] = std::get<16>(*cashflow);
        tuple[17] = std::get<17>(*cashflow);
        tuple[18] = std::get<18>(*cashflow);
        tuple[19] = std::get<19>(*cashflow);
        tuple[20] = std::get<20>(*cashflow);
        return tuple;
    };
}

py::tuple show(const std::shared_ptr<qf::OvernightIndexCashflow>& overnightIndexCashflow) {
    if (overnightIndexCashflow->getType() == "OvernightIndexCashflow") {
        auto tuple = py::tuple(18);
        auto cashflow = overnightIndexCashflow->wrap();
        tuple[0] = std::get<0>(*cashflow);
        tuple[1] = std::get<1>(*cashflow);
        tuple[2] = std::get<2>(*cashflow);
        tuple[3] = std::get<3>(*cashflow);
        tuple[4] = std::get<4>(*cashflow);
        tuple[5] = std::get<5>(*cashflow);
        tuple[6] = std::get<6>(*cashflow);
        tuple[7] = std::get<7>(*cashflow);
        tuple[8] = std::get<8>(*cashflow);
        tuple[9] = std::get<9>(*cashflow);
        tuple[10] = std::get<10>(*cashflow);
        tuple[11] = std::get<11>(*cashflow);
        tuple[12] = std::get<12>(*cashflow);
        tuple[13] = std::get<13>(*cashflow);
        tuple[14] = std::get<14>(*cashflow);
        tuple[15] = std::get<15>(*cashflow);
        tuple[16] = std::get<16>(*cashflow);
        tuple[17] = std::get<17>(*cashflow);
        return tuple;
    } else {
        auto overnightIndexMccyCashflow = std::dynamic_pointer_cast<qf::OvernightIndexMultiCurrencyCashflow>(overnightIndexCashflow);
        auto tuple = py::tuple(25);
        auto cashflow = overnightIndexMccyCashflow->mccyWrap();
        tuple[0] = std::get<0>(*cashflow);
        tuple[1] = std::get<1>(*cashflow);
        tuple[2] = std::get<2>(*cashflow);
        tuple[3] = std::get<3>(*cashflow);
        tuple[4] = std::get<4>(*cashflow);
        tuple[5] = std::get<5>(*cashflow);
        tuple[6] = std::get<6>(*cashflow);
        tuple[7] = std::get<7>(*cashflow);
        tuple[8] = std::get<8>(*cashflow);
        tuple[9] = std::get<9>(*cashflow);
        tuple[10] = std::get<10>(*cashflow);
        tuple[11] = std::get<11>(*cashflow);
        tuple[12] = std::get<12>(*cashflow);
        tuple[13] = std::get<13>(*cashflow);
        tuple[14] = std::get<14>(*cashflow);
        tuple[15] = std::get<15>(*cashflow);
        tuple[16] = std::get<16>(*cashflow);
        tuple[17] = std::get<17>(*cashflow);
        tuple[18] = std::get<18>(*cashflow);
        tuple[19] = std::get<19>(*cashflow);
        tuple[20] = std::get<20>(*cashflow);
        tuple[21] = std::get<21>(*cashflow);
        tuple[22] = std::get<22>(*cashflow);
        tuple[23] = std::get<23>(*cashflow);
        tuple[24] = std::get<24>(*cashflow);
        return tuple;
    }

}


// Other Functions
inline py::tuple getColumnNames(const std::string &cashflowType, const std::string &cashflowSubtype = "") {
    if (cashflowType == "FixedRateCashflow" || cashflowType == "FixedRateCashflow2") {
        auto result = py::tuple(11);
        result[0] = "fecha_inicial";
        result[1] = "fecha_final";
        result[2] = "fecha_pago";
        result[3] = "nocional";
        result[4] = "amortizacion";
        result[5] = "interes";
        result[6] = "amort_es_flujo";
        result[7] = "flujo";
        result[8] = "moneda_nocional";
        result[9] = "valor_tasa";
        result[10] = "tipo_tasa";
        return result;
    } else if (cashflowType == "FixedRateMultiCurrencyCashflow" ) {
        auto result = py::tuple(17);
        result[0] = "fecha_inicial";
        result[1] = "fecha_final";
        result[2] = "fecha_pago";
        result[3] = "nocional";
        result[4] = "amortizacion";
        result[5] = "interes";
        result[6] = "amort_es_flujo";
        result[7] = "flujo";
        result[8] = "moneda_nocional";
        result[9] = "valor_tasa";
        result[10] = "tipo_tasa";
        result[11] = "fecha_fixing_fx";
        result[12] = "moneda_pago";
        result[13] = "indice_fx";
        result[14] = "valor_indice_fx";
        result[15] = "amortizacion_moneda_pago";
        result[16] = "interes_moneda_pago";
        return result;
    } else if (cashflowType == "OvernightIndexCashflow" ) {
        auto result = py::tuple(18);
        result[0] = "fecha_inicial";
        result[1] = "fecha_final";
        result[2] = "fecha_inicial_indice";
        result[3] = "fecha_final_indice";
        result[4] = "fecha_pago";
        result[5] = "nocional";
        result[6] = "amortizacion";
        result[7] = "amort_es_flujo";
        result[8] = "moneda_nocional";
        result[9] = "nombre_indice";
        result[10] = "valor_indice_inicial";
        result[11] = "valor_indice_final";
        result[12] = "valor_tasa";
        result[13] = "tipo_tasa";
        result[14] = "interes";
        result[15] = "flujo";
        result[16] = "spread";
        result[17] = "gearing";
        return result;
    } else if (cashflowType == "OvernightIndexMultiCurrencyCashflow" ) {
        auto result = py::tuple(25);
        result[0] = "fecha_inicial";
        result[1] = "fecha_final";
        result[2] = "fecha_inicial_indice";
        result[3] = "fecha_final_indice";
        result[4] = "fecha_pago";
        result[5] = "nocional";
        result[6] = "amortizacion";
        result[7] = "amort_es_flujo";
        result[8] = "moneda_nocional";
        result[9] = "nombre_indice";
        result[10] = "valor_indice_inicial";
        result[11] = "valor_indice_final";
        result[12] = "valor_tasa";
        result[13] = "tipo_tasa";
        result[14] = "interes";
        result[15] = "flujo";
        result[16] = "spread";
        result[17] = "gearing";
        result[18] = "moneda_pago";
        result[19] = "indice_fx";
        result[20] = "fecha_fixing_fx";
        result[21] = "valor_indice_fx";
        result[22] = "interes_moneda_pago";
        result[23] = "amortizacion_moneda_pago";
        result[24] = "flujo_moneda_pago";
        return result;
    } else if (cashflowType == "IborCashflow" || cashflowType == "IborCashflow2") {
        auto result = py::tuple(15);
        result[0] = "fecha_inicial";
        result[1] = "fecha_final";
        result[2] = "fecha_fixing";
        result[3] = "fecha_pago";
        result[4] = "nocional";
        result[5] = "amortizacion";
        result[6] = "interes";
        result[7] = "amort_es_flujo";
        result[8] = "flujo";
        result[9] = "moneda_nocional";
        result[10] = "codigo_indice_tasa";
        result[11] = "valor_tasa";
        result[12] = "spread";
        result[13] = "gearing";
        result[14] = "tipo_tasa";
        return result;
    } else if (cashflowType == "IborMultiCurrencyCashflow") {
        auto result = py::tuple(22);
        result[0] = "fecha_inicial";
        result[1] = "fecha_final";
        result[2] = "fecha_fixing";
        result[3] = "fecha_pago";
        result[4] = "nocional";
        result[5] = "amortizacion";
        result[6] = "interes";
        result[7] = "amort_es_flujo";
        result[8] = "flujo";
        result[9] = "moneda_nocional";
        result[10] = "codigo_indice_tasa";
        result[11] = "spread";
        result[12] = "gearing";
        result[13] = "valor_tasa";
        result[14] = "tipo_tasa";
        result[15] = "fecha_fixing_fx";
        result[16] = "moneda_pago";
        result[17] = "codigo_indice_fx";
        result[18] = "valor_indice_fx";
        result[19] = "amortizacion_moneda_pago";
        result[20] = "interes_moneda_pago";
        result[21] = "flujo_moneda_pago";
        return result;
    } else if (cashflowType == "IcpClpCashflow" || cashflowType == "IcpClpCashflow2") {
        auto result = py::tuple(15);
        result[0] = "fecha_inicial";
        result[1] = "fecha_final";
        result[2] = "fecha_pago";
        result[3] = "nocional";
        result[4] = "amortizacion";
        result[5] = "amort_es_flujo";
        result[6] = "flujo";
        result[7] = "moneda";
        result[8] = "icp_inicial";
        result[9] = "icp_final";
        result[10] = "valor_tasa";
        result[11] = "interes";
        result[12] = "spread";
        result[13] = "gearing";
        result[14] = "tipo_tasa";
        return result;
    } else if (cashflowType == "QuantoLinearInterestRateCashflow") {
        if (cashflowSubtype == "FixedRateCashflow2") {
            auto result = py::tuple(16);
            result[0] = "fecha_inicial";
            result[1] = "fecha_final";
            result[2] = "fecha_pago";
            result[3] = "nocional";
            result[4] = "amortizacion";
            result[5] = "interes";
            result[6] = "amort_es_flujo";
            result[7] = "flujo";
            result[8] = "moneda";
            result[9] = "valor_tasa";
            result[10] = "tipo_tasa";
            result[11] = "valor_indice_fx";
            result[12] = "nominal_moneda_pago";
            result[13] = "amort_moneda_pago";
            result[14] = "interes_moneda_pago";
            result[15] = "flujo_moneda_pago";
            return result;
        } else if (cashflowSubtype == "IborCashflow2") {
            auto result = py::tuple(20);
            result[0] = "fecha_inicial";
            result[1] = "fecha_final";
            result[2] = "fecha_fixing";
            result[3] = "fecha_pago";
            result[4] = "nocional";
            result[5] = "amortizacion";
            result[6] = "interes";
            result[7] = "amort_es_flujo";
            result[8] = "flujo";
            result[9] = "moneda";
            result[10] = "codigo_indice_tasa";
            result[11] = "valor_tasa";
            result[12] = "spread";
            result[13] = "gearing";
            result[14] = "tipo_tasa";
            result[15] = "valor_indice_fx";
            result[16] = "nominal_moneda_pago";
            result[17] = "amort_moneda_pago";
            result[18] = "interes_moneda_pago";
            result[19] = "flujo_moneda_pago";
            return result;
        } else {
            throw std::invalid_argument("Cashflow subtype " + cashflowSubtype + " is not recognized.");
        }
    } else if (cashflowType == "SimpleCashflow") {
        auto result = py::tuple(3);
        result[0] = "fecha_pago";
        result[1] = "monto";
        result[2] = "moneda";
        return result;
    } else if (cashflowType == "SimpleMultiCurrencyCashflow") {
        auto result = py::tuple(8);
        result[0] = "fecha_pago";
        result[1] = "monto_nominal";
        result[2] = "moneda_nominal";
        result[3] = "fecha_fixing_fx";
        result[4] = "moneda_pago";
        result[5] = "codigo_indice_fx";
        result[6] = "valor_indice_fx";
        result[7] = "monto_moneda_pago";
        return result;
    } else if (cashflowType == "IcpClfCashflow") {
        auto result = py::tuple(18);
        result[0] = "fecha_inicial";
        result[1] = "fecha_final";
        result[2] = "fecha_pago";
        result[3] = "nocional";
        result[4] = "amortizacion";
        result[5] = "amort_es_flujo";
        result[6] = "flujo";
        result[7] = "moneda_nocional";
        result[8] = "icp_inicial";
        result[9] = "icp_final";
        result[10] = "uf_inicial";
        result[11] = "uf_final";
        result[12] = "valor_tasa";
        result[13] = "interes";
        result[14] = "spread";
        result[15] = "gearing";
        result[16] = "tipo_tasa";
        result[17] = "flujo_en_clp";
        return result;
    } else if (cashflowType == "CompoundedOvernightRateCashflow" || cashflowType == "CompoundedOvernightRateCashflow2") {
        auto result = py::tuple(14);
        result[0] = "fecha_inicial";
        result[1] = "fecha_final";
        result[2] = "fecha_pago";
        result[3] = "nocional";
        result[4] = "amortizacion";
        result[5] = "interes";
        result[6] = "amort_es_flujo";
        result[7] = "flujo";
        result[8] = "moneda_nocional";
        result[9] = "codigo_indice_tasa";
        result[10] = "tipo_tasa";
        result[11] = "valor_tasa";
        result[12] = "spread";
        result[13] = "gearing";
        return result;
    } else if (cashflowType == "CompoundedOvernightRateMultiCurrencyCashflow2") {
        auto result = py::tuple(21);
        result[0] = "fecha_inicial";
        result[1] = "fecha_final";
        result[2] = "fecha_pago";
        result[3] = "nocional";
        result[4] = "amortizacion";
        result[5] = "interes";
        result[6] = "amort_es_flujo";
        result[7] = "flujo";
        result[8] = "moneda_nocional";
        result[9] = "codigo_indice_tasa";
        result[10] = "tipo_tasa";
        result[11] = "spread";
        result[12] = "gearing";
        result[13] = "valor_tasa";
        result[14] = "moneda_pago";
        result[15] = "fx_rate_index";
        result[16] = "fecha_fixing_fx";
        result[17] = "valor_indice_fx";
        result[18] = "interes_moneda_pago";
        result[19] = "amortizacion_moneda_pago";
        result[20] = "flujo_moneda_pago";
        return result;
    } else {
        throw std::invalid_argument("Cashflow type " + cashflowType + " is not recognized.");
    }
}



#endif //QCFINANCIALPYBIND11HELPERS_H
