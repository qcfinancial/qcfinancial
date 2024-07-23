//
// Created by Alvaro Patricio Diaz Valenzuela on 2023-11-17.
//

#include "cashflows/CompoundedOvernightRateMultiCurrencyCashflow2.h"
#include "HelperFunctions.h"

namespace QCode::Financial {
    CompoundedOvernightRateMultiCurrencyCashflow2::CompoundedOvernightRateMultiCurrencyCashflow2(
            std::shared_ptr<InterestRateIndex> index,
            const QCDate &startDate,
            const QCDate &endDate,
            const QCDate &settlementDate,
            const std::vector<QCDate> &fixingDates,
            double nominal,
            double amortization,
            bool doesAmortize,
            shared_ptr<QCCurrency>notionalCurrency,
        double spread,
        double gearing,
        const QCInterestRate &interestRate,
        unsigned int eqRateDecimalPlaces,
        unsigned int lookback,
        unsigned int lockout,
        const QCDate &fxRateIndexFixingDate,
        std::shared_ptr<QCCurrency> settlementCurrency,
        std::shared_ptr<FXRateIndex> fxRateIndex
    ) : CompoundedOvernightRateCashflow2(
            index, startDate, endDate, settlementDate, fixingDates,
            nominal, amortization, doesAmortize,
            notionalCurrency, spread, gearing,
            interestRate, eqRateDecimalPlaces, lookback, lockout
            ),
        _fxRateIndexFixingDate(fxRateIndexFixingDate),
        _settlementCurrency(std::move(settlementCurrency)),
        _fxRateIndex(std::move(fxRateIndex)),
        _fxRateIndexValue(1.0) {

    }

    shared_ptr <QCCurrency> CompoundedOvernightRateMultiCurrencyCashflow2::settlementCurrency() {
        return _settlementCurrency;
    };

    void CompoundedOvernightRateMultiCurrencyCashflow2::setFxRateIndexValue(double fxRateIndexValue) {
        _fxRateIndexValue = fxRateIndexValue;
    };

    void CompoundedOvernightRateMultiCurrencyCashflow2::setFxRateIndexValue(const TimeSeries &fxRateIndexValues) {
        if (!QCode::Helpers::isDateInTimeSeries(_fxRateIndexFixingDate, fxRateIndexValues))
        {
            std::string msg = "No value for ";
            msg += _fxRateIndex->getCode() + " and date " + _fxRateIndexFixingDate.description(false) + ".";
            throw invalid_argument(msg);
        }
        else
        {
            _fxRateIndexValue = fxRateIndexValues.at(_fxRateIndexFixingDate);
        }
    }

    [[nodiscard]] std::string CompoundedOvernightRateMultiCurrencyCashflow2::getFXRateIndexCode() const {
        return _fxRateIndex->getCode();
    };

    [[nodiscard]] double CompoundedOvernightRateMultiCurrencyCashflow2::getFXRateIndexValue() const {
        return _fxRateIndexValue;
    };

    [[nodiscard]] QCDate CompoundedOvernightRateMultiCurrencyCashflow2::getFXRateIndexFixingDate() const {
        return _fxRateIndexFixingDate;
    };

    [[nodiscard]] shared_ptr <FXRateIndex> CompoundedOvernightRateMultiCurrencyCashflow2::getFXRateIndex() const {
        return _fxRateIndex;
    };

    std::shared_ptr<CompoundedOvernightRateMultiCurrencyCashflow2::CompoundedOvernightRateMultiCurrencyCashflow2Wrapper> CompoundedOvernightRateMultiCurrencyCashflow2::mccyWrap() {
        auto flujo = _interest;
        if (_doesAmortize)
            flujo += _amortization;

        auto tup1 = wrap();
        auto tup2 = std::make_tuple(
                _settlementCurrency->getIsoCode(),
                _fxRateIndex->getCode(),
                _fxRateIndexFixingDate.description(false),
                _fxRateIndexValue,
                toSettlementCurrency(_interest),
                toSettlementCurrency(_amortization),
                toSettlementCurrency(flujo));

        return std::make_shared<CompoundedOvernightRateMultiCurrencyCashflow2Wrapper>(std::tuple_cat(*tup1, tup2));

    };

    Record CompoundedOvernightRateMultiCurrencyCashflow2::record() {
        auto result = CompoundedOvernightRateCashflow2::record();
        result["type_of_cashflow"] = "compounded_overnight_rate_multi_currency";
        result["type_of_cashflow"] = "overnight_index_multi_currency";
        result["fx_fixing_date"] = _fxRateIndexFixingDate.description(true);
        result["settlement_currency"] = _settlementCurrency->getIsoCode();
        result["fx_rate_index"] = _fxRateIndex->getCode();
        result["fx_rate_index_value"] = _fxRateIndexValue;
        result["amort_sett_currency"] = toSettlementCurrency(_amortization);
        result["interest_sett_currency"] = toSettlementCurrency(_interest);

        return result;

    }

    [[nodiscard]] std::string CompoundedOvernightRateMultiCurrencyCashflow2::getType() const {
        return "CompoundedOvernightRateMultiCurrencyCashflow2";
    };

    [[nodiscard]] double CompoundedOvernightRateMultiCurrencyCashflow2::toSettlementCurrency(double amount) {
        return _currencyConverter.convert(
                amount,
                _notionalCurrency,
                _fxRateIndexValue,
                *_fxRateIndex);
    }

    [[nodiscard]] double CompoundedOvernightRateMultiCurrencyCashflow2::toSettlementCurrency(
            double amount,
            const TimeSeries& fxRateIndexValues) {

        if (!QCode::Helpers::isDateInTimeSeries(_fxRateIndexFixingDate, fxRateIndexValues))
        {
            std::string msg = "No value for ";
            msg += _fxRateIndex->getCode() + " and date " + _fxRateIndexFixingDate.description() + ".";
            throw invalid_argument(msg);
        }

        double fx = fxRateIndexValues.at(_fxRateIndexFixingDate);
        return _currencyConverter.convert(amount, _settlementCurrency, fx, *_fxRateIndex);
    }

    double CompoundedOvernightRateMultiCurrencyCashflow2::settlementAmount() {
        return _settlementCurrency->amount(toSettlementCurrency(CompoundedOvernightRateCashflow2::amount()));
    }
}