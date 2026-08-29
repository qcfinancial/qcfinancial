//
// Created by Alvaro Patricio Diaz Valenzuela on 2023-11-17.
//

#ifndef QC_DVE_CORE_COMPOUNDEDOVERNIGHTRATEMULTICURRENCYCASHFLOW2_H
#define QC_DVE_CORE_COMPOUNDEDOVERNIGHTRATEMULTICURRENCYCASHFLOW2_H

#include <cashflows/CompoundedOvernightRateCashflow2.h>
#include "asset_classes/QCCurrencyConverter.h"
#include <asset_classes/FXRateIndex.h>


namespace QCode::Financial {
    class CompoundedOvernightRateMultiCurrencyCashflow2 : public CompoundedOvernightRateCashflow2 {
    public:
        typedef std::tuple<
                std::string,               /* Start date */
                std::string,               /* End date */
                std::string,               /* Settlement date */
                double,                    /* Nominal */
                double,                    /* Amortization */
                double,                    /* Interest */
                bool,                      /* Amortization is cashflow */
                double,                    /* Total cashflow */
                std::string,               /* Nominal currency */
                std::string,               /* Interest rate index code */
                std::string,               /* Interest rate */
                double,                    /* Spread */
                double,                    /* Gearing */
                double,                    /* Interest rate value */
                std::string,               /* Settlement Currency */
                std::string,               /* FX Rate Index */
                std::string,               /* FX Rate Index Fixing Date */
                double,                    /* FX Rate Index Value */
                double,                    /* Interest in settlement currency */
                double,                    /* Amortization in settlement currency */
                double                     /* Cashflow in settlement currency */
        >
                CompoundedOvernightRateMultiCurrencyCashflow2Wrapper;

        CompoundedOvernightRateMultiCurrencyCashflow2(
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
                const QCInterestRate &interestRate,
                unsigned int eqRateDecimalPlaces,
                unsigned int lookback,
                unsigned int lockout,
                const QCDate &fxRateIndexFixingDate,
                std::shared_ptr<QCCurrency> settlementCurrency,
                std::shared_ptr<FXRateIndex> fxRateIndex);

        [[nodiscard]] std::string getType() const override;

        double settlementAmount() override;

        shared_ptr<QCCurrency> settlementCurrency() override;

        void setFxRateIndexValue(double fxRateIndexValue);

        void setFxRateIndexValue(const TimeSeries &fxRateIndexValues);

        void setFxRateNotionalCurveDerivatives(const std::vector<double>& der);

        void setFxRateSettlementCurveDerivatives(const std::vector<double>& der);

        void setFxRateSpotDerivative(double der);

        [[nodiscard]] std::vector<double> getAmountNotionalCurveDerivatives() const;

        [[nodiscard]] std::vector<double> getAmountSettlementCurveDerivatives() const;

        [[nodiscard]] double getAmountFxDelta() const;

        [[nodiscard]] std::string getFXRateIndexCode() const;

        [[nodiscard]] double getFXRateIndexValue() const;

        [[nodiscard]] QCDate getFXRateIndexFixingDate() const;

        [[nodiscard]] shared_ptr<FXRateIndex> getFXRateIndex() const;

        [[nodiscard]] double toSettlementCurrency(double amount);

        [[nodiscard]] double toSettlementCurrency(double amount, const TimeSeries& fxRateIndexValues);

        double settlementCurrencyAmount();

        std::shared_ptr<CompoundedOvernightRateMultiCurrencyCashflow2Wrapper> mccyWrap();

        Record record() override;



    protected:
        QCDate _fxRateIndexFixingDate;
        std::shared_ptr<QCCurrency> _settlementCurrency;
        std::shared_ptr<FXRateIndex> _fxRateIndex;
        double _fxRateIndexValue;
        QCCurrencyConverter _currencyConverter;

        /** @brief	Derivatives of the CIP-projected FX forward with respect to the notional
         * curve's vertices. Set by ForwardFXRates::setFXRateCIP. */
        std::vector<double> _fxRateNotionalCurveDerivatives;

        /** @brief	Derivatives of the CIP-projected FX forward with respect to the settlement
         * curve's vertices. Set by ForwardFXRates::setFXRateCIP. */
        std::vector<double> _fxRateSettlementCurveDerivatives;

        /** @brief	Derivative of the CIP-projected FX forward with respect to the spot FX value. */
        double _fxRateSpotDerivative{0.0};

        /** @brief	Derivatives of settlementCurrencyAmount() with respect to the notional
         * curve's vertices. Cached by settlementCurrencyAmount(). */
        std::vector<double> _amountNotionalCurveDerivatives;

        /** @brief	Derivatives of settlementCurrencyAmount() with respect to the settlement
         * curve's vertices. Cached by settlementCurrencyAmount(). */
        std::vector<double> _amountSettlementCurveDerivatives;

        /** @brief	Derivative of settlementCurrencyAmount() with respect to the spot FX value.
         * Cached by settlementCurrencyAmount(). */
        double _amountFxDelta{0.0};

    };

}


#endif //QC_DVE_CORE_COMPOUNDEDOVERNIGHTRATEMULTICURRENCYCASHFLOW2_H
