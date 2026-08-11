#include "present_value/PresentValueFX.h"

#include "cashflows/FixedRateMultiCurrencyCashflow.h"
#include "cashflows/IborMultiCurrencyCashflow.h"

namespace QCode::Financial {

    double PresentValueFX::pv(
            const QCDate &valuationDate,
            const std::shared_ptr<Cashflow> &cashflow,
            const std::shared_ptr<InterestRateCurve> &settlementCurve) {
        auto typeOfCashflow = cashflow->getType();

        double settlementCurrencyAmount;
        std::vector<double> amountNotionalCurveDerivatives;
        std::vector<double> amountSettlementCurveDerivatives;
        double amountFxDelta;

        if (typeOfCashflow == "FixedRateMultiCurrencyCashflow") {
            auto &cf = dynamic_cast<FixedRateMultiCurrencyCashflow &>(*cashflow);
            settlementCurrencyAmount = cf.settlementCurrencyAmount();
            amountNotionalCurveDerivatives = cf.getAmountNotionalCurveDerivatives();
            amountSettlementCurveDerivatives = cf.getAmountSettlementCurveDerivatives();
            amountFxDelta = cf.getAmountFxDelta();
        } else if (typeOfCashflow == "IborMultiCurrencyCashflow") {
            auto &cf = dynamic_cast<IborMultiCurrencyCashflow &>(*cashflow);
            settlementCurrencyAmount = cf.settlementCurrencyAmount();
            amountNotionalCurveDerivatives = cf.getAmountNotionalCurveDerivatives();
            amountSettlementCurveDerivatives = cf.getAmountSettlementCurveDerivatives();
            amountFxDelta = cf.getAmountFxDelta();
        } else {
            throw std::invalid_argument("Type of cashflow " + typeOfCashflow + " not implemented.");
        }

        _notionalCurveDerivatives.assign(amountNotionalCurveDerivatives.size(), 0.0);
        _settlementCurveDerivatives.assign(amountSettlementCurveDerivatives.size(), 0.0);
        _fxDelta = 0.0;

        auto t = valuationDate.dayDiff(cashflow->endDate());
        if (t <= 0) {
            return 0.0;
        }

        auto dfSettlement = settlementCurve->getDiscountFactorAt(t);
        auto result = settlementCurrencyAmount * dfSettlement;

        // Notional curve only reaches PV through the cached amount-derivative (settlementCurve's
        // discount factor does not depend on the notional curve).
        for (size_t i = 0; i < amountNotionalCurveDerivatives.size(); ++i) {
            _notionalCurveDerivatives.at(i) = amountNotionalCurveDerivatives.at(i) * dfSettlement;
        }
        // Settlement curve reaches PV through both the amount (via the FX forward) and the discount
        // factor itself: plain product rule, no shortcut taken for either term.
        for (size_t j = 0; j < amountSettlementCurveDerivatives.size(); ++j) {
            _settlementCurveDerivatives.at(j) = amountSettlementCurveDerivatives.at(j) * dfSettlement +
                    settlementCurrencyAmount * settlementCurve->dfDerivativeAt(j);
        }
        // dfSettlement does not depend on spot, so this is a plain product, not a further chain rule.
        _fxDelta = amountFxDelta * dfSettlement;

        return result;
    }

    double PresentValueFX::pv(
            const QCDate &valuationDate,
            Leg &leg,
            const std::shared_ptr<InterestRateCurve> &settlementCurve) {
        std::vector<double> notionalCurveDerivatives;
        std::vector<double> settlementCurveDerivatives(settlementCurve->getLength(), 0.0);
        double fxDelta = 0.0;
        double result = 0.0;

        for (size_t i = 0; i < leg.size(); ++i) {
            result += pv(valuationDate, leg.getCashflowAt(i), settlementCurve);

            if (notionalCurveDerivatives.size() != _notionalCurveDerivatives.size()) {
                notionalCurveDerivatives.assign(_notionalCurveDerivatives.size(), 0.0);
            }
            for (size_t k = 0; k < _notionalCurveDerivatives.size(); ++k) {
                notionalCurveDerivatives.at(k) += _notionalCurveDerivatives.at(k);
            }
            for (size_t k = 0; k < _settlementCurveDerivatives.size(); ++k) {
                settlementCurveDerivatives.at(k) += _settlementCurveDerivatives.at(k);
            }
            fxDelta += _fxDelta;
        }

        _notionalCurveDerivatives = notionalCurveDerivatives;
        _settlementCurveDerivatives = settlementCurveDerivatives;
        _fxDelta = fxDelta;

        return result;
    }

    std::vector<double> PresentValueFX::getNotionalCurveDerivatives() const {
        return _notionalCurveDerivatives;
    }

    std::vector<double> PresentValueFX::getSettlementCurveDerivatives() const {
        return _settlementCurveDerivatives;
    }

    double PresentValueFX::getFxDelta() const {
        return _fxDelta;
    }

} // QCode::Financial
