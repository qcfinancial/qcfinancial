//
// Created by Alvaro Patricio Diaz Valenzuela on 2023-11-14.
//

#include "cashflows/OvernightIndexMultiCurrencyCashflow.h"
#include "asset_classes/QCCurrencyConverter.h"
#include "HelperFunctions.h"

#include <utility>

namespace QCode::Financial {
    OvernightIndexMultiCurrencyCashflow::OvernightIndexMultiCurrencyCashflow(
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
            const QCDate& fxRateIndexFixingDate,
            std::shared_ptr<QCCurrency> settlementCurrency,
            shared_ptr<FXRateIndex> fxRateIndex) :
            OvernightIndexCashflow(
                    accrualStartDate,
                    accrualEndDate,
                    indexStartDate,
                    indexEndDate,
                    settlementDate,
                    std::move(notionalCurrency),
                    notional,
                    amortization,
                    doesAmortize,
                    spread,
                    gearing,
                    rate,
                    std::move(indexName),
                    eqRateDecimalPlaces
                    ),
                    _fxRateIndexFixingDate(fxRateIndexFixingDate),
                    _settlementCurrency(std::move(settlementCurrency)),
                    _fxRateIndex(std::move(fxRateIndex)),
                    _fxRateIndexValue(1.0) {
    };

    shared_ptr<QCCurrency> OvernightIndexMultiCurrencyCashflow::settlementCurrency() {
        return _settlementCurrency;
    }

    void OvernightIndexMultiCurrencyCashflow::setFxRateIndexValue(double fxRateIndexValue) {
        _fxRateIndexValue = fxRateIndexValue;
    }

    std::string OvernightIndexMultiCurrencyCashflow::getFXRateIndexCode() const {
        return _fxRateIndex->getCode();
    }

    double OvernightIndexMultiCurrencyCashflow::getFXRateIndexValue() const {
        return _fxRateIndexValue;
    }

    shared_ptr<FXRateIndex> OvernightIndexMultiCurrencyCashflow::getFXRateIndex() const {
        return _fxRateIndex;
    }

    std::string OvernightIndexMultiCurrencyCashflow::getType() const  {
        return "OvernightIndexMultiCurrencyCashflow";
    }

    void OvernightIndexMultiCurrencyCashflow::_fixIndices(
            const TimeSeries& overnightIndexValues,
            const TimeSeries& fxRateIndexValues) {
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

        if (!QCode::Helpers::isDateInTimeSeries(_indexStartDate, overnightIndexValues))
        {
            std::string msg = "No value for ";
            msg += _indexName + " and date " + _indexStartDate.description(false) + ".";
            throw invalid_argument(msg);
        }
        else
        {
            _startDateIndex = overnightIndexValues.at(_indexStartDate);
        }

        if (!QCode::Helpers::isDateInTimeSeries(_indexEndDate, overnightIndexValues))
        {
            std::string msg = "No value for ";
            msg += _indexName + " and date " + _indexEndDate.description(false) + ".";
            throw invalid_argument(msg);
        }
        else
        {
            _endDateIndex = overnightIndexValues.at(_indexEndDate);
        }
    }

    double OvernightIndexMultiCurrencyCashflow::settlementCurrencyInterest() {
        auto interest = ccy()->amount(_calculateInterest(_endDate, _endDateIndex));
        QCCurrencyConverter ccyConverter;
        return _settlementCurrency->amount(
                ccyConverter.convert(
                        interest,
                        _notionalCurrency,
                        _fxRateIndexValue,
                        *_fxRateIndex));
    }

    double OvernightIndexMultiCurrencyCashflow::settlementCurrencyInterest(
            const TimeSeries& overnightIndexValues,
            const TimeSeries& fxRateIndexValues) {
        _fixIndices(overnightIndexValues, fxRateIndexValues);
        return settlementCurrencyInterest();
    }

    double OvernightIndexMultiCurrencyCashflow::settlementCurrencyAmortization() {
        QCCurrencyConverter ccyConverter;
        return _settlementCurrency->amount(
                ccyConverter.convert(
                ccy()->amount(_amortization),
                _notionalCurrency,
                _fxRateIndexValue,
                *_fxRateIndex));
    }

    double OvernightIndexMultiCurrencyCashflow::settlementCurrencyAmortization(
            const TimeSeries& overnightIndexValues,
            const TimeSeries& fxRateIndexValues) {
        _fixIndices(overnightIndexValues, fxRateIndexValues);
        return settlementCurrencyAmortization();
    }

    double OvernightIndexMultiCurrencyCashflow::settlementCurrencyAmount() {
        auto result = settlementCurrencyInterest();
        if (_doesAmortize)
            result += settlementCurrencyAmortization();
        return result;
    }

    double OvernightIndexMultiCurrencyCashflow::settlementCurrencyAmount(
            const TimeSeries& overnightIndexValues,
            const TimeSeries& fxRateIndexValues) {
        _fixIndices(overnightIndexValues, fxRateIndexValues);
        return settlementCurrencyAmount();
    }

    double OvernightIndexMultiCurrencyCashflow::settlementAmount() {
        return _settlementCurrency->amount(settlementCurrencyAmount());
    }

    std::shared_ptr<OvernightIndexMultiCurrencyCashflowWrapper> OvernightIndexMultiCurrencyCashflow::mccyWrap() {
         /* Accrual Start Date
         Accrual End Date
         Index Start Date
         Index End Date
         Settlement Date
         Notional
         Amortization
         Amortization is cashflow
         Notional Currency code
         Index name
         Start date index value
         End date index value
         Equivalent Rate
         Type of rate
         Interest
         Cashflow
         Spread
         Gearing
         Settlement currency
         FX Rate Index
         FX Rate Index Fixing Date
         FX Rate Index Value
         Interest in settlement currency
         Amortization in settlement currency
         Cashflow in settlement currency */
        auto interes = _calculateInterest(_endDate, _endDateIndex);
        auto flujo = interes;
        if (_doesAmortize)
            flujo += _amortization;

        auto tup1 = wrap();
        auto tup2 = std::make_tuple(
                _settlementCurrency->getIsoCode(),
                _fxRateIndex->getCode(),
                _fxRateIndexFixingDate.description(false),
                _fxRateIndexValue,
                settlementCurrencyInterest(),
                settlementCurrencyAmortization(),
                settlementCurrencyAmount());

        return std::make_shared<OvernightIndexMultiCurrencyCashflowWrapper>(std::tuple_cat(*tup1, tup2));
    }

    QCDate OvernightIndexMultiCurrencyCashflow::getFXRateIndexFixingDate() const {
        return _fxRateIndexFixingDate;
    }
}

