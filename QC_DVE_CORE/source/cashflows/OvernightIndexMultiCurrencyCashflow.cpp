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
            const QCDate &fxRateIndexFixingDate,
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

    shared_ptr<QCCurrency> OvernightIndexMultiCurrencyCashflow::settlementCcy() {
        return _settlementCurrency;
    }

    void OvernightIndexMultiCurrencyCashflow::setFxRateIndexValue(double fxRateIndexValue) {
        _fxRateIndexValue = fxRateIndexValue;
    }

    std::string OvernightIndexMultiCurrencyCashflow::getFXRateIndexCode() const {
        _fxRateIndex->getCode();
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
            double fxRateIndexValue{ fxRateIndexValues.at(_fxRateIndexFixingDate) };
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
        auto interest = _calculateInterest(_accrualEndDate, _endDateIndex);
        QCCurrencyConverter ccyConverter;
        return ccyConverter.convert(
                interest,
                _notionalCurrency,
                _fxRateIndexValue,
                *_fxRateIndex);
    }

    double OvernightIndexMultiCurrencyCashflow::settlementCurrencyInterest(
            const TimeSeries& overnightIndexValues,
            const TimeSeries& fxRateIndexValues) {
        _fixIndices(overnightIndexValues, fxRateIndexValues);
        return settlementCurrencyInterest();
    }

    double OvernightIndexMultiCurrencyCashflow::settlementCurrencyAmortization() {
        QCCurrencyConverter ccyConverter;
        return ccyConverter.convert(
                _amortization,
                _notionalCurrency,
                _fxRateIndexValue,
                *_fxRateIndex);
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

    std::shared_ptr<OvernightIndexMultiCurrencyCashflowWrapper> OvernightIndexMultiCurrencyCashflow::wrap() {

    }
}

