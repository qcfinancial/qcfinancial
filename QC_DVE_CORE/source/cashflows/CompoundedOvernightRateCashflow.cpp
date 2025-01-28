//
// Created by ADiazV on 20-12-2021.
//

#include "cashflows/CompoundedOvernightRateCashflow.h"

namespace QCode {
    namespace Financial {
        CompoundedOvernightRateCashflow::CompoundedOvernightRateCashflow(
                std::shared_ptr<InterestRateIndex> index,
                const QCDate &startDate,
                const QCDate &endDate,
                const QCDate &settlementDate,
                const std::vector<QCDate> &fixingDates,
                double nominal,
                double amortization,
                bool doesAmortize,
                shared_ptr<QCCurrency> currency,
                double spread,
                double gearing,
                bool isAct360,
                unsigned int eqRateDecimalPlaces,
                unsigned int lookback = 0,
                unsigned int lockout = 0) :
                _index(index),
                _startDate(startDate),
                _endDate(endDate),
                _settlementDate(settlementDate),
                _fixingDates(fixingDates),
                _nominal(nominal),
                _amortization(amortization),
                _doesAmortize(doesAmortize),
                _currency(currency),
                _spread(spread),
                _gearing(gearing),
                _isAct360(isAct360),
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

        double CompoundedOvernightRateCashflow::amount() {
            auto rate = _index->getRate().getRateFromWf(_endDateWf / _initialDateWf, _startDate, _endDate);
            auto interest = _calculateInterest(rate, _endDate);
            return (_doesAmortize) ? interest + _amortization : interest;
        }


        shared_ptr<QCCurrency> CompoundedOvernightRateCashflow::ccy() {
            return _currency;
        }


        QCDate CompoundedOvernightRateCashflow::date() {
            return _settlementDate;
        }

        QCDate CompoundedOvernightRateCashflow::endDate() {
            return _endDate;
        }


        std::string CompoundedOvernightRateCashflow::getType() const {
            return "CompoundedOvernightRateCashflow";
        }


        std::shared_ptr<QCCurrency> CompoundedOvernightRateCashflow::getInitialCcy() const {
            return _currency;
        }


        const QCDate& CompoundedOvernightRateCashflow::getStartDate() const {
            return _startDate;
        }


        const QCDate& CompoundedOvernightRateCashflow::getEndDate() const {
            return _endDate;
        }


        const QCDate& CompoundedOvernightRateCashflow::getSettlementDate() const {
            return _settlementDate;
        }


        const std::vector<QCDate>& CompoundedOvernightRateCashflow::getFixingDates() const {
            return _fixingDates;
        }


        double CompoundedOvernightRateCashflow::getNominal() const {
            return _nominal;
        }


        double CompoundedOvernightRateCashflow::nominal(const QCDate &fecha) const {
            return _nominal;
        }


        double CompoundedOvernightRateCashflow::getAmortization() const {
            return _amortization;
        }


        double CompoundedOvernightRateCashflow::amortization() const {
            return _amortization;
        }


        double CompoundedOvernightRateCashflow::interest() {
            return _calculateInterest(0.0, _endDate);
        }


        double CompoundedOvernightRateCashflow::interest(const QCode::Financial::TimeSeries &fixings) {
            auto eqRateValue = fixing(fixings);
            return _calculateInterest(eqRateValue, _endDate);
        }


        double CompoundedOvernightRateCashflow::_calculateInterest(double rateValue, QCDate& fecha) {
            _index->setRateValue(rateValue + _spread);
            auto wf = _index->getRate().wf(_startDate, fecha);
            return _nominal * (wf - 1.0);
        }


        double CompoundedOvernightRateCashflow::fixing() {
            throw invalid_argument("A TimeSeries object with overnight rate values is needed.");
        }


