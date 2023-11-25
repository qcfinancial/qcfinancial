//
// Created by Alvaro Patricio Diaz Valenzuela on 2023-11-16.
//

#include "cashflows/CompoundedOvernightRateCashflow2.h"

#include <utility>


    namespace QCode::Financial {
        CompoundedOvernightRateCashflow2::CompoundedOvernightRateCashflow2(
                std::shared_ptr<InterestRateIndex> index,
                const QCDate &startDate,
                const QCDate &endDate,
                const QCDate &settlementDate,
                const std::vector<QCDate> &fixingDates,
                double nominal,
                double amortization,
                bool doesAmortize,
                shared_ptr<QCCurrency> notionalCurrency,
                double spread,
                double gearing,
                const QCInterestRate& interestRate,
                unsigned int eqRateDecimalPlaces,
                unsigned int lookback = 0,
                unsigned int lockout = 0) :
                _index(std::move(index)),
                _startDate(startDate),
                _endDate(endDate),
                _settlementDate(settlementDate),
                _fixingDates(fixingDates),
                _notional(nominal),
                _amortization(amortization),
                _doesAmortize(doesAmortize),
                _notionalCurrency(std::move(notionalCurrency)),
                _spread(spread),
                _gearing(gearing),
                _interestRate(interestRate),
                _eqRateDecimalPlaces(eqRateDecimalPlaces),
                _lookback(lookback),
                _lockout(lockout),
                _initialDateWf(1.0),
                _endDateWf(1.0),
                _interest(0.0) {

            // Make sure fixing dates are sorted in ascending order.
            sort(_fixingDates.begin(), _fixingDates.end());

            _interest = _calculateInterest(0.0, _endDate);

            // Initialize _amountDerivatives
            _amountDerivatives.push_back(0.0);
            _amountDerivatives.push_back(0.0);
        }

        double CompoundedOvernightRateCashflow2::amount() {
            auto rate = _index->getRate().getRateFromWf(_endDateWf / _initialDateWf, _startDate, _endDate);
            auto interest = _calculateInterest(rate, _endDate);
            return (_doesAmortize) ? interest + _amortization : interest;
        }


        shared_ptr<QCCurrency> CompoundedOvernightRateCashflow2::ccy() {
            return _notionalCurrency;
        }


        QCDate CompoundedOvernightRateCashflow2::date() {
            return _settlementDate;
        }


        std::string CompoundedOvernightRateCashflow2::getType() const {
            return "CompoundedOvernightRateCashflow2";
        }


        const QCDate& CompoundedOvernightRateCashflow2::getStartDate() const {
            return _startDate;
        }


        const QCDate& CompoundedOvernightRateCashflow2::getEndDate() const {
            return _endDate;
        }


        const QCDate& CompoundedOvernightRateCashflow2::getSettlementDate() const {
            return _settlementDate;
        }


        const std::vector<QCDate>& CompoundedOvernightRateCashflow2::getFixingDates() const {
            return _fixingDates;
        }


        double CompoundedOvernightRateCashflow2::getNotional() const {
            return _notional;
        }


        double CompoundedOvernightRateCashflow2::getAmortization() const {
            return _amortization;
        }


        double CompoundedOvernightRateCashflow2::interestFromSpread() {
            return _calculateInterest(0.0, _endDate);
        }


        double CompoundedOvernightRateCashflow2::interest(const QCode::Financial::TimeSeries &fixings) {
            auto eqRateValue = fixing(fixings);
            _interest = _calculateInterest(eqRateValue, _endDate);
            return _interest;
        }


        double CompoundedOvernightRateCashflow2::_calculateInterest(double rateValue, QCDate& fecha) {
            _index->setRateValue(rateValue + _spread);
            auto wf = _index->getRate().wf(_startDate, fecha);
            return _notional * (wf - 1.0);
        }


        double CompoundedOvernightRateCashflow2::fixing(const QCode::Financial::TimeSeries &fixings) {
            // lockout != 0 is not implemented
            _fillIndexEndDates();

            double producto = 1.0;
            for (auto& fix_date: _fixingDates) {
                producto *= _getFixingWf(fix_date, fixings);
            }
            _endDateWf = producto;
            auto eqRate = _index->getRate().getRateFromWf(producto, _startDate, _endDate);
            double factor = std::pow(10, _eqRateDecimalPlaces);
            return std::round(eqRate * factor) / factor;
        }


        double CompoundedOvernightRateCashflow2::accruedInterest(
                const QCDate &fecha,
                const QCode::Financial::TimeSeries &fixings) {
            auto fixing = accruedFixing(fecha, fixings);
            auto f = fecha;
            _interest = _calculateInterest(fixing, f);
            return _interest;
        }


        double CompoundedOvernightRateCashflow2::accruedFixing(
                const QCDate &fecha,
                const QCode::Financial::TimeSeries &fixings) {
            if (fecha <= _startDate) return 0.0;
            // lockout != 0 is not implemented
            _fillIndexEndDates();

            double producto = 1.0;
            for (auto& fix_date: _fixingDates) {
                if (fix_date < fecha) {
                    producto *= _getFixingWf(fix_date, fixings);
                }
                else {
                    break;
                }
            }
            auto f = fecha;
            auto eqRate = _index->getRate().getRateFromWf(producto, _startDate, f);
            double factor = std::pow(10, _eqRateDecimalPlaces);
            auto accFixing =  std::round(eqRate * factor) / factor;
            _interest = _calculateInterest(accFixing, f);
            return accFixing;
        }


        void CompoundedOvernightRateCashflow2::_fillIndexEndDates() {
            if (_indexEndDates.empty()) {
                for (auto& fix_date: _fixingDates) {
                    _indexEndDates[fix_date] = _index->getEndDate(fix_date);
                }
            }
        }


        double CompoundedOvernightRateCashflow2::_getFixingWf(
                QCDate &fecha,
                const QCode::Financial::TimeSeries &fixings) {
            auto fix_rate = fixings.find(fecha);
            if (fix_rate != fixings.end()) {
                _index->setRateValue(fix_rate->second);
                return _index->getRate().wf(fecha, _indexEndDates[fecha]);
            }
            else {
                auto msg = "There is no value for " + _index->getCode() + " in TimeSeries for " + fecha.description(false);
                throw std::invalid_argument(msg);
            }

        }


        bool CompoundedOvernightRateCashflow2::doesAmortize() const {
            return _doesAmortize;
        }


        bool CompoundedOvernightRateCashflow2::_validate() {
            auto s = _startDate;
            return true;
        }


        void CompoundedOvernightRateCashflow2::setNotional(double notional) {
            _notional = notional;
        }


        void CompoundedOvernightRateCashflow2::setAmortization(double amortization) {
            _amortization = amortization;
        }


        std::string CompoundedOvernightRateCashflow2::getTypeOfRate() {
            auto rate = _index->getRate();
            return rate.getWealthFactor()->description() + rate.getYearFraction()->description();
        }


        double CompoundedOvernightRateCashflow2::getSpread() const {
            return _spread;
        }


        double CompoundedOvernightRateCashflow2::getGearing() const {
            return _gearing;
        }


        unsigned int CompoundedOvernightRateCashflow2::getEqRateDecimalPlaces() const {
            return _eqRateDecimalPlaces;
        }


        unsigned int CompoundedOvernightRateCashflow2::getLookBack() const {
            return _lookback;
        }


        unsigned int CompoundedOvernightRateCashflow2::getLockOut() const {
            return _lockout;
        }


        std::vector<double> CompoundedOvernightRateCashflow2::getAmountDerivatives() const {
            return _amountDerivatives;
        }


        void CompoundedOvernightRateCashflow2::setAmountDerivatives(std::vector<double> amountDerivatives) {
            _amountDerivatives = std::move(amountDerivatives);
        }


        void CompoundedOvernightRateCashflow2::setEndDateWf(double wf) {
            _endDateWf = wf;
        }


        shared_ptr<InterestRateIndex> QCode::Financial::CompoundedOvernightRateCashflow2::getInterestRateIndex() {
            return _index;
        }


        void CompoundedOvernightRateCashflow2::setInitialDateWf(double wf) {
            _initialDateWf = wf;
        }


        std::shared_ptr<CompoundedOvernightRateCashflow2::CompoundedOvernightRateCashflow2Wrapper> CompoundedOvernightRateCashflow2::wrap() {
            auto type_of_rate = _index->getRate().getWealthFactor()->description() + _index->getRate().getYearFraction()->description();
            CompoundedOvernightRateCashflow2::CompoundedOvernightRateCashflow2Wrapper tup = std::make_tuple(
                    _startDate.description(false),
                    _endDate.description(false),
                    _settlementDate.description(false),
                    _notional,
                    _amortization,
                    _interest,
                    _doesAmortize,
                    amount(),
                    _notionalCurrency->getIsoCode(),
                    _index->getCode(),
                    type_of_rate,
                    _spread,
                    _gearing,
                    _getRateValue());

            return std::make_shared<CompoundedOvernightRateCashflow2::CompoundedOvernightRateCashflow2Wrapper>(tup);
        }


        double CompoundedOvernightRateCashflow2::_getRateValue() const {
            auto wf = _endDateWf / _initialDateWf;
            if (wf == 1.0)
                return 0.0;
            auto rate = _index->getRate();

            return rate.getRateFromWf(wf, _startDate.dayDiff(_endDate));
        }


        std::string CompoundedOvernightRateCashflow2::getInterestRateIndexCode() const
        {
            return _index->getCode();
        }

    } // QCode
// Financial