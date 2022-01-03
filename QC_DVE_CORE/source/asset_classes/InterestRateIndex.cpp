#include "asset_classes/InterestRateIndex.h"

namespace QCode {
    namespace Financial {
        InterestRateIndex::InterestRateIndex(
                std::string code,
                QCInterestRate rate,
                Tenor startLag,
                Tenor tenor,
                QCBusinessCalendar fixingCalendar,
                QCBusinessCalendar valueCalendar,
                std::shared_ptr<QCCurrency> ccy) :
                FinancialIndex(FinancialIndex::InterestRate, code),
                _rate(rate),
                _startLag(startLag),
                _tenor(tenor),
                _fixingCalendar(fixingCalendar),
                _valueCalendar(valueCalendar),
                _ccy(ccy) {
        }

        void InterestRateIndex::setRateValue(double rateValue) {
            _rate.setValue(rateValue);
        }

        QCDate InterestRateIndex::getStartDate(QCDate &fixingDate) {
            auto dias = _startLag.getDays();
            return _fixingCalendar.shift(fixingDate, dias);
        }

        QCDate InterestRateIndex::getEndDate(QCDate &fixingDate) {
            auto dias = _startLag.getDays();
            auto startDate = _fixingCalendar.shift(fixingDate, dias);
            if (_tenor.getString() == "2D") {
                dias = _tenor.getDays();
                auto temp = _valueCalendar.shift(startDate, dias);
                auto meses = _tenor.getTotalMonths();
                return _valueCalendar.nextBusinessDay(temp.addMonths(meses));
            } else {
                auto meses = _tenor.getTotalMonths();
                auto temp = startDate.addMonths(meses);
                dias = _tenor.getDays();
                return _valueCalendar.nextBusinessDay(temp.addDays(dias));
            }
        }

        Tenor InterestRateIndex::getTenor() {
            return _tenor;
        }

        QCInterestRate InterestRateIndex::getRate() {
            return _rate;
        }

        unsigned int InterestRateIndex::getDaysOfStartLag() {
            return _startLag.getDays();
        }

        unsigned int InterestRateIndex::getMonthsOfTenor() {
            return _tenor.getMonths() + _tenor.getYears() * 12;
        }

        InterestRateIndex::~InterestRateIndex() {
        }

    }
}