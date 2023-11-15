//
// Created by Soporte on 23-04-2023.
//

#include "cashflows/OvernightIndexCashflow.h"

#include <utility>

namespace QCode {
    namespace Financial
    {
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
                std::string  indexName,
                unsigned int eqRateDecimalPlaces) :
                _accrualStartDate(accrualStartDate),
                _accrualEndDate(accrualEndDate),
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
                _startDateIndex(DEFAULT_INDEX),
                _endDateIndex(DEFAULT_INDEX)
        {}

        std::string OvernightIndexCashflow::getType() const {
            return "OvernightIndexCashflow";
        }

        shared_ptr<QCCurrency> OvernightIndexCashflow::ccy() {
            return _notionalCurrency;
        }

        QCDate OvernightIndexCashflow::date() {
            return _settlementDate;
        }


        double OvernightIndexCashflow::amount() {
            auto interest = _calculateInterest(_accrualEndDate, _endDateIndex);
            if (_doesAmortize)
            {
                return _amortization + interest;
            }
            else
            {
                return interest;
            }
        }

        void OvernightIndexCashflow::setEqRateDecimalPlaces(unsigned int decimalPlaces) {
            _eqRateDecimalPlaces = decimalPlaces;
        }


        double OvernightIndexCashflow::accruedInterest(QCDate& accrualDate, double indexValue) {
            return _calculateInterest(accrualDate, indexValue);
        }

        double OvernightIndexCashflow::getEqRate(QCDate& date, double indexValue) {
            unsigned int LIMIT_EQ_RATE_DECIMAL_PLACES = 12;
            double yf = _rate.yf(_accrualStartDate, date);
            if (yf == 0.0)
            {
                return 0.0;
            }
            double eqRate = (indexValue / _startDateIndex - 1) / yf;
            if (_eqRateDecimalPlaces > LIMIT_EQ_RATE_DECIMAL_PLACES)
            {
                return eqRate;
            }
            else
            {
                long double factor = std::pow(10, _eqRateDecimalPlaces);
                return round(eqRate * factor) / factor;
            }
        }

        void OvernightIndexCashflow::setStartDateIndex(double indexValue) {
            _startDateIndex = indexValue;
        };


        double OvernightIndexCashflow::getStartDateIndex() const {
            return _startDateIndex;
        };


        double OvernightIndexCashflow::getEndDateIndex() const {
            return _endDateIndex;
        };


        void OvernightIndexCashflow::setEndDateIndex(double indexValue) {
            _endDateIndex = indexValue;
        };


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

            auto interes = _calculateInterest(_accrualEndDate, _endDateIndex);
            auto flujo = interes;
            if (_doesAmortize)
                flujo += _amortization;
            OvernightIndexCashflowWrapper tup = std::make_tuple(
                    _accrualStartDate.description(false),
                    _accrualEndDate.description(false),
                    _indexStartDate.description(false),
                    _indexEndDate.description(false),
                    _settlementDate.description(false),
                    _notional,
                    _amortization,
                    _doesAmortize,
                    _notionalCurrency->getIsoCode(),
                    _indexName,
                    _startDateIndex,
                    _endDateIndex,
                    getEqRate(_accrualEndDate, _endDateIndex),
                    getTypeOfRate(),
                    interes,
                    flujo,
                    _spread,
                    _gearing);

            return std::make_shared<OvernightIndexCashflowWrapper>(tup);
        };


        double OvernightIndexCashflow::getRateValue() {
            return getEqRate(_accrualEndDate, _endDateIndex);
        };


        std::string OvernightIndexCashflow::getTypeOfRate() {
            return _rate.getWealthFactor()->description() + _rate.getYearFraction()->description();
        };


        QCDate OvernightIndexCashflow::getAccrualStartDate() const {
            return _accrualStartDate;
        };


        QCDate OvernightIndexCashflow::getAccrualEndDate() const {
            return _accrualEndDate;
        };


        QCDate OvernightIndexCashflow::getIndexStartDate() const {
            return _indexStartDate;
        };


        QCDate OvernightIndexCashflow::getIndexEndDate() const {
            return _indexEndDate;
        };

        double OvernightIndexCashflow::_calculateInterest(QCDate& date, double indexValue)
        {
            double eqRate = getEqRate(date, indexValue);
            _rate.setValue(eqRate * _gearing + _spread);
            return _notional * (_rate.wf(_accrualStartDate, date) - 1);
        }

        QCDate OvernightIndexCashflow::getSettlementDate() const {
            return _settlementDate;
        }

        double OvernightIndexCashflow::accruedInterest(const QCDate& fecha, const TimeSeries& fixings)
        {
            auto indexValue = 0.0;
            try
            {
                indexValue = fixings.at(fecha);
            }
            catch (const exception& e)
            {
                throw invalid_argument("Index value not found for date " +
                                       fecha.description(false) + ".");
            }
            auto fechaOk = fecha;
            return _calculateInterest(fechaOk, indexValue);
        }

        OvernightIndexCashflow::~OvernightIndexCashflow() = default;

    } // QCode
}

