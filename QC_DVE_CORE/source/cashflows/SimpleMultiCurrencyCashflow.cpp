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
            if (_currency->getIsoCode() == _settlementCurrency->getIsoCode()) {
                return _nominal;
            }
            if (_fxRateIndex->strongCcyCode() == _currency->getIsoCode()) {
                return _nominal * _fxRateIndexValue;
            } else {
                return _nominal / _fxRateIndexValue;
            }
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

    }
}