        double CompoundedOvernightRateCashflow::fixing(const QCode::Financial::TimeSeries &fixings) {
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


        double CompoundedOvernightRateCashflow::accruedInterest(const QCDate &fecha) {
            throw invalid_argument("A TimeSeries object with overnight rate values is needed.");
        }


        double CompoundedOvernightRateCashflow::accruedInterest(
                const QCDate &fecha,
                const QCode::Financial::TimeSeries &fixings) {
            auto fixing = accruedFixing(fecha, fixings);
            auto f = fecha;
            _interest = _calculateInterest(fixing, f);
            return _interest;
        }


        double CompoundedOvernightRateCashflow::accruedFixing(const QCDate &fecha) {
            throw invalid_argument("A TimeSeries object with overnight rate values is needed.");
        }


        double CompoundedOvernightRateCashflow::accruedFixing(
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


        void CompoundedOvernightRateCashflow::_fillIndexEndDates() {
            if (_indexEndDates.empty()) {
                for (auto& fix_date: _fixingDates) {
                    _indexEndDates[fix_date] = _index->getEndDate(fix_date);
                }
            }
        }


        double CompoundedOvernightRateCashflow::_getFixingWf(
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


        bool CompoundedOvernightRateCashflow::doesAmortize() const {
            return _doesAmortize;
        }


        bool CompoundedOvernightRateCashflow::_validate() {
            auto s = _startDate;
            return true;
        }


        void CompoundedOvernightRateCashflow::setNominal(double nominal) {
            _nominal = nominal;
        }


        void CompoundedOvernightRateCashflow::setAmortization(double amortization) {
            _amortization = amortization;
        }


        std::string CompoundedOvernightRateCashflow::getTypeOfRate() {
            auto rate = _index->getRate();
            return rate.getWealthFactor()->description() + rate.getYearFraction()->description();
        }


        double CompoundedOvernightRateCashflow::getSpread() const {
            return _spread;
        }


        double CompoundedOvernightRateCashflow::getGearing() const {
            return _gearing;
        }


        unsigned int CompoundedOvernightRateCashflow::getEqRateDecimalPlaces() const {
            return _eqRateDecimalPlaces;
        }


        unsigned int CompoundedOvernightRateCashflow::getLookBack() const {
            return _lookback;
        }


        unsigned int CompoundedOvernightRateCashflow::getLockOut() const {
            return _lockout;
        }


        std::vector<double> CompoundedOvernightRateCashflow::getAmountDerivatives() const {
            return _amountDerivatives;
        }


        void CompoundedOvernightRateCashflow::setAmountDerivatives(std::vector<double> amountDerivatives) {
            _amountDerivatives = std::move(amountDerivatives);
        }


        void CompoundedOvernightRateCashflow::setEndDateWf(double wf) {
            _endDateWf = wf;
        }


        shared_ptr<InterestRateIndex> QCode::Financial::CompoundedOvernightRateCashflow::getInterestRateIndex() {
            return _index;
        }


        void CompoundedOvernightRateCashflow::setInitialDateWf(double wf) {
            _initialDateWf = wf;
        }


        std::shared_ptr<CompoundedOvernightRateCashflow::CompoundedOvernightRateCashflowWrapper> CompoundedOvernightRateCashflow::wrap() {
            CompoundedOvernightRateCashflow::CompoundedOvernightRateCashflowWrapper tup = std::make_tuple(
                    _startDate,
                    _endDate,
                    _settlementDate,
                    _nominal,
                    _amortization,
                    _interest,
                    _doesAmortize,
                    amount(),
                    _currency,
                    _index->getCode(),
                    _index->getRate(),
                    _spread,
                    _gearing,
                    _getRateValue());

            return std::make_shared<CompoundedOvernightRateCashflow::CompoundedOvernightRateCashflowWrapper>(tup);
        }


        double CompoundedOvernightRateCashflow::_getRateValue() const {
            auto wf = _endDateWf / _initialDateWf;
            if (wf == 1.0)
                return 0.0;
            auto rate = _index->getRate();

            return rate.getRateFromWf(wf, _startDate.dayDiff(_endDate));
        }


        std::string CompoundedOvernightRateCashflow::getInterestRateIndexCode() const
        {
            return _index->getCode();
        }
    }
}



