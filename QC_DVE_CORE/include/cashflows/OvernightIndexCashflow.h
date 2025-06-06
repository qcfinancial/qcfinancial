#ifndef OVERNIGHTINDEXCASHFLOW_H
#define OVERNIGHTINDEXCASHFLOW_H

#include<tuple>
#include<memory>

#include "cashflows/Cashflow.h"
#include "asset_classes/QCInterestRate.h"
#include "asset_classes/QCAct360.h"
#include "asset_classes/QC30360.h"
#include "asset_classes/QCLinearWf.h"
#include "TypeAliases.h"

namespace QCode
{
    namespace Financial
    {
        const double DEFAULT_INDEX = 1.0;
        const unsigned int DEFAULT_EQ_RATE_DECIMAL_PLACES = 8;

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
        double                       /* Gearing */
        > OvernightIndexCashflowWrapper;

        enum class DatesForEquivalentRate
        {
            qcAccrual = 1,
            qcIndex = 2
        };

        class OvernightIndexCashflow : public Cashflow {
        public:
            OvernightIndexCashflow(
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
                    std::string  indexName,
                    unsigned int eqRateDecimalPlaces,
                    DatesForEquivalentRate datesForEquivalentRate);

            double amount() override;


            shared_ptr<QCCurrency> ccy() override;


            QCDate date() override;

            QCDate endDate() override;


            [[nodiscard]] std::string getType() const override;


            [[nodiscard]] QCDate getStartDate() const;


            [[nodiscard]] QCDate getEndDate() const;


            [[nodiscard]] QCDate getSettlementDate() const;


            [[nodiscard]] double getNotional() const;


            void setNotional(double notional);


            [[nodiscard]] double getAmortization() const;


            void setAmortization(double amortization);


            void setStartDateIndex(double indexValue);


            void setStartDateIndexDerivatives(std::vector<double> der);


            [[nodiscard]] std::vector<double> getStartDateIndexDerivatives() const;


            void setEndDateIndex(double indexValue);


            void setEndDateIndexDerivatives(std::vector<double> der);


            [[nodiscard]] std::vector<double> getEndDateIndexDerivatives() const;


            [[nodiscard]] double getStartDateIndex() const;


            [[nodiscard]] double getEndDateIndex() const;


            [[nodiscard]] QCDate getIndexStartDate() const;


            [[nodiscard]] QCDate getIndexEndDate() const;


            void setEqRateDecimalPlaces(unsigned int decimalPlaces);


            [[nodiscard]] unsigned int getEqRateDecimalPlaces() const;


            double getEqRate(QCDate& date, double indexValue);


            double accruedInterest(QCDate& accrualDate, double indexValue);


            double accruedInterest(const QCDate& fecha, const TimeSeries& fixings);


            virtual double settlementAmount();


            virtual shared_ptr<QCCurrency> settlementCurrency();


            shared_ptr<OvernightIndexCashflowWrapper> wrap();


            virtual Record record();


            DatesForEquivalentRate getDatesForEqRate() const;


            void setDatesForEqRate(DatesForEquivalentRate datesForEqRate);


            double getRateValue();


            std::string getTypeOfRate();


            [[nodiscard]] std::string getIndexCode() const;


            [[nodiscard]] std::vector<double> getAmountDerivatives() const;


            ~OvernightIndexCashflow() override;

        protected:

            /** @brief	The default amount of decimal places for TNA. */
            unsigned int _eqRateDecimalPlaces;

            DatesForEquivalentRate _datesForEquivalentRate;

            /** @brief	Calculates interest (final or accrued) given a date and ICP value. */
            double _calculateInterest(QCDate &date, double indexValue);

            /** @brief	ICP at start date */
            double _startDateIndexValue;

            /** @brief	ICP at end date */
            double _endDateIndexValue;

            /** @brief	The start date */
            QCDate _startDate;

            /** @brief	The end date */
            QCDate _endDate;

            /** @brief	The start date */
            QCDate _indexStartDate;

            /** @brief	The end date */
            QCDate _indexEndDate;

            /** @brief	The settlement date */
            QCDate _settlementDate;

            /** @brief	The nominal */
            double _notional;

            /** @brief	The amortization */
            double _amortization;


            /** @brief	True if amortization is part of the cashflow */
            bool _doesAmortize;

            /** @brief	The nominal currency. It is always CLP. */
            shared_ptr<QCCurrency> _notionalCurrency;

            /** @brief	The spread */
            double _spread;

            /** @brief	The gearing */
            double _gearing;

            QCInterestRate _rate;

            std::string _indexName;

            /** @brief	Stores de derivatives of Index at start date with respect to rates belonging to zero
             * coupon curve.
            */
            std::vector<double> _startDateIndexDerivatives;

            /** @brief	Stores de derivatives of amount() with respect to rates belonging to zero
            * coupon curve.
            */
            std::vector<double> _amountDerivatives;

            /** @brief	Stores de derivatives of Index at end date with respect to rates belonging to zero
            * coupon curve.
            */
            std::vector<double> _endDateIndexDerivatives;
        };
    }
}

#endif // OVERNIGHTINDEXCASHFLOW_H