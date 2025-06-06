//
// Created by Alvaro Díaz Valenzuela on 23-04-2023.
//

#include "cashflows/OvernightIndexCashflow.h"

#include <utility>

namespace QCode::Financial {
    OvernightIndexCashflow::OvernightIndexCashflow(
            const QCDate &accrualStartDate,
            const QCDate &accrualEndDate,
            const QCDate &indexStartDate,
            const QCDate &indexEndDate,
            const QCDate &settlementDate,
            std::shared_ptr<QCCurrency> notionalCurrency,
            double notional,
            double amortization,
            bool doesAmortize,
            double spread,
            double gearing,
            const QCInterestRate &rate,
            std::string indexName,
            unsigned int eqRateDecimalPlaces,
            DatesForEquivalentRate datesForEquivalentRate) :
            _startDate(accrualStartDate),
            _endDate(accrualEndDate),
            _indexStartDate(indexStartDate),
            _indexEndDate(indexEndDate),
            _settlementDate(settlementDate),
            _notionalCurrency(std::move(notionalCurrency)),
            _notional(notional),
            _amortization(amortization),
            _doesAmortize(doesAmortize),
            _spread(spread),
            _gearing(gearing),
            _rate(rate),
            _indexName(std::move(indexName)),
            _eqRateDecimalPlaces(eqRateDecimalPlaces),
            _datesForEquivalentRate(datesForEquivalentRate),
            _startDateIndexValue(DEFAULT_INDEX),
            _endDateIndexValue(DEFAULT_INDEX){}

    std::string OvernightIndexCashflow::getType() const {
        return "OvernightIndexCashflow";
    }

    shared_ptr<QCCurrency> OvernightIndexCashflow::ccy() {
        return _notionalCurrency;
    }

    QCDate OvernightIndexCashflow::date() {
        return _settlementDate;
    }

    QCDate OvernightIndexCashflow::endDate() {
        return _endDate;
    }

    double OvernightIndexCashflow::amount() {
        auto tempDecimalPlaces = _eqRateDecimalPlaces;
        _eqRateDecimalPlaces = 20;
        auto endDate = (_datesForEquivalentRate == DatesForEquivalentRate::qcAccrual) ? _endDate : _indexEndDate;
        auto rate = getEqRate(endDate, _endDateIndexValue);
        _rate.setValue(_spread + rate);
        auto interest = _notional * (_rate.wf(_startDate, _endDate) - 1.0);

        // Falta probar las derivadas
        if (_startDateIndexDerivatives.size() == _endDateIndexDerivatives.size())
        {
            _amountDerivatives.resize(_startDateIndexDerivatives.size());
            for (size_t i = 0; i < _startDateIndexDerivatives.size(); ++i)
            {
                _amountDerivatives.at(i) = _notional *
                        ((_endDateIndexDerivatives.at(i) * _startDateIndexValue -
                        _endDateIndexValue * _startDateIndexDerivatives.at(i)) /
                        pow(_startDateIndexValue, 2.0));
            }
        }
        else
        {
            std::vector<double> zeroDerivatives(_startDateIndexDerivatives.size(), 0.0);
            _amountDerivatives.resize(_startDateIndexDerivatives.size());
            _amountDerivatives = zeroDerivatives;
        }

        if (_doesAmortize)
        {
            _eqRateDecimalPlaces = tempDecimalPlaces;
            return _amortization + interest;
        }
        else
        {
            _eqRateDecimalPlaces = tempDecimalPlaces;
            return interest;
        }
    }


    void OvernightIndexCashflow::setEqRateDecimalPlaces(unsigned int decimalPlaces) {
        _eqRateDecimalPlaces = decimalPlaces;
    }

    unsigned int OvernightIndexCashflow::getEqRateDecimalPlaces() const {
        return _eqRateDecimalPlaces;
    }


    DatesForEquivalentRate OvernightIndexCashflow::getDatesForEqRate() const {
        return _datesForEquivalentRate;
    }


    void OvernightIndexCashflow::setDatesForEqRate(DatesForEquivalentRate datesForEqRate) {
        _datesForEquivalentRate = datesForEqRate;
    }


