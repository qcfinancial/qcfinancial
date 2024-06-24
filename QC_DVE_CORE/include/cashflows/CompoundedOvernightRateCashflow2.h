//
// Created by Alvaro Patricio Diaz Valenzuela on 2023-11-16.
//

#ifndef QC_DVE_CORE_COMPOUNDEDOVERNIGHTRATECASHFLOW2_H
#define QC_DVE_CORE_COMPOUNDEDOVERNIGHTRATECASHFLOW2_H

#include <cashflows/Cashflow.h>
#include <asset_classes/InterestRateIndex.h>
#include <asset_classes/QCAct360.h>
#include <asset_classes/QCLinearWf.h>


namespace QCode::Financial {
    class CompoundedOvernightRateCashflow2 : public Cashflow {
    public:
        typedef std::tuple<
                std::string,                  /* Start date */
                std::string,                  /* End date */
                std::string,                  /* Settlement date */
                double,                       /* Nominal */
                double,                       /* Amortization */
                double,                       /* Interest */
                bool,                         /* Amortization is cashflow */
                double,                       /* Total cashflow */
                std::string,                  /* Nominal currency */
                std::string,                  /* Interest rate index code */
                std::string,                  /* Interest rate type*/
                double,                       /* Spread */
                double,                       /* Gearing */
                double                        /* Interest rate value */
        >
        CompoundedOvernightRateCashflow2Wrapper;

        CompoundedOvernightRateCashflow2(
                std::shared_ptr<InterestRateIndex> index,
                const QCDate &startDate,
                const QCDate &endDate,
                const QCDate &settlementDate,
                const std::vector<QCDate> &fixingDates,
                double nominal,
                double amortization,
                bool doesAmortize,
                shared_ptr <QCCurrency> notionalCurrency,
                double spread,
                double gearing,
                const QCInterestRate& interestRate,
                unsigned int eqRateDecimalPlaces,
                unsigned int lookback,
                unsigned int lockout);

        // Overriding methods in Cashflow
        double amount() override;

        shared_ptr <QCCurrency> ccy() override;

        virtual shared_ptr <QCCurrency> settlementCurrency();

        QCDate date() override;

        // Methods specific to this class
        [[nodiscard]] std::string getType() const override;

        [[nodiscard]] const QCDate& getStartDate() const;

        [[nodiscard]] const QCDate& getEndDate() const;

        [[nodiscard]] const QCDate& getSettlementDate() const;

        [[nodiscard]] const std::vector<QCDate>& getFixingDates() const;

        [[nodiscard]] double getNotional() const;

        [[nodiscard]] double getAmortization() const;

        double interestFromSpread();

        double interest(const TimeSeries &fixings);

        double fixing(const TimeSeries &fixings);

        double accruedInterest(const QCDate &fecha, const TimeSeries &fixings);

        double accruedFixing(const QCDate &fecha, const TimeSeries &fixings);

        [[nodiscard]] bool doesAmortize() const;

        std::shared_ptr<QCode::Financial::InterestRateIndex> getInterestRateIndex();

        [[nodiscard]] std::string getInterestRateIndexCode() const;

        [[nodiscard]] double getSpread() const;

        [[nodiscard]] double getGearing() const;

        [[nodiscard]] std::vector<double> getAmountDerivatives() const;

        void setNotional(double notional);

        void setAmortization(double amortization);

        void setFixings(const TimeSeries &fixings);

        [[nodiscard]] TimeSeries getFixings() const;

        virtual double settlementAmount();

        std::string getTypeOfRate();

        [[nodiscard]] unsigned int getEqRateDecimalPlaces() const;

        [[nodiscard]] unsigned int getLookBack() const;

        [[nodiscard]] unsigned int getLockOut() const;

        void setAmountDerivatives(std::vector<double> amountDerivatives);

        void setInitialDateWf(double wf);

        void setEndDateWf(double wf);

        std::shared_ptr<CompoundedOvernightRateCashflow2Wrapper> wrap();

        ~CompoundedOvernightRateCashflow2() override = default;

    protected:
        /** @brief	The interest rate index. It is always Lin ACT360. */
        QCInterestRate _rate = QCInterestRate(
                0.0,
                std::make_shared<QCAct360>(QCAct360()),
                std::make_shared<QCLinearWf>(QCLinearWf()));

        std::shared_ptr<InterestRateIndex> _index;

        std::map<QCDate, QCDate> _indexEndDates;

        QCInterestRate _interestRate;

        double _initialDateWf{};

        double _endDateWf{};

        double _getFixingWf(QCDate &fecha, const QCode::Financial::TimeSeries &fixings);

        /** @brief	The default amount of decimal places for TNA. */
        unsigned int _eqRateDecimalPlaces;

        /** @brief	Stores the derivatives of amount() with respect to rates belonging to zero
        * coupon curve.
        */
        std::vector<double> _amountDerivatives;

        unsigned int _lookback;

        unsigned int _lockout;

        /** @brief	The start date */
        QCDate _startDate;

        /** @brief	The end date */
        QCDate _endDate;

        /** @brief	The settlement date */
        QCDate _settlementDate;

        /** @brief	List of dates corresponding to fixing dates. In this case only contains start date */
        std::vector<QCDate> _fixingDates;

        QCode::Financial::TimeSeries _fixedRates;

        /** @brief	The nominal */
        double _notional;

        /** @brief	The amortization */
        double _amortization;

        /** @brief	The interest amount calculated */
        double _interest;

        /** @brief	True if amortization is part of the cashflow */
        bool _doesAmortize;

        /** @brief	The nominal currency. It is always CLP. */
        shared_ptr <QCCurrency> _notionalCurrency;

        /** @brief	The spread */
        double _spread;

        /** @brief	The gearing */
        double _gearing;

        /**
         * @fn	bool IcpClpCashflow::_validate();
         *
         * @brief	Validates that the object is instantiated properly.
         *
         * @author	Alvaro Díaz V.
         * @date	24/09/2018
         *
         * @return	True if it succeeds, false if it fails.
         */
        bool _validate();

        /** @brief	Message that describes errors encountered when constructing the object */
        std::string _validateMsg;

        void _fillIndexEndDates();

        double _calculateInterest(double rateValue, QCDate &fecha);

        [[nodiscard]] double _getRateValue() const;


    };

} // QCode
// Financial

#endif //QC_DVE_CORE_COMPOUNDEDOVERNIGHTRATECASHFLOW2_H
