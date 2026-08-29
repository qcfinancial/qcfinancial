#include "present_value/PresentValueFX.h"

#include "cashflows/FixedRateMultiCurrencyCashflow.h"
#include "cashflows/IborMultiCurrencyCashflow.h"
#include "cashflows/OvernightIndexMultiCurrencyCashflow.h"
#include "cashflows/CompoundedOvernightRateMultiCurrencyCashflow2.h"
#include "cashflows/SimpleMultiCurrencyCashflow.h"

namespace QCode::Financial {

    double PresentValueFX::pv(
            const QCDate &valuationDate,
            const std::shared_ptr<Cashflow> &cashflow,
            const std::shared_ptr<InterestRateCurve> &discountCurve) {
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
        } else if (typeOfCashflow == "OvernightIndexMultiCurrencyCashflow") {
            auto &cf = dynamic_cast<OvernightIndexMultiCurrencyCashflow &>(*cashflow);
            settlementCurrencyAmount = cf.settlementCurrencyAmount();
            amountNotionalCurveDerivatives = cf.getAmountNotionalCurveDerivatives();
            amountSettlementCurveDerivatives = cf.getAmountSettlementCurveDerivatives();
            amountFxDelta = cf.getAmountFxDelta();
        } else if (typeOfCashflow == "CompoundedOvernightRateMultiCurrencyCashflow2") {
            auto &cf = dynamic_cast<CompoundedOvernightRateMultiCurrencyCashflow2 &>(*cashflow);
            settlementCurrencyAmount = cf.settlementCurrencyAmount();
            amountNotionalCurveDerivatives = cf.getAmountNotionalCurveDerivatives();
            amountSettlementCurveDerivatives = cf.getAmountSettlementCurveDerivatives();
            amountFxDelta = cf.getAmountFxDelta();
        } else if (typeOfCashflow == "SimpleMultiCurrencyCashflow") {
            auto &cf = dynamic_cast<SimpleMultiCurrencyCashflow &>(*cashflow);
            settlementCurrencyAmount = cf.settlementCurrencyAmount();
            amountNotionalCurveDerivatives = cf.getAmountNotionalCurveDerivatives();
            amountSettlementCurveDerivatives = cf.getAmountSettlementCurveDerivatives();
            amountFxDelta = cf.getAmountFxDelta();
        } else {
            throw std::invalid_argument("Type of cashflow " + typeOfCashflow + " not implemented.");
        }

        _notionalCurveDerivatives.assign(amountNotionalCurveDerivatives.size(), 0.0);
        _cipSettlementCurveDerivatives.assign(amountSettlementCurveDerivatives.size(), 0.0);
        _discountCurveDerivatives.assign(discountCurve->getLength(), 0.0);
        _fxDelta = 0.0;

        auto t = valuationDate.dayDiff(cashflow->endDate());
        if (t <= 0) {
            return 0.0;
        }

        auto dfDiscount = discountCurve->getDiscountFactorAt(t);
        auto result = settlementCurrencyAmount * dfDiscount;

        // Notional curve only reaches PV through the cached amount-derivative (discountCurve's
        // discount factor does not depend on the notional curve).
        for (size_t i = 0; i < amountNotionalCurveDerivatives.size(); ++i) {
            _notionalCurveDerivatives.at(i) = amountNotionalCurveDerivatives.at(i) * dfDiscount;
        }
        // CIP-projection curve reaches PV only through the amount (via the FX forward); it has no
        // bearing on discountCurve's own discount factor, so this is not a product-rule term.
        for (size_t j = 0; j < amountSettlementCurveDerivatives.size(); ++j) {
            _cipSettlementCurveDerivatives.at(j) = amountSettlementCurveDerivatives.at(j) * dfDiscount;
        }
        // Discount curve reaches PV only through the discount factor; it has no bearing on the
        // amount (which was fixed by the CIP projection curve), so this too is not a product-rule
        // term. The two vectors above are independent and are NOT summed here: they are only
        // meaningful to sum when the caller used the same curve for CIP projection and discounting.
        for (size_t k = 0; k < discountCurve->getLength(); ++k) {
            _discountCurveDerivatives.at(k) = settlementCurrencyAmount * discountCurve->dfDerivativeAt(k);
        }
        // dfDiscount does not depend on spot, so this is a plain product, not a further chain rule.
        _fxDelta = amountFxDelta * dfDiscount;

        return result;
    }

    double PresentValueFX::pv(
            const QCDate &valuationDate,
            Leg &leg,
            const std::shared_ptr<InterestRateCurve> &discountCurve) {
        std::vector<double> notionalCurveDerivatives;
        std::vector<double> cipSettlementCurveDerivatives;
        std::vector<double> discountCurveDerivatives(discountCurve->getLength(), 0.0);
        double fxDelta = 0.0;
        double result = 0.0;

        for (size_t i = 0; i < leg.size(); ++i) {
            result += pv(valuationDate, leg.getCashflowAt(i), discountCurve);

            if (notionalCurveDerivatives.size() != _notionalCurveDerivatives.size()) {
                notionalCurveDerivatives.assign(_notionalCurveDerivatives.size(), 0.0);
            }
            for (size_t k = 0; k < _notionalCurveDerivatives.size(); ++k) {
                notionalCurveDerivatives.at(k) += _notionalCurveDerivatives.at(k);
            }
            if (cipSettlementCurveDerivatives.size() != _cipSettlementCurveDerivatives.size()) {
                cipSettlementCurveDerivatives.assign(_cipSettlementCurveDerivatives.size(), 0.0);
            }
            for (size_t k = 0; k < _cipSettlementCurveDerivatives.size(); ++k) {
                cipSettlementCurveDerivatives.at(k) += _cipSettlementCurveDerivatives.at(k);
            }
            for (size_t k = 0; k < _discountCurveDerivatives.size(); ++k) {
                discountCurveDerivatives.at(k) += _discountCurveDerivatives.at(k);
            }
            fxDelta += _fxDelta;
        }

        _notionalCurveDerivatives = notionalCurveDerivatives;
        _cipSettlementCurveDerivatives = cipSettlementCurveDerivatives;
        _discountCurveDerivatives = discountCurveDerivatives;
        _fxDelta = fxDelta;

        return result;
    }

    std::vector<double> PresentValueFX::getNotionalCurveDerivatives() const {
        return _notionalCurveDerivatives;
    }

    std::vector<double> PresentValueFX::getCipSettlementCurveDerivatives() const {
        return _cipSettlementCurveDerivatives;
    }

    std::vector<double> PresentValueFX::getDiscountCurveDerivatives() const {
        return _discountCurveDerivatives;
    }

    double PresentValueFX::getFxDelta() const {
        return _fxDelta;
    }

} // QCode::Financial