    double OvernightIndexCashflow::accruedInterest(QCDate &accrualDate, double indexValue) {
        return _calculateInterest(accrualDate, indexValue);
    }

    double OvernightIndexCashflow::getEqRate(QCDate &date, double indexValue) {
        // Calcula la tasa equivalente a una fecha posterior a la fecha de inicio
        // de devengo del cupón y para un valor del índice.

        double yf = 0.0;
        // Este if decide qué fecha inicial (la de devengo o la del índice)
        // usar para el cálculo de la fracción de año.
        if (_datesForEquivalentRate == DatesForEquivalentRate::qcAccrual) {
            yf = _rate.yf(_startDate, date);
        }
        else {
            yf = _rate.yf(_indexStartDate, date);
        }

        if (yf == 0.0) {
            return 0.0;
        }
        const double eqRate = (indexValue / _startDateIndexValue - 1) / yf;
        if (constexpr unsigned int LIMIT_EQ_RATE_DECIMAL_PLACES = 12;
            _eqRateDecimalPlaces > LIMIT_EQ_RATE_DECIMAL_PLACES) {
            return eqRate;
        }
        const auto factor = std::pow(10, _eqRateDecimalPlaces);
        return round(eqRate * factor) / factor;
    }


    void OvernightIndexCashflow::setStartDateIndex(double indexValue) {
        _startDateIndexValue = indexValue;
    };

    void OvernightIndexCashflow::setStartDateIndexDerivatives(std::vector<double> der)
    {
        _startDateIndexDerivatives.resize(der.size());
        for (size_t i = 0; i < der.size(); ++i)
        {
            _startDateIndexDerivatives.at(i) = der.at(i);
        }
    }

    std::vector<double> OvernightIndexCashflow::getStartDateIndexDerivatives() const
    {
        return _startDateIndexDerivatives;
    }


    double OvernightIndexCashflow::getStartDateIndex() const {
        return _startDateIndexValue;
    };


    double OvernightIndexCashflow::getEndDateIndex() const {
        return _endDateIndexValue;
    };


    void OvernightIndexCashflow::setEndDateIndex(double indexValue) {
        _endDateIndexValue = indexValue;
    };


    void OvernightIndexCashflow::setEndDateIndexDerivatives(std::vector<double> der)
    {
        _endDateIndexDerivatives.resize(der.size());
        for (size_t i = 0; i < der.size(); ++i)
        {
            _endDateIndexDerivatives.at(i) = der.at(i);
        }
    }

    std::vector<double> OvernightIndexCashflow::getEndDateIndexDerivatives() const
    {
        return _endDateIndexDerivatives;
    }

    std::vector<double> OvernightIndexCashflow::getAmountDerivatives() const
    {
        return _amountDerivatives;
    }


    void OvernightIndexCashflow::setNotional(double notional) {
        _notional = notional;
    };


    double OvernightIndexCashflow::getNotional() const {
        return _notional;
    };


    void OvernightIndexCashflow::setAmortization(double amortization) {
        _amortization = amortization;
    };


    double OvernightIndexCashflow::getAmortization() const {
        return _amortization;
    };


    shared_ptr<OvernightIndexCashflowWrapper> OvernightIndexCashflow::wrap() {
        // Accrual Start Date
        // Accrual End Date
        // Index Start Date
        // Index End Date
        // Settlement Date
        // Notional
        // Amortization
        // Amortization is cashflow
        // Notional Currency
        // Index name
        // Start date index value
        // End date index value
        // Equivalent Rate
        // Type of rate
        // Interest
        // Cashflow
        // Spread
        // Gearing

        auto flujo = settlementAmount();
        auto interes = flujo;
        if (_doesAmortize)
            interes -= _amortization;
        double rate = 0.0;

        // Este if decide qué fecha final (la de devengo o la del índice)
        // usar para el cálculo de la fracción de año.
        if (_datesForEquivalentRate == DatesForEquivalentRate::qcAccrual) {
            rate = getEqRate(_endDate, _endDateIndexValue);
        } else {
            rate = getEqRate(_indexEndDate, _endDateIndexValue);
        }
        OvernightIndexCashflowWrapper tup = std::make_tuple(
                _startDate.description(false),
                _endDate.description(false),
                _indexStartDate.description(false),
                _indexEndDate.description(false),
                _settlementDate.description(false),
                _notional,
                _amortization,
                _doesAmortize,
                _notionalCurrency->getIsoCode(),
                _indexName,
                _startDateIndexValue,
                _endDateIndexValue,
                rate,
                getTypeOfRate(),
                interes,
                flujo,
                _spread,
                _gearing);

        return std::make_shared<OvernightIndexCashflowWrapper>(tup);
    };


