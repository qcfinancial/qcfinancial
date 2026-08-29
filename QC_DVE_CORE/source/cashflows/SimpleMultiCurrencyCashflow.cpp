#include "cashflows/SimpleMultiCurrencyCashflow.h"

namespace QCode {
    namespace Financial {
        SimpleMultiCurrencyCashflow::SimpleMultiCurrencyCashflow(
                const QCDate &endDate,
                double nominal,
                std::shared_ptr<QCCurrency> currency,
                const QCDate &fxRateIndexFixingDate,
                std::shared_ptr<QCCurrency> settlementCurrency,
                std::shared_ptr<FXRateIndex> fxRateIndex,
                double fxRateIndexValue) :
                SimpleCashflow(endDate, nominal, currency),
                _fxRateIndexFixingDate(fxRateIndexFixingDate),
                _settlementCurrency(settlementCurrency),
                _fxRateIndex(fxRateIndex),
                _fxRateIndexValue(fxRateIndexValue) {
            if (!_validate()) {
                throw invalid_argument(_validateMsg);
            }
        }

        double SimpleMultiCurrencyCashflow::amount() {
            return _nominal;
        }

        double SimpleMultiCurrencyCashflow::settlementAmount() {
            return settlementCurrencyAmount();
        }

        double SimpleMultiCurrencyCashflow::settlementCurrencyAmount() {
            _amountNotionalCurveDerivatives.assign(_fxRateNotionalCurveDerivatives.size(), 0.0);
            _amountSettlementCurveDerivatives.assign(_fxRateSettlementCurveDerivatives.size(), 0.0);
            _amountFxDelta = 0.0;

            if (_currency->getIsoCode() == _settlementCurrency->getIsoCode()) {
                return _nominal;
            }

            bool isStrong = _fxRateIndex->strongCcyCode() == _currency->getIsoCode();
            if (isStrong) {
                for (size_t i = 0; i < _fxRateNotionalCurveDerivatives.size(); ++i) {
                    _amountNotionalCurveDerivatives.at(i) = _nominal * _fxRateNotionalCurveDerivatives.at(i);
                }
                for (size_t j = 0; j < _fxRateSettlementCurveDerivatives.size(); ++j) {
                    _amountSettlementCurveDerivatives.at(j) = _nominal * _fxRateSettlementCurveDerivatives.at(j);
                }
                _amountFxDelta = _nominal * _fxRateSpotDerivative;
                return _nominal * _fxRateIndexValue;
            } else {
                auto fInvSq = 1.0 / (_fxRateIndexValue * _fxRateIndexValue);
                for (size_t i = 0; i < _fxRateNotionalCurveDerivatives.size(); ++i) {
                    _amountNotionalCurveDerivatives.at(i) = -_nominal * fInvSq * _fxRateNotionalCurveDerivatives.at(i);
                }
                for (size_t j = 0; j < _fxRateSettlementCurveDerivatives.size(); ++j) {
                    _amountSettlementCurveDerivatives.at(j) = -_nominal * fInvSq * _fxRateSettlementCurveDerivatives.at(j);
                }
                _amountFxDelta = -_nominal * fInvSq * _fxRateSpotDerivative;
                return _nominal / _fxRateIndexValue;
            }
        }

        void SimpleMultiCurrencyCashflow::setFxRateNotionalCurveDerivatives(const std::vector<double>& der) {
            _fxRateNotionalCurveDerivatives = der;
        }

        void SimpleMultiCurrencyCashflow::setFxRateSettlementCurveDerivatives(const std::vector<double>& der) {
            _fxRateSettlementCurveDerivatives = der;
        }

        void SimpleMultiCurrencyCashflow::setFxRateSpotDerivative(double der) {
            _fxRateSpotDerivative = der;
        }

        std::vector<double> SimpleMultiCurrencyCashflow::getAmountNotionalCurveDerivatives() const {
            return _amountNotionalCurveDerivatives;
        }

        std::vector<double> SimpleMultiCurrencyCashflow::getAmountSettlementCurveDerivatives() const {
            return _amountSettlementCurveDerivatives;
        }

        double SimpleMultiCurrencyCashflow::getAmountFxDelta() const {
            return _amountFxDelta;
        }

        double SimpleMultiCurrencyCashflow::nominal() {
            return _nominal;
        }

        shared_ptr<QCCurrency> SimpleMultiCurrencyCashflow::settlementCcy() {
            return _settlementCurrency;
        }

        void SimpleMultiCurrencyCashflow::setFxRateIndexValue(double fxRateIndexValue) {
            _fxRateIndexValue = fxRateIndexValue;
        }

        shared_ptr<SimpleMultiCurrencyCashflowWrapper> SimpleMultiCurrencyCashflow::wrap() {
            SimpleMultiCurrencyCashflowWrapper tup = std::make_tuple(
                    _endDate,
                    _nominal,
                    _currency,
                    _fxRateIndexFixingDate,
                    _settlementCurrency,
                    _fxRateIndex,
                    _fxRateIndexValue,
                    settlementAmount());

            return std::make_shared<SimpleMultiCurrencyCashflowWrapper>(tup);
        }

        bool SimpleMultiCurrencyCashflow::_validate() {
            bool result;
            _validateMsg = "";
            if (_fxRateIndex->strongCcyCode() == _currency->getIsoCode() &&
                _fxRateIndex->weakCcyCode() == _settlementCurrency->getIsoCode()) {
                result = true;
            } else if (_fxRateIndex->weakCcyCode() == _currency->getIsoCode() &&
                       _fxRateIndex->strongCcyCode() == _settlementCurrency->getIsoCode()) {
                result = true;
            } else {
                result = false;
                _validateMsg += "Fx Rate Index provided is not compatible with nominal and ";
                _validateMsg += "settlement currency. ";
            }
            if (_fxRateIndexFixingDate > _endDate) {
                result = false;
                _validateMsg += "Fx Rate fixing date is gt settlement date.";
            }
            return result;
        }

        SimpleMultiCurrencyCashflow::~SimpleMultiCurrencyCashflow() = default;

        std::string SimpleMultiCurrencyCashflow::getType() const {
            return "SimpleMultiCurrencyCashflow";
        }

        std::string SimpleMultiCurrencyCashflow::getFXRateIndexCode() const {
            return _fxRateIndex->getCode();
        }

        QCDate SimpleMultiCurrencyCashflow::getFXRateIndexFixingDate() const {
            return _fxRateIndexFixingDate;
        }

        Record SimpleMultiCurrencyCashflow::record() {
            auto result = Record();
            result["type_of_cashflow"] = "simple_multi_currency";
            result["end_date"] = _endDate.description(false);
            result["notional"] = _nominal;
            result["amortization"] = _nominal;
            result["amort_is_cashflow"] = true;
            result["cashflow"] = amount();
            result["notional_currency"] = ccy()->getIsoCode();
            result["fx_fixing_date"] = _fxRateIndexFixingDate.description(true);
            result["settlement_currency"] = _settlementCurrency->getIsoCode();
            result["fx_rate_index"] = _fxRateIndex->getCode();
            result["fx_rate_index_value"] = _fxRateIndexValue;
            result["present_value"] = getPresentValue();
            result["discount_factor"] = getDiscountFactor();

            return result;
        }

    }
}