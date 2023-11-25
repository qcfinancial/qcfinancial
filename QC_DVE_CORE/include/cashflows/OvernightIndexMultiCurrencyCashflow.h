//
// Created by Alvaro Patricio Diaz Valenzuela on 2023-11-14.
//

#ifndef QC_DVE_CORE_OVERNIGHTINDEXMULTICURRENCYCASHFLOW_H
#define QC_DVE_CORE_OVERNIGHTINDEXMULTICURRENCYCASHFLOW_H

#include <cashflows/OvernightIndexCashflow.h>
#include <asset_classes/FXRateIndex.h>

namespace QCode::Financial {

    typedef std::tuple<
            std::string,                 /* Accrual Start Date */
            std::string,                 /* Accrual End Date */
            std::string,                 /* Index Start Date */
            std::string,                 /* Index End Date */
            std::string,                 /* Settlement Date */
            double,                      /* Notional */
            double,                      /* Amortization */
            bool,                        /* Amortization is cashflow */
            std::string,                 /* Notional Currency code */
            std::string,                 /* Index name */
            double,                      /* Start date index value */
            double,                      /* End date index value */
            double,                      /* Equivalent Rate */
            std::string,                 /* Type of rate */
            double,                      /* Interest */
            double,                      /* Cashflow */
            double,                      /* Spread */
            double,                      /* Gearing */
            std::string,                 /* Settlement currency */
            std::string,                /* FX Rate Index */
            std::string,                 /* FX Rate Index Fixing Date */
            double,                      /* FX Rate Index Value */
            double,                      /* Interest in settlement currency */
            double,                      /* Amortization in settlement currency */
            double                       /* Cashflow in settlement currency */
    > OvernightIndexMultiCurrencyCashflowWrapper;

    class OvernightIndexMultiCurrencyCashflow : public OvernightIndexCashflow {
    public:
        OvernightIndexMultiCurrencyCashflow(
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
                std::shared_ptr<FXRateIndex> fxRateIndex);

        shared_ptr<QCCurrency> settlementCcy();

        void setFxRateIndexValue(double fxRateIndexValue);

        [[nodiscard]] std::string getFXRateIndexCode() const;

        [[nodiscard]] double getFXRateIndexValue() const;

        [[nodiscard]] QCDate getFXRateIndexFixingDate() const;

        [[nodiscard]] shared_ptr<FXRateIndex> getFXRateIndex() const;

        std::shared_ptr<OvernightIndexMultiCurrencyCashflowWrapper> mccyWrap();

        [[nodiscard]] std::string getType() const override;

        double settlementCurrencyInterest();

        double toSettlementCurrency(double amount) const;

        double settlementCurrencyInterest(
                const TimeSeries &overnightIndexValues,
                const TimeSeries &fxRateIndexValues);

        double settlementCurrencyAmortization();

        double settlementCurrencyAmortization(
                const TimeSeries &overnightIndexValues,
                const TimeSeries &fxRateIndexValues);

        double settlementCurrencyAmount();

        double settlementCurrencyAmount(const TimeSeries &overnightIndexValues, const TimeSeries &fxRateIndexValues);

    protected:
        QCDate _fxRateIndexFixingDate;
        std::shared_ptr<QCCurrency> _settlementCurrency;
        std::shared_ptr<FXRateIndex> _fxRateIndex;
        double _fxRateIndexValue;

        void _fixIndices(
                const TimeSeries &overnightIndexValues,
                const TimeSeries &fxRateIndexValues);
    };

}

#endif //QC_DVE_CORE_OVERNIGHTINDEXMULTICURRENCYCASHFLOW_H
