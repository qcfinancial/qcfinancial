//
// Created by ADiazV on 24-12-2021.
//

#include "TestHelpers.h"
#include "LegFactory.h"

std::shared_ptr<QCUSD> TestHelpers::getUSD() {
    static shared_ptr<QCUSD> usd = std::make_shared<QCUSD>(QCUSD());
    return usd;
}

std::shared_ptr<QCAct360> TestHelpers::getAct360() {
    static shared_ptr<QCAct360> result = std::make_shared<QCAct360>(QCAct360());
    return result;
}

std::shared_ptr<QCLinearWf> TestHelpers::getLin() {
    static shared_ptr<QCLinearWf> result = std::make_shared<QCLinearWf>(QCLinearWf());
    return result;
}

QCInterestRate TestHelpers::getLinAct360() {
    static QCInterestRate result = QCInterestRate(0.0, getAct360(), getLin());
    return result;
}

std::shared_ptr<QCode::Financial::InterestRateIndex> TestHelpers::getSofr() {
    std::string code = "SOFRRATE";
    auto startLag = QCode::Financial::Tenor("0D");
    auto tenor = QCode::Financial::Tenor("1D");
    auto ny = QCBusinessCalendar(QCDate(1, 1, 2021), 10);
    auto sofr = QCode::Financial::InterestRateIndex(
            code,
            TestHelpers::getLinAct360(),
            startLag,
            tenor,
            ny,
            ny,
            TestHelpers::getUSD());
    static std::shared_ptr<QCode::Financial::InterestRateIndex> sofrPtr =
            std::make_shared<QCode::Financial::InterestRateIndex>(sofr);
    return sofrPtr;
}

QCode::Financial::ZeroCouponCurve TestHelpers::getTestZcc() {
    std::vector<long> plazos{1, 30, 90, 180, 365, 730};
    std::vector<double> tasas{.001, .002, .003, .004, .005, .006};
    auto curva = std::make_shared<QCCurve<long>>(QCCurve<long>(plazos, tasas));
    auto interp = std::make_shared<QCLinearInterpolator>(QCLinearInterpolator(curva));
    auto rate = TestHelpers::getLinAct360();
    return {interp, rate};
}

std::shared_ptr<QCode::Financial::InterestRateCurve> TestHelpers::getTestZccPtr() {
    auto curve = getTestZcc();
    return std::make_shared<QCode::Financial::ZeroCouponCurve>(curve);
}

QCode::Financial::CompoundedOvernightRateCashflow TestHelpers::getCashflow(double spread) {
    auto sofr = TestHelpers::getSofr();
    auto startDate = QCDate(27, 12, 2021);
    auto endDate = QCDate(7, 1, 2022);
    auto settlementDate = QCDate(7, 1, 2022);
    std::vector<QCDate> fixingDates{
            QCDate(27, 12, 2021),
            QCDate(28, 12, 2021),
            QCDate(29, 12, 2021),
            QCDate(30, 12, 2021),
            QCDate(31, 12, 2021),
            QCDate(3, 1, 2022),
            QCDate(4, 1, 2022),
            QCDate(5, 1, 2022),
            QCDate(6, 1, 2022),
    };
    double notional = 10000000;
    double amortization = 1000000;
    bool doesAmortize = false;
    auto ccy = TestHelpers::getUSD();
    double gearing = 1.0;
    bool isAct360 = true;
    unsigned int eqRateDecimalPlaces = 8;
    unsigned int lookback = 0;
    unsigned int lockout = 0;

    return {sofr,
            startDate,
            endDate,
            settlementDate,
            fixingDates,
            notional,
            amortization,
            doesAmortize,
            ccy,
            spread,
            gearing,
            isAct360,
            eqRateDecimalPlaces,
            lookback,
            lockout};
}

QCode::Financial::CompoundedOvernightRateCashflow TestHelpers::getCashflowWithAmort() {
    auto sofr = TestHelpers::getSofr();
    auto startDate = QCDate(27, 12, 2021);
    auto endDate = QCDate(7, 1, 2022);
    auto settlementDate = QCDate(7, 1, 2022);
    std::vector<QCDate> fixingDates{
            QCDate(27, 12, 2021),
            QCDate(28, 12, 2021),
            QCDate(29, 12, 2021),
            QCDate(30, 12, 2021),
            QCDate(31, 12, 2021),
            QCDate(3, 1, 2022),
            QCDate(4, 1, 2022),
            QCDate(5, 1, 2022),
            QCDate(6, 1, 2022),
    };
    double notional = 10000000;
    double amortization = notional;
    bool doesAmortize = true;
    auto ccy = TestHelpers::getUSD();
    double spread = 0.0;
    double gearing = 1.0;
    bool isAct360 = true;
    unsigned int eqRateDecimalPlaces = 8;
    unsigned int lookback = 0;
    unsigned int lockout = 0;

    return {sofr,
            startDate,
            endDate,
            settlementDate,
            fixingDates,
            notional,
            amortization,
            doesAmortize,
            ccy,
            spread,
            gearing,
            isAct360,
            eqRateDecimalPlaces,
            lookback,
            lockout};
}

QCode::Financial::Leg
TestHelpers::getConrLeg(
        const QCDate &startDate,
        const QCDate &endDate,
        const QCode::Financial::Tenor &periodicity,
        double spread,
        const QCBusinessCalendar &calendar) {
    auto leg = QCode::Financial::LegFactory::buildBulletCompoundedOvernightLeg(
            QCode::Financial::RecPay::Receive,
            QCDate(3, 1, 2022),
            endDate,
            QCDate::QCBusDayAdjRules::qcFollow,
            periodicity,
            QCInterestRateLeg::QCStubPeriod::qcShortFront,
            calendar,
            0,
            calendar,
            TestHelpers::getSofr(),
            10000000,
            true,
            std::make_shared<QCCurrency>(QCUSD()),
            spread,
            1.0,
            true,
            6,
            0,
            0);
    return leg;
}