    Record OvernightIndexCashflow::record() {
        auto result = Record();
        auto interes = _calculateInterest(_endDate, _endDateIndexValue);
        auto flujo = interes;
        if (_doesAmortize)
            flujo += _amortization;
        double rate = 0.0;
        if (_datesForEquivalentRate == DatesForEquivalentRate::qcAccrual) {
            rate = getEqRate(_endDate, _endDateIndexValue);
        } else {
            rate = getEqRate(_indexEndDate, _endDateIndexValue);
        }
        result["type_of_cashflow"] = "overnight_index";
        result["start_date"] = _startDate.description(false);
        result["end_date"] = _endDate.description(false);
        result["index_start_date"] = _indexStartDate.description(false);
        result["index_end_date"] = _indexEndDate.description(false);
        result["settlement_date"] = _settlementDate.description(false);
        result["notional"] = _notional;
        result["amortization"] = _amortization;
        result["interest"] = interes;
        result["amort_is_cashflow"] = _doesAmortize;
        result["cashflow"] = flujo;
        result["notional_currency"] = _notionalCurrency->getIsoCode();
        result["interest_rate_index"] = _indexName;
        result["start_date_index"] = _startDateIndexValue;
        result["end_date_index"] = _endDateIndexValue;
        result["rate_value"] = rate;
        result["spread"] = _spread;
        result["gearing"] = _gearing;
        result["type_of_rate"] = getTypeOfRate();

        return result;
    }


    double OvernightIndexCashflow::getRateValue() {
        if (_datesForEquivalentRate == DatesForEquivalentRate::qcAccrual) {
            return getEqRate(_endDate, _endDateIndexValue);
        } else {
            return getEqRate(_indexEndDate, _endDateIndexValue);
        }
    };


    std::string OvernightIndexCashflow::getTypeOfRate() {
        return _rate.getWealthFactor()->description() + _rate.getYearFraction()->description();
    };


    QCDate OvernightIndexCashflow::getStartDate() const {
        return _startDate;
    };


    QCDate OvernightIndexCashflow::getEndDate() const {
        return _endDate;
    };


    QCDate OvernightIndexCashflow::getIndexStartDate() const {
        return _indexStartDate;
    };


    QCDate OvernightIndexCashflow::getIndexEndDate() const {
        return _indexEndDate;
    };

    double OvernightIndexCashflow::_calculateInterest(QCDate &date, double indexValue) {
        double eqRate = getEqRate(date, indexValue);
        _rate.setValue(eqRate * _gearing + _spread);
        return _notional * (_rate.wf(_startDate, date) - 1);
    }

    QCDate OvernightIndexCashflow::getSettlementDate() const {
        return _settlementDate;
    }

    double OvernightIndexCashflow::accruedInterest(const QCDate &fecha, const TimeSeries &fixings) {
        auto indexValue = 0.0;
        try {
            indexValue = fixings.at(fecha);
        }
        catch (const exception &e) {
            throw invalid_argument("Index value not found for date " +
                                   fecha.description(false) + ".");
        }
        auto fechaOk = fecha;
        return _calculateInterest(fechaOk, indexValue);
    }

    std::string OvernightIndexCashflow::getIndexCode() const {
        return _indexName;
    }

    double OvernightIndexCashflow::settlementAmount() {
        auto interest = _calculateInterest(_endDate, _endDateIndexValue);
        double settAmount = 0.0;
        if (_doesAmortize) {
            settAmount = _amortization + interest;
        } else {
            settAmount = interest;
        }
        // }
        return _notionalCurrency->amount(settAmount);
    }

    shared_ptr<QCCurrency> OvernightIndexCashflow::settlementCurrency() {
        return _notionalCurrency;
    }

    OvernightIndexCashflow::~OvernightIndexCashflow() = default;

} // QCode


